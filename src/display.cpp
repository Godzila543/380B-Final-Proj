/*
 * OLED Display implementation using Adafruit SSD1306 - Optimized
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "display.h"
#include "config.h"
#include <math.h> // For isnan

// Define the display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Display object with the I2C address 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Optimization: State Tracking ---
struct DisplayState
{
    float angle = NAN;
    float avgLux = NAN;
    float temp = NAN;
    float humidityValue = NAN;
    int lastArrowX = -1, lastArrowY = -1;
    int lastArrowX2 = -1, lastArrowY2 = -1;
    int lastArrowX3 = -1, lastArrowY3 = -1;
};
DisplayState lastState;
// --- End Optimization ---

// Helper function to draw text and clear background first
void drawText(int x, int y, const char *text, bool clearBackground = true)
{
    if (clearBackground)
    {
        // Clear the background area where text will be drawn
        // Assuming text size 1 (6x8 font), clear a slightly larger area
        display.fillRect(x, y, 70, 8, SSD1306_BLACK); // Adjust width as needed
    }
    display.setCursor(x, y);
    display.print(text);
}

void initDisplay()
{
    Serial.println(F("Starting display initialization..."));

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        while (true)
        {
            delay(100);
        }
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // --- Optimization: Draw static elements once ---
    display.setCursor(0, 0);
    display.println(F("Light Tracker"));
    display.drawLine(0, 10, 80, 10, SSD1306_WHITE); // Adjusted line length
    // display.drawLine(80, 0, 80, 64, SSD1306_WHITE); // Adjusted divider position
    // --- End Optimization ---

    display.display(); // Show static elements
    Serial.println(F("Display initialized"));
}

void updateDisplay(float angle, float avgLux, const SensorData &data, float temp, float humidityValue)
{
    char buffer[24];
    bool changed = false;

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Use background color for overwrite

    // --- Optimization: Update values only if changed ---
    // Show angle
    // Use a tolerance for float comparison or check if NAN
    if (isnan(lastState.angle) || fabs(angle - lastState.angle) > 0.1)
    {
        snprintf(buffer, sizeof(buffer), "Ang: %.1f", angle);
        drawText(0, 15, buffer);
        lastState.angle = angle;
        changed = true;
    }

    // Show average lux
    if (isnan(lastState.avgLux) || fabs(avgLux - lastState.avgLux) > 0.1)
    {
        snprintf(buffer, sizeof(buffer), "Lux: %.1f", avgLux);
        drawText(0, 25, buffer);
        lastState.avgLux = avgLux;
        changed = true;
    }

    // Show temperature
    if (isnan(lastState.temp) || fabs(temp - lastState.temp) > 0.1)
    {
        snprintf(buffer, sizeof(buffer), "Tmp: %.1f", temp);
        drawText(0, 35, buffer);
        lastState.temp = temp;
        changed = true;
    }

    // Show humidity
    if (isnan(lastState.humidityValue) || fabs(humidityValue - lastState.humidityValue) > 0.1)
    {
        snprintf(buffer, sizeof(buffer), "Hum: %.1f", humidityValue);
        drawText(0, 45, buffer);
        lastState.humidityValue = humidityValue;
        changed = true;
    }
    // --- End Optimization ---

    // --- Optimization: Update arrow only if angle changed ---
    if (changed)
    {                       // Only redraw arrow if any value changed (or force redraw if needed)
        const int cx = 100; // Adjusted center x
        const int cy = 32;
        const int r = 25;

        // Erase the old arrow (line and triangle)
        if (lastState.lastArrowX != -1)
        {
            display.drawLine(cx, cy, lastState.lastArrowX, lastState.lastArrowY, SSD1306_BLACK);
            display.fillTriangle(lastState.lastArrowX, lastState.lastArrowY,
                                 lastState.lastArrowX2, lastState.lastArrowY2,
                                 lastState.lastArrowX3, lastState.lastArrowY3, SSD1306_BLACK);
        }
        // Erase the old circle (optional, if it interferes)
        // display.drawCircle(cx, cy, r, SSD1306_BLACK);

        // Draw the circle (can be moved to init if static)
        display.drawCircle(cx, cy, r, SSD1306_WHITE);

        // Calculate new arrow tip position
        float rad = angle * PI / 180.0;
        int arrowX = cx + r * cos(rad);
        int arrowY = cy - r * sin(rad);

        // Draw the new line
        display.drawLine(cx, cy, arrowX, arrowY, SSD1306_WHITE);

        // Draw new arrow head (triangle)
        const int headSize = 6;
        float perpRad = rad + PI / 2;
        int x1 = arrowX;
        int y1 = arrowY;
        int x2 = arrowX - headSize * cos(rad) + (headSize / 2) * cos(perpRad);
        int y2 = arrowY + headSize * sin(rad) - (headSize / 2) * sin(perpRad);
        int x3 = arrowX - headSize * cos(rad) - (headSize / 2) * cos(perpRad);
        int y3 = arrowY + headSize * sin(rad) + (headSize / 2) * sin(perpRad);
        display.fillTriangle(x1, y1, x2, y2, x3, y3, SSD1306_WHITE);

        // Store new arrow position
        lastState.lastArrowX = x1;
        lastState.lastArrowY = y1;
        lastState.lastArrowX2 = x2;
        lastState.lastArrowY2 = y2;
        lastState.lastArrowX3 = x3;
        lastState.lastArrowY3 = y3;
    }
    // --- End Optimization ---

    // Send the buffer to the display only if something changed
    if (changed)
    {
        display.display();
    }
}