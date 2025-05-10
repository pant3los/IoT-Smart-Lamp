import paho.mqtt.client as mqtt
import json
import base64
from flask import Flask, request
import threading
import time

MQTT_BROKER = ""
MQTT_PORT = 
MQTT_USERNAME = ""  # Replace with yours data
MQTT_PASSWORD = "" 
DEVICE_ID = ""

UPLINK_TOPIC = f"v3/{MQTT_USERNAME}/devices/{DEVICE_ID}/up"
DOWNLINK_TOPIC = f"v3/{MQTT_USERNAME}/devices/{DEVICE_ID}/down/push"

app = Flask(__name__)

server_current_mode = "off"   # Track the current mode
last_movement = 0             # Store last computed movement
stop_thread = False           # Control flag for periodic thread

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe(UPLINK_TOPIC)
    print(f"Subscribed to {UPLINK_TOPIC}")

def send_downlink(mode, r=None, g=None, b=None, movement=None):
    payload = {"mode": mode}
    if mode == "rgb":
        payload["R"] = r or 0
        payload["G"] = g or 0
        payload["B"] = b or 0
    if mode == "detection" and movement is not None:
        payload["movement"] = movement

    frm_payload = base64.b64encode(json.dumps(payload).encode()).decode()
    downlink_data = {
        "downlinks": [
            {
                "f_port": 1,
                "frm_payload": frm_payload,
                "priority": "NORMAL"
            }
        ]
    }
    client.publish(DOWNLINK_TOPIC, json.dumps(downlink_data))
    print(f"Downlink ({mode}) sent with payload: {payload}")

def on_message(client, userdata, msg):
    global last_movement
    print(f"Received message on {msg.topic}")
    uplink = json.loads(msg.payload.decode())
    print(json.dumps(uplink, indent=4))

    decoded = uplink.get("uplink_message", {}).get("decoded_payload", {})
    L = decoded.get("L")
    P = decoded.get("P")

    # If current mode is detection, apply logic and send downlink accordingly
    if server_current_mode == "detection" and L is not None and P is not None:
        if L > 900 and P == 1:
            last_movement = 1
        else:
            last_movement = 0
        # Send immediate downlink with movement after receiving uplink
        send_downlink("detection", movement=last_movement)

def set_server_mode(mode):
    global server_current_mode, stop_thread
    server_current_mode = mode
    print(f"Server mode set to: {server_current_mode}")

    if server_current_mode == "detection":
        stop_thread = False
        # Start a new thread for periodic downlink sends if not already started
        threading.Thread(target=periodic_detection_downlink, daemon=True).start()
    else:
        # If not detection mode, stop sending periodic downlinks
        stop_thread = True

@app.route("/set_mode", methods=["POST"])
def set_mode():
    mode = request.form.get("mode")
    if mode == "rgb":
        r = int(request.form.get("r", 0))
        g = int(request.form.get("g", 0))
        b = int(request.form.get("b", 0))
        set_server_mode(mode)
        # For RGB, we can send downlink immediately
        send_downlink(mode, r, g, b)
    elif mode == "detection":
        # Do not send downlink here, just set mode
        set_server_mode(mode)
        # The periodic thread and on_message logic will handle sending downlinks with movement
    else:
        # For other modes (on, off, party), we can send downlink directly
        set_server_mode(mode)
        send_downlink(mode)
    return "OK"

def periodic_detection_downlink():
    # Every 5 seconds, if still in detection mode, send downlink with last_movement
    while not stop_thread:
        if server_current_mode == "detection":
            send_downlink("detection", movement=last_movement)
        time.sleep(5)

client = mqtt.Client()
client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
client.on_connect = on_connect
client.on_message = on_message
client.tls_set()
client.connect(MQTT_BROKER, MQTT_PORT, 60)
client.loop_start()

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
