/*
 * OLED Display implementation using U8g2
 */

#include <Wire.h>
#include <U8g2lib.h>
#include "display.h"
#include "config.h"

// Using a constructor with explicit I2C address and lower memory usage (page buffer mode)
// The 1 at the end means page buffer mode (uses less RAM than full buffer 'F' mode)
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 0x3C);

void initDisplay()
{
    // Set lower I2C speed to avoid power issues
    // Wire.setClock(100000); // Use 100kHz instead of default 400kHz
    // Wire.begin();
    // delay(100); // Short delay to let I2C settle

    Serial.println(F("Starting display initialization..."));

    // Initialize with a more minimal setup
    u8g2.begin();
    Serial.println(F("Display initialized"));

    // Continue with setup (minimal version)
    u8g2.setFont(u8g2_font_5x8_tf); // A smaller font uses less RAM

    // Simple initial display message
    u8g2.firstPage();
    do
    {
        u8g2.drawStr(0, 0, "Starting...");
    } while (u8g2.nextPage());
}

void updateDisplay(float angle, float avgLux, const SensorData &data, float temp, float humidityValue)
{
    // Using page buffer mode - start drawing sequence
    u8g2.firstPage();
    do
    {
        char buffer[24]; // Larger buffer to be safe

        // Title
        u8g2.setFont(u8g2_font_5x8_tf);
        u8g2.drawStr(0, 10, "Light Tracker");
        u8g2.drawLine(0, 12, 70, 12);
        u8g2.drawLine(70, 0, 70, 64);

        // Convert values to integers to avoid sprintf float issues
        int angleInt = (int)angle;
        int angleFrac = (int)((angle - angleInt) * 10);

        int luxInt = (int)avgLux;
        int luxFrac = (int)((avgLux - luxInt) * 10);

        int tempInt = (int)temp;
        int tempFrac = (int)((temp - tempInt) * 10);

        int humInt = (int)humidityValue;
        int humFrac = (int)((humidityValue - humInt) * 10);

        // Show angle using integer arithmetic to avoid float formatting issues
        snprintf(buffer, sizeof(buffer), "Ang: %d.%d", angleInt, angleFrac);
        u8g2.drawStr(0, 20, buffer);

        // Show average lux
        snprintf(buffer, sizeof(buffer), "Lux: %d.%d", luxInt, luxFrac);
        u8g2.drawStr(0, 30, buffer);

        // Show temperature
        snprintf(buffer, sizeof(buffer), "Tmp: %d.%d", tempInt, tempFrac);
        u8g2.drawStr(0, 40, buffer);

        // Show humidity
        snprintf(buffer, sizeof(buffer), "Hum: %d.%d", humInt, humFrac);
        u8g2.drawStr(0, 50, buffer);

        // Simple direction indicator with arrow
        const int cx = 100;
        const int cy = 32;
        const int r = 25;

        // Convert angle to radians for drawing
        float rad = angle * PI / 180.0;

        // Calculate the tip of the arrow
        int arrowX = cx + r * cos(rad);
        int arrowY = cy - r * sin(rad);

        // Draw the main line
        u8g2.drawLine(cx, cy, arrowX, arrowY);

        // Draw the circle
        u8g2.drawCircle(cx, cy, r);

        // Draw arrow head (triangle at the tip)
        // Calculate perpendicular angle for the arrow head
        float perpRad = rad + PI / 2;

        // Arrow head size
        const int headSize = 9;

        // Calculate points for triangle
        int x1 = arrowX;
        int y1 = arrowY;

        // Calculate the other two points of the triangle
        int x2 = arrowX - headSize * cos(rad) + (headSize / 2) * cos(perpRad);
        int y2 = arrowY + headSize * sin(rad) - (headSize / 2) * sin(perpRad);

        int x3 = arrowX - headSize * cos(rad) - (headSize / 2) * cos(perpRad);
        int y3 = arrowY + headSize * sin(rad) + (headSize / 2) * sin(perpRad);

        // Draw triangle for arrow head
        u8g2.drawTriangle(x1, y1, x2, y2, x3, y3);

    } while (u8g2.nextPage());
}