#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include "SPIFFS.h"
#include "header.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <AsyncTCP.h>
#include <Wire.h>
#include <U8g2lib.h>

int currentWidget = 0;
int interval = 0;
int lastChange = 0;

AsyncWebServer server(80);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);

String weather_base_url = "https://api.openweathermap.org/data/2.5/weather?appid=80a0b6e9e8019ea97ceb2137ba302c7b&units=metric&q=";
String stock_base_url = "https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&apikey=56VD1DVXPXNQ8MB5&interval=5min&symbol=";
String time_base_url = "https://timeapi.io/api/Time/current/zone?timeZone=";


String choosePlaceholderValues(const char* widgetName, const char* element) {
  String filePath = "/";
  filePath += widgetName;
  filePath += ".txt";

  if(SPIFFS.exists(filePath)){
    if(String(element) == "button"){
      return String("-");
    }
    else{
      return String("/remove");
    }
    
  }
  else{
    if(String(element) == "button"){
      return String("+");
    }
    else{
      return String("/add");
    }
  }
}

// Replace placeholder values in html with stored values
String processor(const String& var){

  if(var == "weatherFormType"){
    return choosePlaceholderValues("weather", "form");
  }
  if(var == "textFormType"){
    return choosePlaceholderValues("text", "form");
  }
  if(var == "weatherButtonType"){
    return choosePlaceholderValues("weather", "button");
  }
  if(var == "textButtonType"){
    return choosePlaceholderValues("text", "button");
  }
  if(var == "weatherInputPlaceholder"){
    if(SPIFFS.exists("/weather.txt")){
      return readFile(SPIFFS, "/weather.txt");
    }
    else{
      return "City";
    }
  }
  if(var == "textInputPlaceholder"){
    if(SPIFFS.exists("/text.txt")){
      return readFile(SPIFFS, "/text.txt");
    }
    else{
      return "Your Text";
    }
  }
  if(var == "IntervalButton"){
    if(interval > 0){
      return String(interval) + " seconds";
    }
    else{
      return String("No Interval");
    }
  }

  return String();  
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void textWidget(int y1, int y2, int y3, const char* param1, const char* param2, const char* param3, const uint8_t *font){
  u8g2.clearBuffer();
  u8g2.setFont(font);
  u8g2.drawStr(y1, 0, param1);
  u8g2.drawStr(y2, 0, param2);
  u8g2.drawStr(y3, 0, param3);
  u8g2.sendBuffer();
}

void weatherWidget(const char* temp, const char* wind, const char* description){

  int iconNumber = 69;

  if(String(description) == "Drizzle" || String(description) == "Rain" || String(description) == "Thunder"){
    iconNumber = 67;
  }
  if(String(description) == "Clouds" || String(description) == "broken clouds"){
    iconNumber = 64;
  }

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
  u8g2.drawGlyph(60, 0, iconNumber);	
  u8g2.setFont(u8g2_font_crox1hb_tf);
  u8g2.drawStr(50, 0, temp);
  u8g2.drawStr(37, 0, wind);
  u8g2.sendBuffer();
}


void clockWidget(const char* param1, const char* param2, const char* param3){
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_crox1h_tf);
  u8g2.drawStr(82, 00, param1);
  u8g2.setFont(u8g2_font_crox3hb_tf);
  u8g2.drawStr(60, 00, param2);
  u8g2.drawStr(40, 00, param3);
  u8g2.sendBuffer();
}

void setup() {

  Serial.begin(115200);
  Wire.begin();
  SPIFFS.begin(true);
  u8g2.begin();
  u8g2.setFontDirection(1);
  u8g2.setFlipMode(1);

  textWidget(73, 52, 0, "Searching", "Wi-Fi", "", u8g2_font_crox1h_tf);

  String address = findNetwork();

  // Show server ip address on screen
  textWidget(80, 65, 50, "Server", address.substring(0, 7).c_str(), address.substring(7, address.length()).c_str(), u8g2_font_crox1hb_tf);
  delay(3000);

  // Start server and handle routes
  server.begin();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", panel_html, processor);
  });

  server.on("/cycle", HTTP_GET, [] (AsyncWebServerRequest *request) {
    currentWidget++;

    int widgetCount = 1;

    if(SPIFFS.exists("/weather.txt")){
      widgetCount++;
    }
    if(SPIFFS.exists("/text.txt")){
      widgetCount++;
    }

    if(currentWidget > (widgetCount - 1)){
      currentWidget = 0;
    }
    request->send_P(200, "text/html", panel_html, processor);
  });

  server.on("/setInterval", HTTP_GET, [] (AsyncWebServerRequest *request) {

    interval = request->getParam("seconds")->value().toInt();
    request->send_P(200, "text/html", panel_html, processor);
  });


  server.on("/add", HTTP_GET, [] (AsyncWebServerRequest *request) {

    String inputString;

    if (request->hasParam("weather")) {
      inputString = request->getParam("weather")->value();

      String weather_host = (weather_base_url + String(inputString));

      if(makeWeatherRequest(weather_host.c_str()) != "null"){
        writeFile(SPIFFS, "/weather.txt", inputString.c_str());
      }

    }
    else if (request->hasParam("text")) {
      inputString = request->getParam("text")->value();
      writeFile(SPIFFS, "/text.txt", inputString.c_str());
    }
    
    request->send_P(200, "text/html", panel_html, processor);
  });

  server.on("/remove", HTTP_GET, [] (AsyncWebServerRequest *request) {

    if (request->hasParam("weather")) {
      SPIFFS.remove("/weather.txt");
    }
    else if (request->hasParam("text")) {
      SPIFFS.remove("/text.txt");
    }
    
    request->send_P(200, "text/html", panel_html, processor);
  });

}

