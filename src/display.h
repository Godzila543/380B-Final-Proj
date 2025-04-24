/*
 * OLED Display functions
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "sensors.h"

// Initialize the OLED display
void initDisplay();

// Update the display with current information
void updateDisplay(float angle, float avgLux, const SensorData &data, float temp, float humidityValue);

#endif