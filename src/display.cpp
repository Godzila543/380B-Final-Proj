/*
 * OLED Display implementation using Adafruit SSD1306
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "display.h"
#include "config.h"

// Define the display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Display object with the I2C address 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void initDisplay()
{
    Serial.println(F("Starting display initialization..."));

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        // Keep trying until it works - this will hang if the display is missing
        while (true)
        {
            delay(100);
        }
    }

    // Clear the buffer
    display.clearDisplay();

    // Set text properties
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    // Show initial display message
    display.println(F("Starting..."));
    display.display();

    Serial.println(F("Display initialized"));
}

void updateDisplay(float angle, float avgLux, const SensorData &data, float temp, float humidityValue)
{
    // Clear the display
    display.clearDisplay();

    // Title
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Light Tracker"));

    // Draw horizontal line below title
    display.drawLine(0, 10, 78, 10, SSD1306_WHITE);

    // Draw vertical divider line
    display.drawLine(78, 0, 78, 64, SSD1306_WHITE);

    // Display values
    char buffer[24];

    // Show angle
    display.setCursor(0, 15);
    snprintf(buffer, sizeof(buffer), "Ang: %.1f", angle);
    display.println(buffer);

    // Show average lux
    display.setCursor(0, 25);
    snprintf(buffer, sizeof(buffer), "Lux: %.1f", avgLux);
    display.println(buffer);

    // Show temperature
    display.setCursor(0, 35);
    snprintf(buffer, sizeof(buffer), "Tmp: %.1f", temp);
    display.println(buffer);

    // Show humidity
    display.setCursor(0, 45);
    snprintf(buffer, sizeof(buffer), "Hum: %.1f", humidityValue);
    display.println(buffer);

    // Draw direction indicator with arrow
    const int cx = 105; // center x
    const int cy = 32;  // center y
    const int r = 25;   // radius

    // Draw circle
    display.drawCircle(cx, cy, r, SSD1306_WHITE);

    // Calculate arrow tip position
    float rad = angle * PI / 180.0;
    int arrowX = cx + r * cos(rad);
    int arrowY = cy - r * sin(rad); // Y is inverted in screen coordinates

    // Draw the line from center to tip
    display.drawLine(cx, cy, arrowX, arrowY, SSD1306_WHITE);

    // Draw arrow head (triangle)
    const int headSize = 6;
    float perpRad = rad + PI / 2;

    // Calculate triangle points
    int x1 = arrowX;
    int y1 = arrowY;
    int x2 = arrowX - headSize * cos(rad) + (headSize / 2) * cos(perpRad);
    int y2 = arrowY + headSize * sin(rad) - (headSize / 2) * sin(perpRad);
    int x3 = arrowX - headSize * cos(rad) - (headSize / 2) * cos(perpRad);
    int y3 = arrowY + headSize * sin(rad) + (headSize / 2) * sin(perpRad);

    // Draw triangle
    display.fillTriangle(x1, y1, x2, y2, x3, y3, SSD1306_WHITE);

    // Send the buffer to the display
    display.display();
}