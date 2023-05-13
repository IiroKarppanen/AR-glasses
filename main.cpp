#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <AsyncTCP.h>
#include <FS.h>
#include "SPIFFS.h"
#include "header.h"
#include <ArduinoJson.h>

#define SUN	0
#define SUN_CLOUD  1
#define CLOUD 2
#define RAIN 3
#define THUNDER 4


const char panel_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>

<head>
  <title>AR Control Panel</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
  <script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
  <script src="https://cdn.jsdelivr.net/npm/popper.js@1.14.7/dist/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>
  <style>


    /* Grid Layout*/
    .content {
        width: 50vh;
        margin: auto;
        justify-content: space-evenly;
        grid-template-columns: auto auto auto;

    }

    @media screen and (max-width: 650px) {
        .content{
            width: 90vh;
        }
    }

    .add-widgets{
        border: 1px solid rgb(211, 211, 211); 
        border-radius: 15px;
        padding: 25px;
    }
    .current-widgets{
        border: 1px solid rgb(211, 211, 211); 
        border-radius: 15px;
        padding: 25px;
    }

    .widget-list {
        overflow: scroll;

    }
    
    /* Invidual elements */
    form {
        align-items: center;
        justify-content: center;
        text-align: center;
    }
    body {background-color: transparent;}
    button {
        word-wrap: break-word;
        max-width: 100%;
    }
    h1   {
        color: rgb(73, 73, 73);
        font-size: 25;
        font-weight: 300;
        text-align: center;
        padding: 20px;
        height: 10vh;
    }
    h2   {
        color: rgb(73, 73, 73);
        font-weight: 300;
        text-align: center;
        padding: 20px;
        font-size: 25px;
        border-top: 1px solid rgb(211, 211, 211); 
    }

     
  </style>
</head>

<body>
    <h1>Control Panel</h1>
    <div class="content">
        <div class="add-widgets">
            <div class="row">
                <div class="col text-center p-4">
                    <a href="/cycle"><button type="button" class="btn btn-primary rounded-lg">Show Next</button></a>
                </div>
            </div>
            <div class="row">
                <div class="col text-center p-4">
                    <label>Set interval to change widgets</label>
                    <div class="dropdown">
                        <button class="btn btn-dark dropdown-toggle" type="button" id="dropdownMenuButton" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
                            No interval
                        </button>
                        <div class="dropdown-menu w-100" aria-labelledby="dropdownMenuButton">
                            <a class="dropdown-item" href="#">10s</a>
                            <a class="dropdown-item" href="#">20s</a>
                            <a class="dropdown-item" href="#">30s</a>
                            <a class="dropdown-item" href="#">60s</a>
                            <a class="dropdown-item" href="#">300s</a>
                        </div>
                </div>
            </div>
            </div>
            <div class="row">
                <div class="col text-center p-4">
                    <h2>Widgets</h2>

                    <label>Clock Widget</label>
                    <div class="dropdown">
                        <button class="btn btn-dark dropdown-toggle" type="button" id="dropdownMenuButton" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
                            Timezone (UTC)
                        </button>
                        <div class="dropdown-menu w-100" aria-labelledby="dropdownMenuButton">
                            <a class="dropdown-item" href="#">-3</a>
                            <a class="dropdown-item" href="#">-2</a>
                            <a class="dropdown-item" href="#">-1</a>
                            <a class="dropdown-item" href="#">0</a>
                            <a class="dropdown-item" href="#">+1</a>
                            <a class="dropdown-item" href="#">+1</a>
                            <a class="dropdown-item" href="#">+2</a>
                            <a class="dropdown-item" href="#">+3</a>
                            <a class="dropdown-item" href="#">+4</a>
                            <a class="dropdown-item" href="#">+5</a>
                            <a class="dropdown-item" href="#">+6</a>
                            <a class="dropdown-item" href="#">+7</a>
                            <a class="dropdown-item" href="#">+8</a>
                        </div>
                        <button type="button" class="btn btn-success">&#65291</button>
                    </div>
                </div>
            </div>
            <div class="row">
                <div class="col text-center p-4">
                    <label>Weather Widget</label>
                    <form action="/get", "/add?moi=hei">
                        <div class="input-group">
                        <input class="form-control rounded"placeholder="City" name="inputString">
                        <button type="button" class="btn btn-success">&#65291</button>
                        </div>
                    </form>
                    <p class="text-danger">%invalidCity%</p>
                    %Invalid City Name%
               
                </div>
            </div>
            <div class="row">
                <div class="col text-center p-4">
                    <label>Text Widget</label>
                        <form action="/add", "/add?moi=hei">
                            <div class="input-group">
                            <input class="form-control rounded"placeholder="Your text" name="inputString">
                            <button type="button" class="btn btn-success">&#65291</button>
                            </div>
                        </form>           
                </div>
            </div>
            <div class="row">
                <div class="col text-center p-4">
                    <label>Stock Widget</label>
                        <form action="/add">
                            <div class="input-group">
                            <input class="form-control rounded"placeholder="Ticker" name="inputString">
                            <button type="button" class="btn btn-success">&#65291</button>
                            </div>
                        </form>    
                        <p class="text-danger">%invalidTicker%</p>       
                </div>
            </div>
           
        </div>
    </div>
