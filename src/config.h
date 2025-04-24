/*
 * Configuration file for Light Gradient Tracking System
 */

#ifndef CONFIG_H
#define CONFIG_H

// #define SENSOR_1_XPOS 5.0
// #define SENSOR_1_YPOS 1.0
// #define SENSOR_2_XPOS -9.7
// #define SENSOR_2_YPOS 5.0
// #define SENSOR_3_XPOS -9.7
// #define SENSOR_3_YPOS -4.5

// #def

// Sensor positions (x,y) in cm relative to center
// Triangular pattern with sensors at the corners
const float SENSOR1_POS[2] = {5.0, 1};     // Right
const float SENSOR2_POS[2] = {-9.7, 5.0};  // Top left
const float SENSOR3_POS[2] = {-9.7, -4.5}; // Bottom left

// Servo control settings
const int SERVO_PIN = 9;         // Servo signal pin (Servo1 on the shield)
const int SERVO_MIN_ANGLE = 0;   // Minimum servo angle
const int SERVO_MAX_ANGLE = 180; // Maximum servo angle

// Display settings
const int SCREEN_WIDTH = 128;    // OLED display width, in pixels
const int SCREEN_HEIGHT = 64;    // OLED display height, in pixels
const int OLED_RESET = -1;       // Reset pin (-1 if sharing Arduino reset pin)
const int SCREEN_ADDRESS = 0x3C; // I2C address for the display (typically 0x3C or 0x3D)

// Sensor I2C addresses
// TSL2561 has three possible addresses based on the ADDR pin connection
#define SENSOR1_ADDR TSL2561_ADDR_FLOAT // ADDR pin floating
#define SENSOR2_ADDR TSL2561_ADDR_LOW   // ADDR pin connected to GND
#define SENSOR3_ADDR TSL2561_ADDR_HIGH  // ADDR pin connected to VCC

// System settings
const int UPDATE_DELAY = 500; // Delay between updates in milliseconds

// Data wire is plugged into port 2 on the Arduino
// #define ONE_WIRE_BUS 2

#endif