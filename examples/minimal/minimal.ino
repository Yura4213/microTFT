#define TFT_CS  10
#define TFT_RST 8
#define TFT_DC  9

//#define TFT_NO_ROTATE // убрать поворот
#include <microTFT.h>
microTFT tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  tft.init();// инициализэйшн
  //tft.setWindow(5, 5, 20, 20);    // поставить окно
  tft.home();   // домой
  tft.fillScrn(0, 0);   // чисти
}

void loop() {}
