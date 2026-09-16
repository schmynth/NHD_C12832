#include <iostream>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <cstring>
#include "display.h"

const int RST_PIN = 15;      // GPIO17 - /RST (Active LOW Reset)
const int A0_PIN = 14;       // GPIO27 - A0 (Register Select)
const int CS_PIN = 8;        // GPIO8  - /CS1 (Active LOW Chip Select)
const int SI_PIN = 10;       // GPIO10 - SI (Serial Data/MOSI)
const int SCL_PIN = 11;      // GPIO11 - SCL (Serial Clock)

const int SPI_CHANNEL = 0;   // SPI0
const int SPI_SPEED = 1000000; // 1 MHz (safe for bit-bang, can go faster with hardware SPI)
                               
// ST7565R Display Class - Based on NHD-C12832A1Z-NSW-BBW-3V3 Datasheet
NHD_C12832::NHD_C12832() {
    wiringPiSetupGpio();
    
    if (useSoftwareSPI) {
        // Software SPI: set pins as outputs
        pinMode(RST_PIN, OUTPUT);
        pinMode(A0_PIN, OUTPUT);
        pinMode(CS_PIN, OUTPUT);
        pinMode(SI_PIN, OUTPUT);
        pinMode(SCL_PIN, OUTPUT);
        
        digitalWrite(CS_PIN, HIGH);   // CS inactive (HIGH)
        digitalWrite(SCL_PIN, HIGH);  // Clock idle high
    } else {
        // Hardware SPI
        wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED);
        pinMode(RST_PIN, OUTPUT);
        pinMode(A0_PIN, OUTPUT);
        // CS handled by hardware
    }
    
    clearBuffer();
    initialize();
}

/**
 * Reset the display
 * Min pulse width: 10µs, Recovery time: 10µs
 */
void NHD_C12832::reset() {
    std::cout << "[LCD] Resetting display..." << std::endl;
    
    digitalWrite(RST_PIN, HIGH);
    usleep(10000);  // 10ms setup
    digitalWrite(RST_PIN, LOW);
    usleep(10000);  // 10ms low pulse
    digitalWrite(RST_PIN, HIGH);
    usleep(10000);  // 10ms recovery
    
    std::cout << "[LCD] Reset complete" << std::endl;
}

/**
 * Initialize display with datasheet-recommended sequence
 * Reference: Example Initialization Program (page 9 of datasheet)
 */
void NHD_C12832::initialize() {
    reset();
    
    // Initialization sequence from NewHaven datasheet
    sendCommand(0xE2);  // (14) Reset - Internal reset
    usleep(1000);
    
    sendCommand(0xA0);  // (8) ADC Select - Normal direction (0: normal, 1: reverse)
    sendCommand(0xAE);  // (1) Display ON/OFF - OFF (0xAE=OFF, 0xAF=ON)
    sendCommand(0xC8);  // (15) Common output mode select - Normal (0xC0=normal, 0xC8=reverse)
                        // Mirrors upside down
    sendCommand(0xA2);  // (11) LCD Bias Set - 1/9 bias (0xA2=1/9, 0xA3=1/7)
    sendCommand(0x2F);  // (16) Power Control Set - All circuits ON
                        //      0x2C: Booster only
                        //      0x2E: Booster + Regulator
                        //      0x2F: Booster + Regulator + Follower
    usleep(50000);      // Allow power supply to stabilize
    
    sendCommand(0x21);  // (17) V5 Voltage Regulator Internal Resistor Ratio Set
                        //      0x20-0x27 valid range
    
    sendCommand(0x81);  // (18) Electronic Volume Mode Set
    sendCommand(0x3F);  // Electronic Volume Register (contrast: 0x00-0x3F, default 0x1F-0x3F)
    
    sendCommand(0x40);  // (2) Display Start Line Set - 0x40 = line 0
    
    sendCommand(0xAF);  // (1) Display ON/OFF - ON
    
    std::cout << "[LCD] Initialization complete" << std::endl;
}

