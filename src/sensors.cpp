/*
 * Sensors implementation
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

    Serial.println(F("All sensors initialized"));
}

void configureSensor(Adafruit_TSL2561_Unified &sensor)
{
    // You can change the gain and integration time based on your light conditions
    sensor.enableAutoRange(true); // Auto-gain

    // Alternatively, set manual gain:
    // sensor.setGain(TSL2561_GAIN_1X);      // No gain (bright light)
    // sensor.setGain(TSL2561_GAIN_16X);     // 16x gain (dim light)

    // Set integration time
    sensor.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
}

float readLux(Adafruit_TSL2561_Unified &sensor)
{
    sensors_event_t event;
    sensor.getEvent(&event);

    // Check if reading is valid
    if (event.light)
    {
        return event.light;
    }
    else
    {
        Serial.println(F("Error reading light value!"));
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