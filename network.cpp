#include <ESPAsyncWebServer.h>
#include <FS.h>
#include "SPIFFS.h"
#include "header.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <AsyncTCP.h>


String findNetwork(){

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  // Wait until open wifi found
  while(!(WiFi.status() == WL_CONNECTED)){
    int n = WiFi.scanNetworks();

    for (int i = 0; i < n; ++i) {
      if(WiFi.encryptionType(i) == WIFI_AUTH_OPEN && WiFi.SSID(i) != "HP-Print-FD-LaserJet Pro MFP"){
        WiFi.begin(WiFi.SSID(i).c_str());
        Serial.println(WiFi.SSID(i));
        delay(1000);
        break;
      }
    }
    delay(1000);
  }


  return WiFi.localIP().toString();
  
}

String makeRequest(const char* host){

  HTTPClient http;
  
  http.begin(host);
  int httpResponseCode = http.GET();
  
  if (httpResponseCode>0) {
    String payload = http.getString();

    http.end();

    StaticJsonDocument<1024> responseDoc;
    DeserializationError error = deserializeJson(responseDoc, payload);
    if (error) {
        Serial.print("Deserialization failed: ");
        Serial.println(error.c_str());
    }

    if(responseDoc["message"] == "city not found"){
      return "null";
    }

    return payload;
  }
  else{
    http.end();
    return "null";
  }
}
