/*
 * Gradient calculation functions
 */

#ifndef GRADIENT_H
#define GRADIENT_H

// Calculate the gradient of light field using the three sensor readings
void calculateGradient(float lux1, float lux2, float lux3, float &gradientX, float &gradientY);
// void normalizeSensorPos();
void normalizeVector(float &x, float &y);

#endif