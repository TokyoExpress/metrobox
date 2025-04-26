#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>

ESP8266WebServer server(80);
StaticJsonDocument<512> doc;

WiFiManager wm;
char apiKey[34] = "";
char stationCode[4] = "C01";
WiFiManagerParameter apiKeyParam("api_key", "WMATA API key", apiKey, 32);
WiFiManagerParameter stationCodeParam("station_code", "Metro Station Code", stationCode, 3);

bool shouldSaveConfig = false;

void handleReset() {
  server.send(200, "text/plain", "Resetting configuration.");
  Serial.println("[RESET]");
  wm.resetSettings();
  ESP.restart();
}

void saveParametersFlag() {
  shouldSaveConfig = true;
}

void sendAPMessage(WiFiManager* wm) {
  Serial.println("[AP]");
}

void saveParameters() {
  Serial.println("Saving config parameters");
  DynamicJsonDocument json(1024);
  json["apiKey"] = apiKey;
  json["stationCode"] = stationCode;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }

  serializeJson(json, Serial);
  Serial.println();
  serializeJson(json, configFile);
  configFile.close();
}

void setup() {
  Serial.begin(9600);

  wm.setSaveConfigCallback(saveParametersFlag);
  wm.setAPCallback(sendAPMessage);
  wm.addParameter(&apiKeyParam);
  wm.addParameter(&stationCodeParam);
  // wm.setClass("invert");
  wm.setTitle("Metrobox");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);

        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get());
        serializeJson(json, Serial);
        if ( ! deserializeError ) {
          Serial.println("\nparsed json");
          strcpy(apiKey, json["apiKey"]);
          strcpy(stationCode, json["stationCode"]);
        } else {
          Serial.println("[ERROR] failed to load json config");
        }
        configFile.close();
      }
    }
  }

  if (!wm.autoConnect("Metrobox", "metrobox")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }

  server.on("/reset", handleReset);
  server.begin();

  if (shouldSaveConfig) {
    strcpy(apiKey, apiKeyParam.getValue());
    strcpy(stationCode, stationCodeParam.getValue());
    saveParameters();
  }

  Serial.println("apiKey: " + String(apiKey));
  Serial.println("stationCode: " + String(stationCode));
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("\n[CONNECTED]");
  

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

  sendStationInfo();
}

String groups[] = {"1", "2"};
int group_idx = 0;

void loop() {
  server.handleClient();
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
        delay(5000);
        return;
      }

      Stream *stream = http.getStreamPtr();
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, *stream);
      if (error) {
        Serial.println("[ERROR]");
        delay(5000);
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

      http.end();
    } else {
      Serial.println("[ERROR]");
    }
  } else {
    Serial.println("[ERROR]");
  }
  delay(5000);
}

void sendStationInfo() {

  if ((WiFi.status() != WL_CONNECTED)) {
    Serial.println("[ERROR]");
    return;
  }

  WiFiClient client;
  HTTPClient http;
  char payload[1200];
  
  if (http.begin(client, "http://api.wmata.com/Rail.svc/json/jStationInfo?StationCode=" + String(stationCode))) {

    http.addHeader("api_key", String(apiKey));
    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
      Serial.println("[ERROR]");
      return;
    }

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
    Serial.println();

    http.end();
  } else {
    // Serial.println("[HTTP] Unable to connect");
    Serial.println("[ERROR]");
  }
}

// void configure() {
//   wm.startConfigPortal("Metrobox", "metrobox");

//   apiKey = String(apiKeyParam.getValue());
//   stationCode = String(stationCodeParam.getValue());

//   if ((WiFi.status() == WL_CONNECTED)) {
//     stationInfoString = getStationInfo();
//   } else {
//     Serial.print("[ERROR]");
//   }
// }