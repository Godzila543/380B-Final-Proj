/*
 * Sensors implementation - Optimized for speed
 */

#include "sensors.h"
#include "config.h"

// Create sensor objects with unique IDs
Adafruit_TSL2561_Unified sensor1 = Adafruit_TSL2561_Unified(SENSOR1_ADDR, 12345);
Adafruit_TSL2561_Unified sensor2 = Adafruit_TSL2561_Unified(SENSOR2_ADDR, 12346);
Adafruit_TSL2561_Unified sensor3 = Adafruit_TSL2561_Unified(SENSOR3_ADDR, 12347);

void initSensors()
{
    // Initialize sensors
    if (!sensor1.begin())
    {
        Serial.println(F("Sensor 1 not found - check wiring!"));
        while (1)
            ;
    }
    if (!sensor2.begin())
    {
        Serial.println(F("Sensor 2 not found - check wiring!"));
        while (1)
            ;
    }
    if (!sensor3.begin())
    {
        Serial.println(F("Sensor 3 not found - check wiring!"));
        while (1)
            ;
    }

    // Configure all sensors
    configureSensor(sensor1);
    configureSensor(sensor2);
    configureSensor(sensor3);

    Serial.println(F("All sensors initialized for fast reading"));
}

void configureSensor(Adafruit_TSL2561_Unified &sensor)
{
    // --- SPEED OPTIMIZATION ---

    // 1. Disable auto-range - it's slower because it does multiple readings
    sensor.enableAutoRange(false);

    // 2. Set to fastest integration time (13ms) - major speed improvement
    sensor.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); // 13ms is fastest

    // 3. Choose appropriate gain setting
    // For normal indoor lighting, TSL2561_GAIN_1X works well and is faster
    // For low light conditions, you might need TSL2561_GAIN_16X
    sensor.setGain(TSL2561_GAIN_1X); // No gain (faster response)

    // --- END SPEED OPTIMIZATION ---
}

float readLux(Adafruit_TSL2561_Unified &sensor)
{
    sensors_event_t event;
    sensor.getEvent(&event);

    // Avoid Serial printing in timing-critical code
    if (event.light)
    {
        return event.light;
    }
    else
    {
        // Use a constant return value instead of Serial printing
        // Serial.println(F("Error reading light value!"));
        return 0;
    }
}

SensorData readAllSensors()
{
    SensorData data;

    // Read lux values from all sensors
    data.lux1 = readLux(sensor1);
    data.lux2 = readLux(sensor2);
    data.lux3 = readLux(sensor3);

    return data;
}