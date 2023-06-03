#include <FS.h>
#include "SPIFFS.h"

String readFile(fs::FS &fs, const char * path){
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }

  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  file.close();
  return fileContent;
}


void writeFile(fs::FS &fs, const char * path, const char * message){
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