void loop() {

  // Check wifi connection and reconnect if necessary
  if(!(WiFi.status() == WL_CONNECTED)){
    textWidget(80, 65, 50, "Connection", "Lost", "", u8g2_font_crox1hb_tf);
    String address = findNetwork();
    delay(1000);
    textWidget(80, 65, 50, "Server", address.substring(0, 7).c_str(), address.substring(7, address.length()).c_str(), u8g2_font_crox1hb_tf);
    delay(3000);
  }


  String widgets[3] = {"clock"};
  int widgetCount = 1;

  if(SPIFFS.exists("/weather.txt")){
    widgets[widgetCount] = "weather";
    widgetCount++;
  }
  if(SPIFFS.exists("/text.txt")){
    widgets[widgetCount] = "text";
    widgetCount++;
  }


  // Make http request to api
  if(widgets[currentWidget] == String("text")){

    String customText = readFile(SPIFFS, "/text.txt");
    int length = customText.length();
    
    // Divide text to different lines depending on it's lenght

    switch (length)
    {
      case 0 ... 8: 
        textWidget(65, 0, 0, customText.c_str(), "", "", u8g2_font_crox1h_tf);
        break;
      case 9 ... 16:
        textWidget(80, 65, 50, 
        customText.substring(0, length / 2).c_str(), 
        customText.substring(length / 2, ((length / 2) * 2) + 1).c_str(), "", u8g2_font_crox1h_tf);
        break;
      case 17 ... 24:
        textWidget(65, 0, 0, 
        customText.substring(0, length / 3).c_str(),
        customText.substring(length / 3, (length / 3) * 2).c_str(), 
        customText.substring((length / 3) * 2, ((length / 3) * 3) + 1).c_str(), u8g2_font_crox1h_tf);
        break;
    }
    
  }

  else{
  
  HTTPClient http;

  String serverPath;
  if(widgets[currentWidget] == "clock"){
    serverPath = time_base_url + "Europe/Helsinki";

  }
  else{
    serverPath = weather_base_url + readFile(SPIFFS, "/weather.txt");
  }

  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode>0) {
    String payload = http.getString();

    // Handle the json object from api
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
      // Handle deserialization errors
      Serial.print("Deserialization failed: ");
      Serial.println(error.c_str());
      return;  
    }


    if(widgets[currentWidget] == "clock"){
      int month = doc["month"];
      int day = doc["day"];

      String weekday = doc["dayOfWeek"];
      String date = String(day) + "." + String(month);
      String time = doc["time"];

      if(time != "null"){
        clockWidget(weekday.c_str(), time.c_str(), date.c_str());
      }
      
    }
    else if(widgets[currentWidget] == "weather"){

      float temp = doc["main"]["temp"];
      float wind = doc["wind"]["speed"];
      
      String desc = doc["weather"][0]["main"];
      String tempString = String(lround(temp)) + "c";
      String windString = String(lround(wind)) + " m/s";

      
      if(desc != "null"){
        weatherWidget(tempString.c_str(), windString.c_str(), desc.c_str());
      }
      
    }
  }
  }

  

  if(interval > 0){

    lastChange++;

    if(lastChange > interval){
      lastChange = 0;

      currentWidget++;

      int widgetCount = 1;

      if(SPIFFS.exists("/weather.txt")){
        widgetCount++;
      }
      if(SPIFFS.exists("/text.txt")){
        widgetCount++;
      }

      if(currentWidget > (widgetCount - 1)){
        currentWidget = 0;
      }
    }
    
  }
  delay(1000);
}
