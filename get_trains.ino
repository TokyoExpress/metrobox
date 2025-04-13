#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

const char* ssid     = "ResistanceCamp-DC";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "embarctv";     // The password of the Wi-Fi network

void setup() {
  Serial.begin(9600);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println("...");

  // int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print('.');
    // Serial.print(++i); Serial.print(' ');
    // Serial.print(WiFi.status()); Serial.print(' ');
  }
  Serial.println();
  Serial.println("CONNECTED");
  bool ready = false;
  String incomingString;
  while (!ready) {
    if (Serial.available()) {
        incomingString = Serial.readStringUntil('\n');
        if (incomingString == "READY\r") {
          ready = true;
        }
      }
  }
  // Serial.print("IP address:\t");
  // Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}

String groups[] = {"1", "2"};
int group_idx = 0;

void loop() {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;
    char payload[1200];

    if (http.begin(client, "http://api.wmata.com/StationPrediction.svc/json/GetPrediction/C05")) {  // K01 - Court House, C05 - Rosslyn

      http.addHeader("api_key", "f5fdc3d819ad4e58b52c3746060d9f76");

      // Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        // Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {
          Stream *stream = http.getStreamPtr(); // Get stream object

          StaticJsonDocument<512> doc;
          DeserializationError error = deserializeJson(doc, *stream);
          if (error) {
            // Serial.print("deserializeJson() failed: ");
            // Serial.println(error.f_str());
            Serial.println("ERROR");
            return;
          }

          int idx = 0;
          Serial.print(groups[group_idx]+':');
          for (JsonObject Train : doc["Trains"].as<JsonArray>()) {
            if (Train["Group"].as<String>() == groups[group_idx]) {
              Serial.print(Train["Line"].as<String>() + ',');
              Serial.print(Train["Car"].as<String>() + ',');
              Serial.print(Train["Destination"].as<String>() + ',');
              Serial.print(Train["Min"].as<String>() + ',');
              idx++;
              if (idx == 3) {
                break;
              }
            }
          }

          group_idx = (group_idx + 1) % 2;

          // fill missing lines
          for (int i = idx; i < 3; i++) {
            Serial.print(",,,,");
          }
          Serial.println();
        }
      } else {
        // Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        Serial.println("ERROR");
      }

      http.end();
    } else {
      // Serial.println("[HTTP] Unable to connect");
      Serial.println("ERROR");
    }
  }
  // Serial.println('\n');
  delay(5000);
}