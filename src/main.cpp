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
#include "servo_control.h"
#include "temperature.h"
#include "ourSD.h"

// Global variables
float currentAngle = 90.0; // Starting angle
float avgLux = 0.0;        // Average lux value

uSD sdCard(false); // SD card object with debug mode enabled

void setup()
{
    Serial.begin(9600);
    Serial.println(F("Light Gradient Tracking System"));

    Wire.setClock(100000); // Set I2C frequency to 100kHz
    Wire.begin();

    // Initialize subsystems
    initSensors();
    initServo();
    initDisplay();
    initTemp();
    sdCard.setup();
    // sdCard.write_data("Time, Lux1, Lux2, Lux3, Angle, Temp, Humidity\n");

    // normalizeSensorPos();

    delay(100);
}

void loop()
{
    // Read sensor data
    SensorData data = readAllSensors();

    // Calculate average lux
    avgLux = (data.lux1 + data.lux2 + data.lux3) / 3.0;

    // Calculate gradient and direction
    float gradientX, gradientY;
    calculateGradient(data.lux1, data.lux2, data.lux3, gradientX, gradientY);

    // Calculate angle from gradient vector
    float angle = atan2(gradientY, gradientX) * 180.0 / PI;

    // Convert angle (-180 to 180) to servo position (0-180)
    int servoPosition = map(angle, -180, 180, 0, 180);
    currentAngle = servoPosition;

    float temp = tempInC();
    float humidityValue = humidity();

    // Print debug info
    // Serial.print(F("Sensor readings (lux): "));
    // Serial.print(data.lux1);
    // Serial.print(F(", "));
    // Serial.print(data.lux2);
    // Serial.print(F(", "));
    // Serial.println(data.lux3);

    Serial.print(F("Gradient direction: "));
    Serial.print(angle);
    Serial.println(F(" degrees"));

    // Serial.print(F("Servo position: "));
    // Serial.println(servoPosition);

    // Move servo
    setServoAngle(servoPosition);

    // Update display
    updateDisplay(angle, avgLux, data, temp, humidityValue);

    // Write data to SD card
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%ld, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",
             millis(), data.lux1, data.lux2, data.lux3, currentAngle, temp, humidityValue);
    sdCard.write_data(buffer);

    // Small delay
    // delay(500);

    // Serial.println();
}