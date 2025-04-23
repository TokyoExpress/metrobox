#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>

ESP8266WebServer server(80);
StaticJsonDocument<512> doc;
WiFiManager wm;
String stationInfoString;
char apiKey[32];
char stationCode[3] = "A01";

void setup() {
  Serial.begin(9600);

  WiFiManagerParameter apiKeyParam("api_key", "WMATA API key");
  WiFiManagerParameter stationCodeParam("station_code", "Metro Station Code");
  wm.addParameter(&apiKeyParam);
  wm.addParameter(&stationCodeParam);

  wm.autoConnect("Metrobox", "metrobox");
  apiKey = apiKeyParam.getValue();
  stationCode = stationCodeParam.getValue();
  sendStationInfo();

  Serial.println(stationInfoString);

  Serial.println();
  Serial.println("[CONNECTED]");

  bool ready = false;
  String incomingString;
  while (!ready) {
    if (Serial.available()) {
        incomingString = Serial.readStringUntil('\n');
        if (incomingString == "[READY]\r") {
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

    if (http.begin(client, "http://api.wmata.com/StationPrediction.svc/json/GetPrediction/" + String(stationCode))) {

      http.addHeader("api_key", String(apiKey));

      int httpCode = http.GET();

      if (httpCode != HTTP_CODE_OK) {
        Serial.println("[ERROR]");
      } else {
        Stream *stream = http.getStreamPtr(); // Get stream object

        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, *stream);
        if (error) {
          Serial.println("[ERROR]");
          return;
        }

        int idx = 0;
        Serial.print("T:");
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
      http.end();
    } else {
      Serial.println("[ERROR]");
    }
  } else {
    Serial.println("[ERROR]")
  }
  delay(5000);
}

void sendStationInfo() {

  if ((WiFi.status() != WL_CONNECTED)) {
    Serial.print("[ERROR]");
    return;
  }

  WiFiClient client;
  HTTPClient http;
  char payload[1200];
  
  if (http.begin(client, "http://api.wmata.com/Rail.svc/json/jStationInfo?StationCode=" + String(stationCode))) {

    http.addHeader("api_key", String(apiKey));
    int httpCode = http.GET();

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        Stream *stream = http.getStreamPtr();

        DeserializationError error = deserializeJson(doc, *stream);
        if (error) {
          Serial.println("[ERROR]");
          return;
        }

        Serial.print("S:");
        Serial.print(doc["Name"].as<String>() + ',');
        for (String key : {"LineCode1", "LineCode2", "LineCode3", "LineCode4"}) {
          if (!doc[key].isNull()) {
            Serial.print(doc[key].as<String>() + ',');
          }
        }
      }
    } else {
      // Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      Serial.println("[ERROR]");
    }

    http.end();
  } else {
    // Serial.println("[HTTP] Unable to connect");
    Serial.println("[ERROR]");
  }
}

void configure() {
  wm.startConfigPortal("Metrobox", "metrobox");

  apiKey = String(apiKeyParam.getValue());
  stationCode = String(stationCodeParam.getValue());

  if ((WiFi.status() == WL_CONNECTED)) {
    stationInfoString = getStationInfo();
  } else {
    Serial.print("[ERROR]");
  }
}