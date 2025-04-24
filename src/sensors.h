/*
 * Sensors management for Light Gradient Tracking System
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

// Structure to hold sensor readings
struct SensorData
{
    float lux1;
    float lux2;
    float lux3;
};

// Initialize the light sensors
void initSensors();

// Configure a single sensor
void configureSensor(Adafruit_TSL2561_Unified &sensor);

// Read lux value from a sensor
float readLux(Adafruit_TSL2561_Unified &sensor);

// Read all sensors and return the data
SensorData readAllSensors();

#endif