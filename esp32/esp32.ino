/*
   ESP32 code for connecting to WiFi, reading from LDR and PIR,
   sending data to ThingSpeak, and retrieving the field3 value to control an LED.
*/

#include <WiFi.h>
#include <ArduinoJson.h> // Install this library via Library Manager

// Sensor and LED pins
const int ldrPin = 34;    // Analog input for LDR
const int pirPin = 4;     // Digital input for PIR
const int redPin = 2;     // PWM output for red LED
const int greenPin = 18;  // PWM output for green LED
const int bluePin = 5;    // PWM output for blue LED

// WiFi credentials
const char* ssid = "###";
const char* password = "###";

// ThingSpeak API key
String apiKey = "###";
const String readAPIKey = "###"; // Replace with your Read API key
const int channelID = 2777882;                // Replace with your channel ID

// ThingSpeak server settings
const char* host = "api.thingspeak.com";
const int httpPort = 80;

void setup() {
  Serial.begin(115200);

  // Set pin modes
  pinMode(ldrPin, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, HIGH);

  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Read sensors
  int ldrValue = analogRead(ldrPin);
  int pirValue = digitalRead(pirPin);

  Serial.print("LDR: ");
  Serial.print(ldrValue);
  Serial.print(" | PIR: ");
  Serial.println(pirValue);

  // Send data to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    if (client.connect(host, httpPort)) {
      // Construct the GET request
      String url = "/update?api_key=" + apiKey + "&field1=" + String(ldrValue) + "&field2=" + String(pirValue);

      Serial.print("Sending request to: ");
      Serial.println(host);

      // Send the HTTP GET request
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");

      // Wait for response
      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 5000) {
          Serial.println(">>> Client Timeout !");
          client.stop();
          return;
        }
      }

      // Read and print the response
      while (client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
      }
      Serial.println();
      Serial.println("Data sent to ThingSpeak.");
    } else {
      Serial.println("Connection to host failed!");
    }
  } else {
    Serial.println("WiFi not connected!");
  }

  // Retrieve the latest value of field3
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    if (client.connect(host, httpPort)) {
      // Construct the GET request for field3
      String url = "/channels/" + String(channelID) + "/fields/3/last.json?api_key=" + readAPIKey;

      Serial.print("Fetching field3 from: ");
      Serial.println(url);

      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");

      // Wait for response
      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 5000) {
          Serial.println(">>> Client Timeout !");
          client.stop();
          return;
        }
      }

      // Parse the response to get field3 value
      String response = "";
      while (client.available()) {
        response += client.readString();
      }

      int jsonStart = response.indexOf("{");
      if (jsonStart > 0) {
        String jsonResponse = response.substring(jsonStart);
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, jsonResponse);

        if (!error) {
          // Extract field3 value
          const char* field3Val = doc["field3"];
          if (field3Val != NULL) {
            int ledCommand = atoi(field3Val);
            Serial.print("Field3 Value: ");
            Serial.println(ledCommand);

            // Take action based on field3 value
            if (ledCommand == 1) {
              digitalWrite(redPin, LOW); 
              digitalWrite(greenPin, LOW);
              digitalWrite(bluePin, LOW);
              Serial.println("LED ON");

            } else {
              digitalWrite(redPin, HIGH); 
              digitalWrite(greenPin, HIGH);
              digitalWrite(bluePin, HIGH);
              Serial.println("LED OFF");
            }
          } else {
            Serial.println("Field3 not found in response");
          }
        } else {
          Serial.print("JSON parse error: ");
          Serial.println(error.c_str());
        }
      } else {
        Serial.println("No JSON found in response");
      }
    } else {
      Serial.println("Connection to ThingSpeak failed!");
    }
  }

  // Wait 20 seconds before the next operation
  delay(5000);
}