/**
 * Send command byte (A0 = LOW)
 * Timing: tAS=20ns, tAH=10ns, tDS=20ns, tDH=10ns, tCSS=20ns, tCSH=40ns
 */
void NHD_C12832::sendCommand(unsigned char cmd) {
    digitalWrite(A0_PIN, LOW);  // A0 LOW = Command mode
    sendByte(cmd);
}

/**
 * Send data byte (A0 = HIGH)
 */
void NHD_C12832::sendData(unsigned char data) {
    digitalWrite(A0_PIN, HIGH);  // A0 HIGH = Data mode
    sendByte(data);
}

/**
 * Send byte over SPI (MSB first)
 */
void NHD_C12832::sendByte(unsigned char byte) {
    if (useSoftwareSPI) {
        sendByteSoftwareSPI(byte);
    } else {
        sendByteHardwareSPI(byte);
    }
}

/**
 * Software SPI: Bit-bang transmission
 */
void NHD_C12832::sendByteSoftwareSPI(unsigned char byte) {
    digitalWrite(CS_PIN, LOW);  // CS active (LOW)
    usleep(1);                  // tCSS setup
    
    for (int i = 7; i >= 0; i--) {
        unsigned char bit = (byte >> i) & 1;
        
        // Clock low
        digitalWrite(SCL_PIN, LOW);
        usleep(1);
        
        // Set data bit
        digitalWrite(SI_PIN, bit ? HIGH : LOW);
        usleep(1);  // tDS data setup
        
        // Clock high
        digitalWrite(SCL_PIN, HIGH);
        usleep(2);  // tSHW clock high width
    }
    
    digitalWrite(SCL_PIN, LOW);
    usleep(1);
    digitalWrite(CS_PIN, HIGH);  // CS inactive (HIGH)
    usleep(1);                   // tCSH hold
}

/**
 * Hardware SPI transmission
 */
void NHD_C12832::sendByteHardwareSPI(unsigned char byte) {
    digitalWrite(CS_PIN, LOW);
    wiringPiSPIDataRW(SPI_CHANNEL, &byte, 1);
    digitalWrite(CS_PIN, HIGH);
}

/**
 * Clear frame buffer
 */
void NHD_C12832::clearBuffer() {
    memset(buffer, 0, sizeof(buffer));
}

/**
 * Fill frame buffer with pattern
 */
void NHD_C12832::fillBuffer() {
    memset(buffer, 0xFF, sizeof(buffer));
}

/**
 * Send buffer to display
 * Display uses page addressing: 4 pages (0-3) x 128 columns
 * Each page = 8 pixels vertical
 */
void NHD_C12832::display() {
    for (int page = 0; page < 4; page++) {
        // Set page address (0xB0-0xB3)
        sendCommand(0xB0 | page);
        
        // Set column address: Lower 4 bits (0x00-0x0F)
        sendCommand(0x00);
        
        // Set column address: Upper 4 bits (0x10-0x1F)
        sendCommand(0x10);
        
        // Send 128 bytes of data for this page
        for (int col = 0; col < 128; col++) {
            sendData(buffer[page * 128 + col]);
        }
    }
}

/**
 * Set a pixel in the buffer
 * x: 0-127 (column)
 * y: 0-31  (row)
 * on: true = set, false = clear
 */
void NHD_C12832::setPixel(int x, int y, bool on) {
    if (x < 0 || x >= 128 || y < 0 || y >= 32) return;
    
    int byte_index = (y / 8) * 128 + x;
    int bit = y % 8;
    
    if (on) {
        buffer[byte_index] |= (1 << bit);
    } else {
        buffer[byte_index] &= ~(1 << bit);
    }
}

/**
 * Draw a horizontal line
 */
void NHD_C12832::drawHLine(int x0, int x1, int y) {
    if (y < 0 || y >= 32) return;
    if (x0 > x1) std::swap(x0, x1);
    
    for (int x = x0; x <= x1; x++) {
        setPixel(x, y, true);
    }
}

/**
 * Draw a vertical line
 */
