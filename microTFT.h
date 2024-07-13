#ifndef MICROTFT_H
#define MICROTFT_H

#include <GyverGFX.h>
#include <GyverIO.h>
#include <SPI.h>
#include <Arduino.h>

//#define ST7735_CASET   0x2A
//#define ST7735_RASET   0x2B // PASET
//#define ST7735_RAMWR   0x2C

#define rgb888to565(clr) ((((((clr) >> 16) & 0xFF) & 0b11111000) << 8) | (((((clr) >> 8) & 0xFF) & 0b11111100) << 3) | ((((clr) & 0xFF) & 0b11111000) >> 3))
//#define rgb323to888(clr) ((uint32_t)(((clr) & 0b11100000) | 0b00011111) << 16 | (uint32_t)((((clr) & 0b00011000) << 3) | 0b00111111) << 8 | ((((clr) & 0b00000111) << 5) | 0b00011111))

#ifndef TFT_NO_GFX
class microTFT : public GyverGFX {
#else
class microTFT {
#endif
  public:
    microTFT(uint8_t pin_cs, uint8_t pin_dc, uint8_t pin_rst) : GyverGFX(128, 160), _cs(pin_cs), _dc(pin_dc), _rst(pin_rst) {
      LCD_CS(0);
      SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));  // запуск spi
    }
    void fill888(uint32_t clr) {  // выбор цвета, 32 бит
      // ****
      fill(rgb888to565(clr));  // переделать // отправить дальше
    }
    void fill(uint16_t clr) {  // выбор цвета, 16 бит
      // ****
      clrH = (clr & 0xFF00) >> 8; // мистика
      clrL = clr & 0xFF;
    }
    void fill323(uint8_t clr) {  // выбор цвета, 8 бит
      // ****
      clrH = /*0b00011001 | */(clr & 0b11100000) | ((clr & 0b00011000) >> 2);  // мистика
      clrL = /*0b11100011 | */(clr & 0b00000111) << 2;
    }
    void fill233(uint8_t clr) {  // выбор цвета, 8 бит
      // ****
      clrH = /*0b00111000 | */(clr & 0b11000000) | ((clr & 0b00111000) >> 3);  // мистика
      clrL = /*0b11100011 | */((clr & 0b00000111) << 2);
    }
    void bckgrnd888(uint32_t clr) { // аналогично
      // ****
      bckgrnd(rgb888to565(clr));
    }
    void bckgrnd(uint16_t clr) {
      // ****
      bclrH = (clr & 0xFF00) >> 8;
      bclrL = clr & 0xFF;
    }
    void bckgrnd323(uint8_t clr) {  // выбор цвета, 8 бит
      // ****
      bclrH = /*0b00011001 | */(clr & 0b11100000) | ((clr & 0b00011000) >> 2);  // мистика
      bclrL = /*0b11100011 | */(clr & 0b00000111) << 2;
    }
    void bckgrnd233(uint8_t clr) {  // выбор цвета, 8 бит
      // ****
      bclrH = /*0b00111000 | */(clr & 0b11000000) | ((clr & 0b00111000) >> 3);  // мистика
      bclrL = /*0b11100011 | */((clr & 0b00000111) << 2);
    }
    #ifndef TFT_NO_ROTATE // если вкл. поворот
    void flipV(bool x) {  // отразить по-вертикали
      uint8_t invxy = _rotate & 1;  // инвертировать ли оси
      uint8_t xmask = _rotate >= 2; // маска по иксу
      uint8_t ymask = _rotate && _rotate != 3;  // маска по y'ку
      bitWrite(madctl, invxy?6:7, (!x)^ymask);  // записать бит
      start();  // вкл чипселект
      lcd7735_sendCmd(0x36);  // мистика
      lcd7735_sendData(madctl);
      end();  // выкл чипселект
    }
    void flipH(bool x) {  // отразить по-горизонтали
      uint8_t invxy = _rotate & 1;  // аналогично
      uint8_t xmask = _rotate >= 2;
      uint8_t ymask = _rotate && _rotate != 3;
      bitWrite(madctl, invxy?7:6, (!x)^xmask);
      start();
      lcd7735_sendCmd(0x36);
      lcd7735_sendData(madctl);
      end();
    }
    #else // если надо просто и быстро
    void flipV(bool x) {
      bitWrite(madctl, 7, !x);  // записать
      start();
      lcd7735_sendCmd(0x36);  // мистика
      lcd7735_sendData(madctl);
      end();
    }
    void flipH(bool x) {
      bitWrite(madctl, 6, !x);  // аналогично
      start();
      lcd7735_sendCmd(0x36);
      lcd7735_sendData(madctl);
      end();
    }
    #endif
    void init() {
      // инициализэйшн
      SPI.begin();
      pinMode(TFT_CS, 1); // настроить ноги
      pinMode(TFT_DC, 1);
      pinMode(TFT_RST, 1);
      SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));  // вкл. spi
      // перезагрузка
      LCD_RST(0);           // RST=0
      delay(10);        //delay for 10 millisecond
      LCD_RST(1);           // RST=1
      delay(10);          //delay for 10 millisecond
      LCD_CS(0);        //enable LCD
      lcd7735_sendCmd(0x11);
      delay(10);
      lcd7735_sendCmd (0x3A); //Set Color mode
      lcd7735_sendData(0x05); //16 bits
      lcd7735_sendCmd(0x36);
      lcd7735_sendData(madctl);
      lcd7735_sendCmd(0x29);//Display on
      //SPI.endTransaction();
    }
    #ifndef TFT_NO_GFX
    void dot(int x, int y, uint8_t fill) {  // точка
      //Serial.print('Ы');  // отладка. Жутко замедляет вывод
      setWindow(x, y, x + 1, y + 1);
      LCD_DC(1);  // вкл. режим отрисовки
      //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
      SPI.transfer(fill ? clrH : bclrH);  // мистика (отправить точку)
      SPI.transfer(fill ? clrL : bclrL);
      //SPI.endTransaction();
      //LCD_CS(1);
    }
    void update() { // надо. Без него ломается. Мистика
    }
    #endif
    #ifndef TFT_NO_CLRBMP
    void bitmapRGB888(uint32_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0) {
      start();
      startSend(max(x0, (int16_t)0), max(y0, (int16_t)0), max(x0+w, /*(int16_t)*/0), max(y0+h, /*(int16_t)*/0));
      for (size_t i = 0; i < w*h; i++) {
        #ifndef TFT_NO_CHECK_DOT
        uint16_t x = (i % w)+x0, y = i / w+y0;
        #ifndef TFT_NO_ROTATE
        if (x >= 0 && y >= 0 && x < _rotate & 1 ? 160 : 128 && x < _rotate & 1 ? 128 : 160) {
        #else
        if (x >= 0 && y >= 0 && x < 128 && x < 160) {
        #endif
        #else {
        #endif
          send565(rgb888to565(arr[i]));
        }
      }
    }
    void bitmapRGB565(uint16_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0) {
      start();
      startSend(max(x0, (int16_t)0), max(y0, (int16_t)0), max(x0+w, /*(int16_t)*/0), max(y0+h, /*(int16_t)*/0));
      for (size_t i = 0; i < w*h; i++) {
        #ifndef TFT_NO_CHECK_DOT
        uint16_t x = (i % w)+x0, y = i / w+y0;
        #ifndef TFT_NO_ROTATE
        if (x >= 0 && y >= 0 && x < _rotate & 1 ? 160 : 128 && x < _rotate & 1 ? 128 : 160) {
        #else
        if (x >= 0 && y >= 0 && x < 128 && x < 160) {
        #endif
        #else {
        #endif
          send565(arr[i]);
        }
      }
    }
    void bitmapRGB323(uint8_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0) {
      start();
      startSend(max(x0, (int16_t)0), max(y0, (int16_t)0), max(x0+w, /*(int16_t)*/0), max(y0+h, /*(int16_t)*/0));
      for (size_t i = 0; i < w*h; i++) {
        #ifndef TFT_NO_CHECK_DOT
        uint16_t x = (i % w)+x0, y = i / w+y0;
        #ifndef TFT_NO_ROTATE
        if (x >= 0 && y >= 0 && x < _rotate & 1 ? 160 : 128 && x < _rotate & 1 ? 128 : 160) {
        #else
        if (x >= 0 && y >= 0 && x < 128 && x < 160) {
        #endif
        #else {
        #endif
          sendByte((arr[i] & 0b11100000) | ((arr[i] & 0b00011000) >> 2));
          sendByte(((arr[i] & 0b00000111) << 2));
        }
      }
    }
    void bitmapRGB233(uint8_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0) {
      start();
      startSend(max(x0, (int16_t)0), max(y0, (int16_t)0), max(x0+w, /*(int16_t)*/0), max(y0+h, /*(int16_t)*/0));
      for (size_t i = 0; i < w*h; i++) {
        #ifndef TFT_NO_CHECK_DOT
        uint16_t x = (i % w)+x0, y = i / w+y0;
        #ifndef TFT_NO_ROTATE
        if (x >= 0 && y >= 0 && x < _rotate & 1 ? 160 : 128 && x < _rotate & 1 ? 128 : 160) {
        #else
        if (x >= 0 && y >= 0 && x < 128 && x < 160) {
        #endif
        #else {
        #endif
          sendByte((arr[i] & 0b11000000) | ((arr[i] & 0b00111000) >> 3));
          sendByte(((arr[i] & 0b00000111) << 2));
        }
      }
    }
    #ifndef TFT_PGMBMP
    void bitmapRGB888_P(const uint32_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0) {
      start();
      startSend(max(x0, (int16_t)0), max(y0, (int16_t)0), max(x0+w, /*(int16_t)*/0), max(y0+h, /*(int16_t)*/0));
      for (size_t i = 0; i < w*h; i++) {
        #ifndef TFT_NO_CHECK_DOT
        uint16_t x = (i % w)+x0, y = i / w+y0;
        #ifndef TFT_NO_ROTATE
        if (x >= 0 && y >= 0 && x < _rotate & 1 ? 160 : 128 && x < _rotate & 1 ? 128 : 160) {
        #else
        if (x >= 0 && y >= 0 && x < 128 && x < 160) {
        #endif
        #else {
        #endif
          send565(rgb888to565(pgm_read_dword(&arr[i])));
        }
      }
    }
    void bitmapRGB565_P(const uint16_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0) {
      start();
      startSend(max(x0, (int16_t)0), max(y0, (int16_t)0), max(x0+w, /*(int16_t)*/0), max(y0+h, /*(int16_t)*/0));
      for (size_t i = 0; i < w*h; i++) {
        #ifndef TFT_NO_CHECK_DOT
        uint16_t x = (i % w)+x0, y = i / w+y0;
        #ifndef TFT_NO_ROTATE
        if (x >= 0 && y >= 0 && x < _rotate & 1 ? 160 : 128 && x < _rotate & 1 ? 128 : 160) {
        #else
        if (x >= 0 && y >= 0 && x < 128 && x < 160) {
        #endif
        #else {
        #endif
          send565(pgm_read_word(&arr[i]));
        }
      }
    }
    void bitmapRGB323_P(const uint8_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0) {
      start();
      startSend(max(x0, (int16_t)0), max(y0, (int16_t)0), max(x0+w, /*(int16_t)*/0), max(y0+h, /*(int16_t)*/0));
      for (size_t i = 0; i < w*h; i++) {
        #ifndef TFT_NO_CHECK_DOT
        uint16_t x = (i % w)+x0, y = i / w+y0;
        #ifndef TFT_NO_ROTATE
        if (x >= 0 && y >= 0 && x < _rotate & 1 ? 160 : 128 && x < _rotate & 1 ? 128 : 160) {
        #else
        if (x >= 0 && y >= 0 && x < 128 && x < 160) {
        #endif
        #else {
        #endif
          sendByte((pgm_read_byte(&arr[i]) & 0b11100000) | ((pgm_read_byte(&arr[i]) & 0b00011000) >> 2));
          sendByte(((pgm_read_byte(&arr[i]) & 0b00000111) << 2));
        }
      }
    }
    void bitmapRGB233_P(const uint8_t* arr, uint16_t w, uint16_t h, int16_t x0 = 0, int16_t y0 = 0) {
      start();
      startSend(max(x0, (int16_t)0), max(y0, (int16_t)0), max(x0+w, /*(int16_t)*/0), max(y0+h, /*(int16_t)*/0));
      for (size_t i = 0; i < w*h; i++) {
        #ifndef TFT_NO_CHECK_DOT
        uint16_t x = (i % w)+x0, y = i / w+y0;
        #ifndef TFT_NO_ROTATE
        if (x >= 0 && y >= 0 && x < _rotate & 1 ? 160 : 128 && x < _rotate & 1 ? 128 : 160) {
        #else
        if (x >= 0 && y >= 0 && x < 128 && x < 160) {
        #endif
        #else {
        #endif
          sendByte((pgm_read_byte(&arr[i]) & 0b11000000) | ((pgm_read_byte(&arr[i]) & 0b00111000) >> 3));
          sendByte(((pgm_read_byte(&arr[i]) & 0b00000111) << 2));
        }
      }
    }
    #endif
    #endif
    void start() {  // вкл. чипселект
      LCD_CS(0);
    }
    void end() {  // выкл чипселект
      LCD_CS(1);
    }
    void startSend(uint8_t x0 = 0, uint8_t y0 = 0, uint8_t x1 = 128, uint8_t y1 = 160) {  // начало шутрой отправки. Аргументы - координаты области
      setWindow(x0, y0, x1, y1); // сделать окно
      LCD_CS(0);  // вкл. чипселект
      //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
      LCD_DC(1);  // вкл. режим отправки
    }
    void sendByte(uint8_t col) {  // отправить байт
      SPI.transfer(col);
    }
    void send565(uint16_t col) {  // отправить цвет 565
      SPI.transfer((col & 0xFF00) >> 8);  // мистика
      SPI.transfer(col & 0xFF);
    }
    void fillScrn(uint16_t color) { // залить дисп
      fillScrn((color & 0xFF00) >> 8, color & 0x00FF);
    }
    void fillScrn(byte colorH, byte colorL) { // залить дисп
      home();
      LCD_CS(0);
      LCD_DC(1);
      //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
      for (int x = 0; x < 160 * 128; x++) {
        SPI.transfer(colorH);
        SPI.transfer(colorL);
      }
      //SPI.endTransaction();
      LCD_CS(1);
    }
    #ifndef TFT_NO_ROTATE
    void rotate(uint8_t x) {  // поворот
      // а дальше сплошная мистика
      uint8_t invxy = x & 1;
      uint8_t xmask = x >= 2;
      uint8_t ymask = x && x != 3;
      _rotate = x;
      start();
      bitWrite(madctl, 5, invxy);
      bitWrite(madctl, 7, !xmask);
      bitWrite(madctl, 6, !ymask);
      lcd7735_sendCmd(0x36);
      lcd7735_sendData(madctl);
      home();
      end();
      size(width, height);  // обновить размер
    }
    #endif

    void home() { // домой
      setCursor(0, 0);  // домой курсор
      LCD_CS(0);
      LCD_DC(0);
      // мистика:
      //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
      lcd7735_sendCmd(0x2A); // Column addr set // Касет
      lcd7735_sendData(0);
      lcd7735_sendData(0);   // XSTART
      lcd7735_sendData(0);
      lcd7735_sendData(width);   // XEND
      lcd7735_sendCmd(0x2B); // Row addr set  // Расет
      lcd7735_sendData(0);
      lcd7735_sendData(0);   // YSTART
      lcd7735_sendData(0);
      lcd7735_sendData(height);   // YEND
      lcd7735_sendCmd(0x2C); // write to RAM  // Рамвр
      //SPI.endTransaction();
      //LCD_CS(1);
    }/*
    void startSend() {  // что? // другой стартсенд
      LCD_CS(0);
      //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
      LCD_DC(1);
    }*/
    void setWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {  // поставить окно
      //LCD_CS(0);
      // оптимизированная мистика:
      LCD_DC(0);
      //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
      LCD_DC(0);
      SPI.transfer(0x2A); // Column addr set  // Касет
      LCD_DC(1);
      SPI.transfer(0);
      SPI.transfer(x0);   // XSTART
      SPI.transfer(0);
      SPI.transfer(x1);   // XEND

      LCD_DC(0);
      SPI.transfer(0x2B); // Row addr set // Расет
      LCD_DC(1);
      SPI.transfer(0);
      SPI.transfer(y0);   // YSTART
      SPI.transfer(0);
      SPI.transfer(y1);   // YEND

      LCD_DC(0);
      SPI.transfer(0x2C); // write to RAM // Рамвр
      //SPI.endTransaction();
      //LCD_CS(1);
    }
    uint8_t width = 128, height = 160;  // ширина, высота
  private:
    const uint8_t _cs, _dc, _rst; // ноги
    //uint8_t _css = 2, _dcs = 2, _rsts = 2;
    uint8_t clrH = 0xFF, clrL = 0xFF; // цвет
    uint8_t bclrH = 0x00, bclrL = 0x00; // фон
    uint8_t madctl = 0b11000000;  // регистр 0х36
    #ifndef TFT_NO_ROTATE // если надо
    uint8_t _rotate;  // поворот
    #endif
    // отпралялки всякого
    void lcd7735_sendCmd(unsigned char cmd) {
      LCD_DC(0); //Set DC low
      SPI.transfer(cmd);
    }
    void lcd7735_sendData(unsigned char data) {
      LCD_DC(1);//Set DC HIGH
      SPI.transfer(data);
    }
    // дёрганье ногами:
    void LCD_CS(bool x) {
      //if (x == _css) return;
      digitalWrite(_cs, x);
      //_css = x;
    }
    void LCD_DC(bool x) {
      //if (x == _dcs) return;
      gio::write(_dc, x);
      //_dcs = x;
    }
    void LCD_RST(bool x) {
      //if (x == _rsts) return;
      gio::write(_rst, x);
      //_rsts = x;
    }
};
#endif
