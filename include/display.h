#include <iostream>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <cstring>
#include "font.h"

// Alignment enum
enum TextAlign {
    ALIGN_LEFT = 0,
    ALIGN_CENTER = 1,
    ALIGN_RIGHT = 2
};


// ST7565R Display Class - Based on NHD-C12832A1Z-NSW-BBW-3V3 Datasheet
class NHD_C12832 {
private:
    // Pin definitions (adjust to your IO board)
    
    // Display buffer: 128 columns x 32 rows = 4 pages of 128 bytes
    unsigned char buffer[512];   // 128 * 32 / 8 = 512 bytes
    bool useSoftwareSPI = true;  // Set to false to use hardware SPI

public:
    NHD_C12832();

    void reset();
    void initialize();
    void sendCommand(unsigned char cmd);
    void sendData(unsigned char data);
    void sendByte(unsigned char byte);
    void sendByteSoftwareSPI(unsigned char byte);
    void sendByteHardwareSPI(unsigned char byte);
    void clearBuffer();
    void fillBuffer();
    void display();
    void setPixel(int x, int y, bool on);
    void drawHLine(int x0, int x1, int y);
    void drawVLine(int x, int y0, int y1);
    void drawLine(int x0, int y0, int x1, int y1);
    void drawRect(int x, int y, int w, int h);
    void fillRect(int x, int y, int w, int h);
    void setContrast(unsigned char value);
    void displayOnOff(bool on);
    void drawChar(int x, int y, char c);
    void printString(int x, int line, const char* str, TextAlign align = ALIGN_LEFT);
    unsigned char* getBuffer();
    int getBufferSize();
};
