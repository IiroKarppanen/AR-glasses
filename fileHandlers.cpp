#include <FS.h>
#include "SPIFFS.h"


String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);
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
  Serial.println(fileContent);
  return fileContent;
}

int hasContent(fs::FS &fs, const char * path){
  int returnType = 0;
  if (SPIFFS.exists(path)) {
    // Get the file size
    File file = SPIFFS.open(path, FILE_READ);
    if (file) {
      size_t fileSize = file.size();
      file.close();

      // Check if the file has content
      if (fileSize > 0) {
        returnType = 1;
      } 
    } 
  } 

  return returnType;
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