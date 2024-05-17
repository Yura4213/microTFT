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
  delay(5000);
  tft.fillScrn(255, 0b11100000); // залить жёлтым
  delay(5000);
  tft.fillScrn(0b1111100000000000); // и красным
  delay(5000);
  tft.start();
  tft.dot(60, 56, 1);
  delay(5000);
  //tft.start();
  tft.fill888(0x0000FF);    // цвет синий
  tft.rect(0, 0, 127, 159, GFX_STROKE); // рамка во весь экран
  tft.fill888(0x000000);    // цвет чёрный
  tft.roundRect(10, 10, 117, 149, GFX_STROKE);
  tft.fill888(0x00FF00);    // цвет зелёный
  tft.circle(63, 79, 20, GFX_STROKE);
  delay(5000);
  tft.home();   // домой
  //tft.start();
  //tft.fill888(0x00FF00);
  // Проверка скорости попиксельной заливки (1042мс)
  uint32_t tmr = millis();
  for (int y = 0; y < 160; y++) {
    for (int x = 0; x < 128; x++) {
      tft.dot(x, y, 1);
    }
  }
  tft.print(millis() - tmr);
  delay(5000);
  tft.home();
  // ...и хитрой заливки (85мс)
  tmr = millis();
  tft.startSend(0, 0, 127, 159);
  for (int y = 0; y < 160; y++) {
    for (int x = 0; x < 128; x++) {
      tft.send565(0b0000011111111111);
    }
  }
  tft.print(millis(
  tft.print(millis() - tmr);
  delay(5000);
  tft.fillScrn(0);
}

void loop() {   // жуть
    #ifndef TFT_NO_ROTATE
    static uint8_t r;
    tft.rotate(++r>=4?r=0:r);  // поворот
    #endif
    tft.fillScrn(0);
    tft.setCursor(0, 0);
    tft.flipV(0);
    tft.flipH(0);
    tft.start();
    tft.print("FVH = 00");
    delay(2000);
    tft.fillScrn(0);
    tft.setCursor(0, 0);
    tft.flipV(1);
    tft.flipH(0);
    tft.start();
    tft.print("FVH = 10");
    delay(2000);
    tft.fillScrn(0);
    tft.setCursor(0, 0);
    tft.flipV(0);
    tft.flipH(1);
    tft.start();
    tft.print("FVH = 01");
    delay(2000);
    tft.fillScrn(0);
    tft.setCursor(0, 0);
    tft.flipV(1);
    tft.flipH(1);
    tft.start();
    tft.print("FVH = 11");
    delay(2000);
}
