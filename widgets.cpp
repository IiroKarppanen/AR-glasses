#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);

void loadingWidget(){
    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_crox1c_tf);
    u8g2.drawStr(60, 20, "Loading..");

    u8g2.sendBuffer();
}

void showIP(const char* address){
    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_crox1c_tf);
    u8g2.drawStr(60, 20, address);

    u8g2.sendBuffer();
}


void weatherWidget(const char* param1, const char* param2, const char* param3){

  u8g2.clearBuffer();

  if(String(param3).equals("Clear")){
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
    u8g2.drawGlyph(60, 24, 69);	
    u8g2.setFont(u8g2_font_crox1hb_tf);
    u8g2.drawStr(50, 25, param1);
    u8g2.drawStr(37, 25, param2);
  }
  else if(String(param3) == "Rain"){
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
    u8g2.drawGlyph(65, 24, 67);	
  }
  // SUN_CLOUD = 65
  // CLOUD == 64
  // THUNDER == 67


  u8g2.sendBuffer();
}


void clockWidget(const char* param1, const char* param2, const char* param3){
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_crox2hb_tf);
  u8g2.drawStr(45, 20, param3);
  u8g2.drawStr(60, 20, param2);
  u8g2.drawStr(75, 20, param1);

  u8g2.sendBuffer();
}


void stockWidget(const char* param1, const char* param2, const char* param3){

  u8g2.clearBuffer();
  

  //u8g2.setFont(u8g2_font_crox2hb_tf);
  //u8g2.drawStr(45, 20, param3);

  u8g2.sendBuffer();
}

void textWidget(const char* param1, const char* param2, const char* param3){

  u8g2.clearBuffer();

  //u8g2.setFont(u8g2_font_crox2hb_tf);
  //u8g2.drawStr(45, 20, param3);

  u8g2.sendBuffer();
}

