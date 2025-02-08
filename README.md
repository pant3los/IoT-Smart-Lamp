# 📡 Smart Lighting IoT System 🌍💡

This project is an **automated lighting system** based on **IoT technology**, designed for **smart cities** and **home automation**. The system uses **light sensors (LDR) and motion sensors (PIR)** to intelligently control lighting based on environmental conditions. The decision-making process takes place in the **cloud**, ensuring efficient energy management.

---

## **📁 Project Structure**
```
/Smart-Lighting-IoT
│── /mkrwan                # LoRaWAN-based implementation (The Things Network)
│   │── mkrwan.ino         # Firmware for LoRaWAN IoT device
│   │── payloadformatterdownlink.txt # Downlink payload formatter for TTN
│   │── mqtt.py            # MQTT-based communication script for TTN
|   │── payloadformatterUplink.txt # uplink payload formatter for TTN
│
│── /esp32                 # WiFi-based implementation (ThingSpeak)
│   │── esp32.ino          # Firmware for ESP32 IoT device (WiFi communication)
│   │── index.html         # Web interface for remote control
│
│── README.md              # Documentation and instructions
```

---

## **🚀 Features**
- **Automatic lighting control** based on real-time **light intensity** and **motion detection**.
- **LoRaWAN implementation** using **The Things Network (TTN)** for low-power, long-range communication.
- **WiFi implementation** using **ThingSpeak** for cloud integration and remote control.
- **Web Interface** for users to manually control and monitor the lighting system.
- **MQTT communication** for real-time data exchange between IoT devices and cloud platforms.

---

## **🛠️ Hardware Requirements**
- **ESP32** microcontroller (for WiFi-based system)
- **Arduino MKR WAN 1300** (for LoRaWAN-based system)
- **LDR (Light Dependent Resistor) sensor** (for light intensity detection)
- **PIR Motion Sensor** (for movement detection)
- **LED or Relay Module** (for controlling the light)
- **Resistors, jumper wires, and a breadboard**
- **Power supply (USB or battery)**

---

## **🖥️ Software Requirements**
- **Arduino IDE** (for programming ESP32 and MKR WAN 1300)
- **The Things Network (TTN)** account (for LoRaWAN setup)
- **ThingSpeak** account (for WiFi-based cloud storage)
- **Python 3** (for running the MQTT broker script)
- **Flask** (for the web server)
- **Paho MQTT** (for handling MQTT messages)

---

## **📡 Communication & Data Flow**
1. **Sensors (LDR & PIR) collect data** → Light intensity (L) & Movement (P).
2. Data is **transmitted to the cloud**:
   - **LoRaWAN implementation:** Sent to **TTN** via **mkrwan.ino**.
   - **WiFi implementation:** Sent to **ThingSpeak** via **esp32.ino**.
3. **Cloud processes the data** and makes a decision (ON/OFF).
4. The **decision is sent back to the IoT device**:
   - **TTN downlink message** (LoRaWAN).
   - **ThingSpeak API request** (WiFi).
5. The **IoT device activates or deactivates the light** accordingly.
6. Users can **manually control the system** via the **web interface (index.html)**.

---

## **📦 Installation & Setup**
### **1️⃣ LoRaWAN Implementation (MKR WAN 1300 + TTN)**
#### **1.1 Setup The Things Network (TTN)**
- Register a **TTN account**.
- Create an **IoT application** and add a **device** (MKR WAN 1300).
- Configure **uplink & downlink** messages.
- Add the **payload formatter** (`payloadformatterdownlink.txt`).

#### **1.2 Flash the firmware**
- Open `mkrwan.ino` in **Arduino IDE**.
- Install **LoRaWAN libraries**.
- Compile and upload the firmware to the **MKR WAN 1300**.

#### **1.3 Run MQTT Broker**
- Install **Paho MQTT**:
  ```
  pip install paho-mqtt flask
  ```
- Run the **MQTT script**:
  ```
  python mqtt.py
  ```

---

### **2️⃣ WiFi Implementation (ESP32 + ThingSpeak)**
#### **2.1 Setup ThingSpeak**
- Register a **ThingSpeak account**.
- Create a **new channel** for light & motion data.
- Note down the **API keys** for sending/receiving data.

#### **2.2 Flash the ESP32 Firmware**
- Open `esp32.ino` in **Arduino IDE**.
- Install **ESP32 board support**.
- Add **WiFi credentials** and **ThingSpeak API keys**.
- Compile and upload the firmware to the **ESP32**.

#### **2.3 Start Web Interface**
- Open `index.html` in a web browser.
- Control the lighting system remotely.

---

## **📊 Web Interface**
The web interface allows users to:
- **Turn lights ON/OFF manually**.
- **Enable detection mode** (automatic control via sensors).
- **Set RGB colors** (if LED supports RGB control).
- **View real-time sensor data**.

To use the interface:
1. Start the **Flask server** (`mqtt.py` must be running).
2. Open `index.html` in your browser.
3. Click the buttons to send commands.

---

## **💡 Future Enhancements**
- **AI-powered light predictions** using machine learning models.
- **Integration with smart home assistants** (Google Assistant, Alexa).
- **Battery optimization** for better energy efficiency.
- **Expansion to smart traffic lights and street lighting.**

---

## **🔗 References**
- [The Things Network](https://www.thethingsnetwork.org/)
- [ThingSpeak](https://thingspeak.com/)
- [Arduino MKR WAN 1300 Docs](https://docs.arduino.cc/hardware/mkr-wan-1300)
- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)

---

🚀 **Smart cities need smart solutions! Let's build a sustainable future together!** 🌍💡
