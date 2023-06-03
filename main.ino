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
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

int currentWidget = 0;
float interval = 0;
float lastChange = 0;

char weatherValues[4][10];
char clockValues[3][10];

AsyncWebServer server(80);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);

const char* weather_base_url = "https://api.openweathermap.org/data/2.5/weather?appid=80a0b6e9e8019ea97ceb2137ba302c7b&units=metric&q=";
const char* stock_base_url = "https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&apikey=56VD1DVXPXNQ8MB5&interval=5min&symbol=";
const char* time_url = "https://timeapi.io/api/Time/current/zone?timeZone=Europe/Helsinki";

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


void widgetLoop(void* parameter){

  while(true){

    String widgets[3] = {"clock", "" , ""};
    int widgetCount = 1;

    if(SPIFFS.exists("/weather.txt")){
      widgets[widgetCount] = "weather";
      widgetCount++;
    }
    if(SPIFFS.exists("/text.txt")){
      widgets[widgetCount] = "text";
      widgetCount++;
    }


    if(widgets[currentWidget] == "text"){

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

    if(widgets[currentWidget] == "clock" && !(strcmp(clockValues[1], "") == 0)){
        clockWidget(clockValues[0], clockValues[1], clockValues[2]);
    }
    if(widgets[currentWidget] == "weather" && !(strcmp(weatherValues[1], "") == 0)){
        weatherWidget(weatherValues[2], weatherValues[3], weatherValues[1]);
    }


  // Change widget automatically if interval is set by user
  lastChange++;
  if(lastChange >= interval && interval != 0){
    lastChange = 0;
    currentWidget++;
    if(currentWidget == widgetCount){
      currentWidget = 0;
    }
  }

  vTaskDelay(pdMS_TO_TICKS(1000)); 
  }
}

void requestLoop(void* parameter){

  while(true){
    
    // Check wifi connection and reconnect if necessary 
    if(!(WiFi.status() == WL_CONNECTED)){
      textWidget(80, 65, 50, "Connection", "Lost", "", u8g2_font_crox1hb_tf);
      String address = findNetwork();
      textWidget(80, 65, 50, "Server", address.substring(0, 7).c_str(), address.substring(7, address.length()).c_str(), u8g2_font_crox1hb_tf);
      delay(2000);
    }

    // GET TIME DATA
    String payload = makeRequest(time_url);

    if(payload != "null"){
      StaticJsonDocument<1024> responseDoc;
      DeserializationError error = deserializeJson(responseDoc, payload.c_str());
      if (error) {
        Serial.print("Deserialization failed: ");
        Serial.println(error.c_str());
        return;  
      }
      int month = responseDoc["month"];
      int day = responseDoc["day"];

      char date[10]; 
      snprintf(date, sizeof(date), "%d.%d", day, month);

      char weekday[10];
      const char* dayOfWeek = responseDoc["dayOfWeek"].as<const char*>();
      snprintf(weekday, sizeof(weekday), "%s", dayOfWeek);

      strcpy(clockValues[0], responseDoc["dayOfWeek"]);
      strcpy(clockValues[1], responseDoc["time"]);
      strcpy(clockValues[2], date);
    }
    

    // GET WEATHER DATA
    if(SPIFFS.exists("/weather.txt")){

    strcpy(weatherValues[0], readFile(SPIFFS, "/weather.txt").c_str());

    char weather_url[200];  
    strcpy(weather_url, weather_base_url);
    strcat(weather_url, weatherValues[0]);

    String payload = makeRequest(weather_url);

    if(payload != "null"){
        StaticJsonDocument<1024> responseDoc;
        DeserializationError error = deserializeJson(responseDoc, payload.c_str());
        if (error) {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
          return;
        }

        float temp = responseDoc["main"]["temp"];
        float wind = responseDoc["wind"]["speed"];
        
        // Description
        strcpy(weatherValues[1], responseDoc["weather"][0]["main"]);

        // Temp
        char tempString[10]; 
        snprintf(tempString, sizeof(tempString), "%dc", lround(temp));
        strcpy(weatherValues[2], tempString);

        // Wind
        char windString[10]; 
        snprintf(windString, sizeof(windString), "%dm/s", lround(wind));
        strcpy(weatherValues[3], windString);
      
    }
    }

    vTaskDelay(pdMS_TO_TICKS(5000)); 
  }
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
      if(makeRequest(weather_host.c_str()) != "null"){
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

  xTaskCreate(requestLoop, "RequestLoop", 20000, NULL, 1, NULL);
  xTaskCreate(widgetLoop, "WidgetLoop", 20000, NULL, 1, NULL);

}

void loop() {
  // Empty because freeRTOS is used
}
