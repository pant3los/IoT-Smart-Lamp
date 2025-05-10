#include <MKRWAN.h>
#include <ArduinoJson.h>

LoRaModem modem;
const char appEui[] = "#####";
const char appKey[] = "####"; 

#define LDR_PIN A1
#define PIR_PIN 6
#define RED_PIN 10
#define GREEN_PIN 9
#define BLUE_PIN 11

String currentMode = "off";
int rgbR = 0, rgbG = 0, rgbB = 0;
int movement = 0;
unsigned long lastPartyChange = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Initializing LoRa module...");
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start LoRa module");
    while (1);
  }

  Serial.print("Module version: ");
  Serial.println(modem.version());
  Serial.print("Device EUI: ");
  Serial.println(modem.deviceEUI());

  Serial.println("Joining TTN...");
  if (!modem.joinOTAA(appEui, appKey)) {
    Serial.println("Failed to join TTN");
    while (1);
  }

  Serial.println("Successfully joined TTN!");

  pinMode(LDR_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  setRGB(255, 255, 255);
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);
  int pirValue = digitalRead(PIR_PIN);

  Serial.print("LDR Value: ");
  Serial.println(ldrValue);
  Serial.print("PIR Value: ");
  Serial.println(pirValue);

  if (currentMode == "on") {
    setRGB(0, 0, 0);

  } else if (currentMode == "off") {
    setRGB(255, 255, 255);

  } else if (currentMode == "detection") {
    if (movement == 1) {
      setRGB(0, 0, 0);
    } else {
      setRGB(255, 255, 255);
    }

  } else if (currentMode == "rgb") {
    setRGB(rgbR, rgbG, rgbB);

  } else if (currentMode == "party") {
    // Random colors quickly (every 200ms)
    if (millis() - lastPartyChange > 200) {
      int r = random(256);
      int g = random(256);
      int b = random(256);
      setRGB(r, g, b);
      lastPartyChange = millis();
    }
  }

  String payload = "L:" + String(ldrValue) + ",P:" + String(pirValue);
  modem.setPort(1);
  modem.beginPacket();
  modem.print(payload);
  int err = modem.endPacket(true);
  if (err > 0) {
    Serial.println("Uplink sent successfully!");
  } else {
    Serial.print("Failed to send uplink. Error: ");
    Serial.println(err);
  }

  delay(5000);
  if (modem.available()) {
    int downlinkSize = modem.available();
    uint8_t downlink[downlinkSize];
    modem.readBytes(downlink, downlinkSize);

    Serial.print("Received downlink: ");
    for (int i = 0; i < downlinkSize; i++) {
      Serial.print((char)downlink[i]);
    }
    Serial.println();

    processDownlink(downlink, downlinkSize);
  } else {
    Serial.println("No downlink received.");
  }

  delay(10000);
}

void processDownlink(uint8_t *payload, int size) {
  String jsonString;
  for (int i = 0; i < size; i++) {
    jsonString += (char)payload[i];
  }

  Serial.print("Received JSON: ");
  Serial.println(jsonString);

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    return;
  }

  if (doc.containsKey("mode")) {
    currentMode = (const char*)doc["mode"];
  }

  if (currentMode == "detection") {
    movement = doc["movement"] | 0;
  }

  if (currentMode == "rgb") {
    rgbR = doc["R"] | 0;
    rgbG = doc["G"] | 0;
    rgbB = doc["B"] | 0;
  }

  Serial.print("Set mode to: ");
  Serial.println(currentMode);
}

void setRGB(int red, int green, int blue) {
  if (red == 255 && green == 255 && blue == 255) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  } else {
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
  }
}
