/*
 * Gradient calculation implementation
 */

#include <Arduino.h>
#include "gradient.h"
#include "config.h"

void calculateGradient(float lux1, float lux2, float lux3, float &gradientX, float &gradientY)
{
    // This function calculates the gradient vector of the light field
    // using a planar approximation from three sensor readings

    // Simple calculation for the plane coefficients (gradient)
    // Using the formula from planar interpolation
    float denominator = (SENSOR1_POS[0] * (SENSOR2_POS[1] - SENSOR3_POS[1]) +
                         SENSOR2_POS[0] * (SENSOR3_POS[1] - SENSOR1_POS[1]) +
                         SENSOR3_POS[0] * (SENSOR1_POS[1] - SENSOR2_POS[1]));

    if (abs(denominator) < 0.0001)
    {
        // Handle the case where sensors are collinear
        gradientX = 0;
        gradientY = 0;
        return;
    }

    // Calculate partial derivatives (gradient components)
    gradientX = (lux1 * (SENSOR2_POS[1] - SENSOR3_POS[1]) +
                 lux2 * (SENSOR3_POS[1] - SENSOR1_POS[1]) +
                 lux3 * (SENSOR1_POS[1] - SENSOR2_POS[1])) /
                denominator;

    gradientY = (lux1 * (SENSOR3_POS[0] - SENSOR2_POS[0]) +
                 lux2 * (SENSOR1_POS[0] - SENSOR3_POS[0]) +
                 lux3 * (SENSOR2_POS[0] - SENSOR1_POS[0])) /
                denominator;

    // Normalize gradient direction (but keep magnitude for strength)
    float magnitude = sqrt(gradientX * gradientX + gradientY * gradientY);

    if (magnitude > 0)
    {
        // We keep the magnitude information for potential future use
        // but normalize for consistency in direction
        float normFactor = magnitude;
        // gradientX /= normFactor;
        // gradientY /= normFactor;
    }
}