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
      if(WiFi.encryptionType(i) == WIFI_AUTH_OPEN && WiFi.SSID(i) != "Sony 75 Tv.v," && WiFi.SSID(i) != "HP-Print-FD-LaserJet Pro MFP"){
        WiFi.begin(WiFi.SSID(i).c_str());
        delay(1000);
        break;
      }
    }
    delay(1000);
  }

  return WiFi.localIP().toString();
  
}

String makeRequest(const char* host, const char* widgetType){
  HTTPClient http;
  String serverPath = host;

  char *params[] = {};

  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();
  
  if (httpResponseCode>0) {
    String payload = http.getString();

    // PARSE JSON
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);


    if(widgetType == "clock"){
      int month = doc["month"];
      int day = doc["day"];
      int hour = doc["hour"];
      int minute = doc["minute"];

      char buffer[7];  
      String date = String(String(day) + "." + String(month));
      String clock = String(String(hour) + ":" + String(minute));


    }
    else if(widgetType = "weather"){
      int temp = doc["temp"];
      int wind = doc["wind"];
      char buffer[7];  
      String tempString = String(temp);
      String tempString2 = String(tempString + "c");
      String windString = String(wind);
      String windString2 = String(windString + " m/s");

    }

    if(doc["message"] == "city not found"){
      return "null";
    }

    return payload;
  }
  return "";
}
