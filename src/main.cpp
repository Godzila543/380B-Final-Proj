/*
 * Light Gradient Tracking System
 * Uses 3 TSL2561 lux sensors to determine light gradient and points servo toward increasing light
 * Displays information on SSD1306 OLED display
 */

#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "sensors.h"
#include "gradient.h"
#include "display.h"
// #include "servo_control.h"
#include "temperature.h"
#include "ourSD.h"

// Global variables
float currentAngle = 90.0;    // Starting angle
float avgLux = 0.0;           // Average lux value
float currentTemp = 25.0;     // Default temperature value
float currentHumidity = 50.0; // Default humidity value

uSD sdCard(false); // SD card object with debug mode enabled

// Timing variables
unsigned long timeSensors = 0;
unsigned long timeCalcs = 0;
unsigned long timeTempHum = 0;
unsigned long timeDisplay = 0;
unsigned long timeSDFormat = 0;
unsigned long timeSDWrite = 0;
unsigned long timeSDLoop = 0;
unsigned long timeTotalLoop = 0;
unsigned long loopStartTime = 0;

// Cycle counter for temperature/humidity readings
uint8_t tempHumCounter = 0;
const uint8_t TEMP_HUM_INTERVAL = 20; // Read temperature/humidity every 20 cycles

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ; // Wait for serial connection on some boards
    Serial.println(F("Light Gradient Tracking System - Timing Enabled"));

    Wire.setClock(400000); // Set I2C frequency to 400kHz (ensure components support this)
    Wire.begin();

    // Initialize subsystems
    initSensors();
    // initServo();
    initDisplay();
    initTemp();
    sdCard.setup();
    sdCard.write_data("Time (ms), Lux1, Lux2, Lux3, Angle (degrees), Temp (celcius), Humidity (Relative %)\n");

    // Take initial temperature and humidity readings
    currentTemp = tempInC();
    currentHumidity = humidity();

    delay(100);
    Serial.println("Setup complete. Starting loop profiling...");
}

void loop()
{
    loopStartTime = micros(); // Start timing the entire loop

    // --- Read sensor data ---
    unsigned long start = micros();
    SensorData data = readAllSensors();
    timeSensors = micros() - start;

    // --- Calculations ---
    start = micros();
    avgLux = (data.lux1 + data.lux2 + data.lux3) / 3.0;
    float gradientX, gradientY;
    calculateGradient(data.lux1, data.lux2, data.lux3, gradientX, gradientY);
    float angle = atan2(gradientY, gradientX) * 180.0 / PI;
    currentAngle = angle; // Store the raw angle for logging
    timeCalcs = micros() - start;

    // --- Read Temp/Humidity (only every TEMP_HUM_INTERVAL cycles) ---
    start = micros();
    if (tempHumCounter == 0)
    {
        // Only read temperature and humidity every TEMP_HUM_INTERVAL cycles
        currentTemp = tempInC();
        currentHumidity = humidity();
    }
    // Increment and wrap counter
    tempHumCounter = (tempHumCounter + 1) % TEMP_HUM_INTERVAL;
    timeTempHum = micros() - start;

    // --- Update display ---
    start = micros();
    // Pass the raw angle to the display function
    updateDisplay(angle, avgLux, data, currentTemp, currentHumidity);
    timeDisplay = micros() - start;

    // --- Format data for SD card ---
    start = micros();
    char buffer[200]; // Reduced buffer size - 1000 was excessive
    snprintf(buffer, sizeof(buffer), "%ld,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
             millis(), data.lux1, data.lux2, data.lux3, currentAngle, currentTemp, currentHumidity);
    timeSDFormat = micros() - start;

    // --- Write data to SD card ---
    start = micros();
    sdCard.write_data(buffer);
    timeSDWrite = micros() - start;

    // --- SD Card background tasks ---
    start = micros();
    sdCard.loop(); // Perform any background SD card operations (like flushing)
    timeSDLoop = micros() - start;

    timeTotalLoop = micros() - loopStartTime; // End timing the entire loop

    // --- Print Timing Profile ---
    // Print timings periodically to avoid flooding Serial
    static unsigned long lastPrintTime = 0;
    if (millis() - lastPrintTime > 1000) // Print every 1000ms (1 second)
    {
        Serial.print("Timings (us) - Total: ");
        Serial.print(timeTotalLoop);
        Serial.print(" | Sensors: ");
        Serial.print(timeSensors);
        Serial.print(" | Calcs: ");
        Serial.print(timeCalcs);
        Serial.print(" | TempHum: ");
        Serial.print(timeTempHum);
        Serial.print(" | Display: ");
        Serial.print(timeDisplay);
        Serial.print(" | SDFormat: ");
        Serial.print(timeSDFormat);
        Serial.print(" | SDWrite: ");
        Serial.print(timeSDWrite);
        Serial.print(" | SDLoop: ");
        Serial.println(timeSDLoop);
        Serial.print("Loop frequency (Hz): ");
        Serial.println(1000000.0 / timeTotalLoop);
        lastPrintTime = millis();
    }
}