void NHD_C12832::drawVLine(int x, int y0, int y1) {
    if (x < 0 || x >= 128) return;
    if (y0 > y1) std::swap(y0, y1);
    
    for (int y = y0; y <= y1; y++) {
        setPixel(x, y, true);
    }
}

/**
 * Draw a diagonal line using Bresenham's algorithm
 */
void NHD_C12832::drawLine(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        setPixel(x0, y0, true);
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/**
 * Draw a rectangle
 */
void NHD_C12832::drawRect(int x, int y, int w, int h) {
    drawHLine(x, x + w - 1, y);
    drawHLine(x, x + w - 1, y + h - 1);
    drawVLine(x, y, y + h - 1);
    drawVLine(x + w - 1, y, y + h - 1);
}

/**
 * Draw a filled rectangle
 */
void NHD_C12832::fillRect(int x, int y, int w, int h) {
    for (int yy = y; yy < y + h; yy++) {
        drawHLine(x, x + w - 1, yy);
    }
}

/**
 * Set contrast (0x00-0x3F, default 0x1F)
 */
void NHD_C12832::setContrast(unsigned char value) {
    if (value > 0x3F) value = 0x3F;
    
    sendCommand(0x81);  // Electronic Volume Mode Set
    sendCommand(value); // Contrast value
    
    std::cout << "[LCD] Contrast set to: 0x" << std::hex << (int)value << std::dec << std::endl;
}

/**
 * Set display on/off
 */
void NHD_C12832::displayOnOff(bool on) {
    sendCommand(on ? 0xAF : 0xAE);
    std::cout << "[LCD] Display " << (on ? "ON" : "OFF") << std::endl;
}

/**
 * Draw single character from Font5x7 bitmap font
 * Font is 5 pixels wide, 7 pixels tall
 * Each character = 5 bytes (5 columns, 7 rows)
 * Characters stored sequentially: ASCII 0x20-0x7F (space-tilde)
 */
void NHD_C12832::drawChar(int x, int y, char c) {
    // Validate character is in font range (0x20-0x7F = space to ~)
    if (c < 0x20 || c > 0x7F) return;
    if (x < 0 || x >= 128 || y < 0 || y >= 32) return;
    
    // Calculate offset into font array
    // Each character is 5 bytes
    unsigned int charOffset = (c - 0x20) * 5;
    
    // Draw each column (5 wide)
    for (int col = 0; col < 5; col++) {
        unsigned char fontByte = Font5x7[charOffset + col];
        
        // Draw each bit in the column (7 tall, LSB = top)
        for (int row = 0; row < 7; row++) {
            if (fontByte & (1 << row)) {
                setPixel(x + col, y + row, true);
            }
        }
    }
}

/**
 * Print a string starting at (x, y)
 * Best results when y is a page boundary: 0, 8, 16, or 24
 * Returns final x position for chaining multiple strings
 */
void NHD_C12832::printString(int x, int line, const char* str, TextAlign align) {
    if (!str) return;
    int y = line * 8;
    
    // Calculate string width (6 pixels per character: 5 + 1 gap)
    int stringWidth = strlen(str) * 6;
    
    // Calculate starting x position based on alignment
    int startX = x;
    switch (align) {
        case ALIGN_LEFT:
            startX = x;
            break;
        case ALIGN_CENTER:
            startX = x - (stringWidth / 2);
            break;
        case ALIGN_RIGHT:
            startX = x - stringWidth;
            break;
    }
    
    // Clamp to display bounds
    if (startX < 0) startX = 0;
    if (startX + stringWidth > 128) startX = 128 - stringWidth;
    
    // Draw each character
    for (int i = 0; str[i] != '\0'; i++) {
        int charX = startX + (i * 6);
        if (charX + 5 >= 128) break;  // Stop if off right edge
        drawChar(charX, y, str[i]);
    }
}

/**
 * Get buffer pointer for direct manipulation
 */
unsigned char* NHD_C12832::getBuffer() {
    return buffer;
}

/**
 * Get buffer size
 */
int NHD_C12832::getBufferSize() {
    return sizeof(buffer);
}