</body>

</html>)rawliteral";


AsyncWebServer server(80);

int currentWidget = 0;
char *widgets[] = {};


const char* host = "https://arlasit-api.onrender.com/date?timeZone=Europe/Helsinki"; 
const char* host2 = "https://arlasit-api.onrender.com/weather?city=Espoo"; 

const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2"; 
const char* PARAM_INPUT_3 = "input3";

const char* PARAM_STRING = "inputString";
const char* PARAM_INT = "inputInt";
const char* PARAM_FLOAT = "inputFloat";


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  file.close();
  Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;   
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

// Replaces placeholder with stored values
String processor(const String& var){
  //Serial.println(var);
  Serial.println("THIS IS PROCESSOR FUNCTION");
  Serial.println(var);

  if(var == "inputString"){
    return readFile(SPIFFS, "/inputString.txt");
  }
  else if(var == "inputInt"){
    return readFile(SPIFFS, "/inputInt.txt");
  }
  else if(var == "inputFloat"){
    return readFile(SPIFFS, "/inputFloat.txt");
  }
  else if(var == "invalidCity"){
    return "Invalid City Name";
  }
  else if(var == "invalidTicker"){
    return "Invalid Stock Ticker";
  }
  return String();  
}

void setup() {
  Serial.begin(115200);
  SPIFFS.begin(true);

  //loadingWidget(); 

  Serial.println("Scanning network...");  
  Serial.println(findNetwork());

  //showIP(String(findNetwork()).c_str());

  server.begin();

  

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", panel_html, processor);
    //request->send(SPIFFS, "index.html", "text/html");
  });

  server.on("/cycle", HTTP_GET, [] (AsyncWebServerRequest *request) {
    currentWidget++;
    if(currentWidget > 1){
      currentWidget = 0;
    }
    request->send_P(200, "text/html", panel_html, processor);
  });

  server.on("/add", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    if (request->hasParam(PARAM_STRING)) {
      inputMessage = request->getParam(PARAM_STRING)->value();
      Serial.println(inputMessage.c_str());
      Serial.println(request->params());
      writeFile(SPIFFS, "/weather.txt", inputMessage.c_str());
    }
    else {
      inputMessage = "No message sent";
    }
    request->send_P(200, "text/html", panel_html, processor);
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
    if (request->hasParam(PARAM_STRING)) {
      inputMessage = request->getParam(PARAM_STRING)->value();
      printf(inputMessage.c_str());
      //writeFile(SPIFFS, "/inputString.txt", inputMessage.c_str());
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    //request->send_P(200, "text/html", panel_html, processor);
    request->send(200, "text/text", "Invalid city name");
  });
  server.onNotFound(notFound);
  server.begin();


}


void loop() {
  // To access your stored values on inputString, inputInt, inputFloat
  //String yourInputString = readFile(SPIFFS, "/inputString.txt");
  //Serial.print("*** Your inputString: ");
  //Serial.println(yourInputString);

  HTTPClient http;
  String serverPath = host;

  
  if(widgets[currentWidget] == "clock"){
    serverPath = host;
  }
  else if(widgets[currentWidget] == "weather"){
    serverPath = host2;
  }

  Serial.println(widgets[currentWidget]);
  Serial.println(serverPath.c_str());
  


  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();
  
  if (httpResponseCode>0) {
    String payload = http.getString();

    // PARSE JSON
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);


    if(currentWidget == 1){
      int year = doc["year"];
      int month = doc["month"];
      int day = doc["day"];
      int hour = doc["hour"];
      int minute = doc["minute"];

      char buffer[7];  
      String date = String(String(day) + "/" + String(month) + "/" + String(year));
      String clock = String(String(hour) + ":" + String(minute));

      //clockWidget(date.c_str(), clock.c_str(), "");
    }
    else if(currentWidget == 2){
      int temp = doc["temp"];
      int wind = doc["wind"];
      char buffer[7];  
      String tempString = String(temp);
      String tempString2 = String(tempString + "c");
      String windString = String(wind);
      String windString2 = String(windString + " m/s");

      //weatherWidget(tempString2.c_str(), windString2.c_str(), doc["description"]);
    }

  }
  else{
    Serial.println("HTTP ERROR CODE: ");
    Serial.println(httpResponseCode);
    Serial.println(WiFi.isConnected());
    if(!(WiFi.status() == WL_CONNECTED)){
      Serial.println("WIFI IS NOT CONNECTED!");
    }
  }
    
  delay(1000);
}





