#define TFT_CS  10
#define TFT_RST 8
#define TFT_DC  9

#include "microTFT.h"
microTFT tft(TFT_CS, TFT_DC, TFT_RST);

//#include "sem.h"
#include "xp.h"
//#include "cat.h"

void setup() {
  tft.init();// инициализэйшн
  tft.home();   // домой
  tft.fillScrn(0, 0);   // чисти
  delay(5000);
  
  tft.rotate(1);
  uint32_t tmr = millis();
  /*tft.bitmapRGB233_P(semerka_160x128, 160, 128);
  tft.print(millis() - tmr);
  delay(5000);*/
  tmr = millis();
  tft.bitmapRGB233_P(windows_xp_160x128, 160, 128);
  tft.print(millis() - tmr);
  delay(5000);/*
  tmr = millis();
  tft.bitmapRGB233_P(cat_160x128, 160, 128);
  tft.print(millis() - tmr);*/
}

void loop() {}
