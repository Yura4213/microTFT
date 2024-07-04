# microTFT
Быстрая библиотека для TFT дисплея на ST7735

- Подключение по аппаратному SPI
- Возможность быстро закрасить определённую область экрана
- Отражение по-вертикали/горизонтали
- Поворот
- Все возможности GyverGFX (графика)
- Разные цвета
- Цветные битмапы

## Зависимости
GyverGFX и GyverIO
#### Совместимость
AVR ATmega, возможно esp (не проверял)
## Использование
```cpp
// Настройки
#define TFT_NO_ROTATE  // убрать поворот
#define TFT_NO_GFX  // убрать графику
#define TFT_NO_CLRBMP  // убрать битмапы
#define TFT_NO_PGMBMP  // убрать progmem битмапы
#define TFT_NO_CHECK_DOT  // не проверять выход за границы дисплея
// Конструктор
void microTFT(uint8_t pin_cs, uint8_t pin_dc, uint8_t pin_rst);
// Инициализация
void init(); // включить
void home(); // домой курсор, окно на весь дисп
// Цвет
void fill(uint16_t clr); // цвет rgb565
void fill888(uint32_t clr); // цвет rgb888
void fill323(uint8_t clr); // цвет rgb323
void fill233(uint8_t clr); // цвет rgb233
void bckgrnd(uint32_t clr); // цвет фона rgb565
void bckgrnd888(uint32_t clr); // цвет фона rgb888
void bckgrnd323(uint32_t clr); // цвет фона rgb323
void bckgrnd233(uint32_t clr); // цвет фона rgb233
// Рисование
void dot(int x, int y, uint8_t fill); // точка
void fillScrn(uint16_t color); // залить дисп цветом 565
void fillScrn(byte colorH, byte colorL); // залить дисп цветом из двух байтов
// + всё из GyverGFX
// Битмапы
void bitmapRGB888(uint32_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0);
void bitmapRGB565(uint16_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0);
void bitmapRGB323(uint8_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0);
void bitmapRGB233(uint8_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0);
void bitmapRGB888_P(const uint32_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0);
void bitmapRGB565_P(const uint16_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0);
void bitmapRGB323_P(const uint8_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0);
void bitmapRGB233_P(const uint8_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0);
// массив, ширина, высота, координаты. Можно выходить за границы дисплея
// Поворот
void flipV(bool x); // отразить по-вертикали
void flipH(bool x); // отразить по-горизонтали
void rotate(uint8_t x); // поворот
// Управление дисплеем
void setWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);  // поставить окно
void start();  // вкл. чипселект
void end();  // выкл чипселект
void startSend(uint8_t x0 = 0, uint8_t y0 = 0, uint8_t x1 = 128, uint8_t y1 = 160);  // начало шустрой отправки. Аргументы - координаты области
void sendByte(uint8_t col); // отправить байт
void send565(uint16_t col); // отправить цвет 565
```
## Пример
```cpp
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
```
## Версии
- v1.0 Оно работает
- v1.1 +цветные битмапы и rgb233


## Планируется потом
- Полная совместимость с GyverOLED
- Программное включение/выключение/перезагрузка
- Ещё сильнее оптимизировать

## Баги и обратная связь
Библиотека ещё сырая, если что-то не работает, или хочется что-то добавить, пишите [сюда](https://t.me/arduinki_esp_chat)
