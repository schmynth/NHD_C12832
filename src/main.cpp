#include <iostream>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <cstring>
#include "display.h"


// ============================================================================
// EXAMPLE USAGE
// ============================================================================

int main() {
    std::cout << "=== NHD-C12832A1Z Display Test ===" << std::endl;

    NHD_C12832 display;

    display.setContrast(0x1F);  // Low contrast
    // Test 1: Clear and draw diagonal lines
    std::cout << "\n[Test 1] Drawing diagonal lines..." << std::endl;
    display.clearBuffer();
    display.drawLine(0, 0, 127, 31);
    display.drawLine(127, 0, 0, 31);
    display.display();
    sleep(2);

    // Test 2: Draw rectangles
    std::cout << "[Test 2] Drawing rectangles..." << std::endl;
    display.clearBuffer();
    display.drawRect(10, 5, 50, 20);
    display.fillRect(70, 5, 40, 20);
    display.display();
    sleep(2);

    // Test 3: Test contrast levels
    std::cout << "[Test 3] Testing contrast..." << std::endl;
    display.setContrast(0x1F);  // Low contrast
    sleep(1);
    /*display.setContrast(0x3F);  // High contrast*/
    sleep(1);

    // Test 4: Grid pattern
    std::cout << "[Test 4] Drawing grid pattern..." << std::endl;
    display.clearBuffer();
    for (int x = 0; x < 128; x += 16) {
        display.drawVLine(x, 0, 31);
    }
    for (int y = 0; y < 32; y += 8) {
        display.drawHLine(0, 127, y);
    }
    display.display();
    sleep(2);

    display.clearBuffer();
    display.display();
    
    // Test 5: Grid pattern
    std::cout << "[Test 5] Printing text..." << std::endl;
    display.clearBuffer();
    
    // Print text at specific positions
    display.printString(0, 0, "Hello");      // Line 1 (page 0)
    display.printString(0, 1, "World");      // Line 2 (page 1)
    display.printString(0, 2, "128x32");    // Line 3 (page 2)
    display.printString(64, 3, "Centered", ALIGN_CENTER);
    
    display.display();  // Send buffer to display

    std::cout << "\n=== Tests Complete ===" << std::endl;

    /*U8G2_ST7565_NHD_C12832_1_4W_SW_SPI u8g2(U8G2_R0, SCL_PIN, SI_PIN, CS_PIN, A0_PIN, RST_PIN);*/
    /*u8g2.clearBuffer();*/
    /*u8g2.setFont(u8g2_font_ncenB14_tr);*/
    /*u8g2.drawStr(0,20,"Hello World!");*/
    /*u8g2.sendBuffer();*/
    return 0;

}
