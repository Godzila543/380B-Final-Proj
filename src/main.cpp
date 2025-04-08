/*
 * Light Gradient Tracking System
 * Uses 3 TSL2561 lux sensors to determine light gradient and points servo toward increasing light
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_MotorShield.h>
#include <Servo.h>

void configureSensor(Adafruit_TSL2561_Unified &sensor);
float readLux(Adafruit_TSL2561_Unified &sensor);
void calculateGradient(float lux1, float lux2, float lux3, float &gradientX, float &gradientY);

// Create sensor objects with unique IDs
Adafruit_TSL2561_Unified sensor1 = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
Adafruit_TSL2561_Unified sensor2 = Adafruit_TSL2561_Unified(TSL2561_ADDR_LOW, 12346);
Adafruit_TSL2561_Unified sensor3 = Adafruit_TSL2561_Unified(TSL2561_ADDR_HIGH, 12347);

// Motor shield setup
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Create a servo object
Servo trackingServo;

// Sensor positions (x,y) in cm relative to center
// Assuming a triangular pattern with sensors at the corners
const float sensor1Pos[2] = {0.0, 5.0};   // Top
const float sensor2Pos[2] = {-5.0, -5.0}; // Bottom left
const float sensor3Pos[2] = {5.0, -5.0};  // Bottom right

void setup()
{
  Serial.begin(9600);
  Serial.println("Light Gradient Tracking System with Adafruit Motor Shield");

  // Initialize motor shield
  AFMS.begin();

  // Attach servo to pin 9 (Servo1 port on the shield)
  trackingServo.attach(9);

  // Initialize servo to middle position (90 degrees)
  trackingServo.write(90);

  // Initialize sensors
  if (!sensor1.begin())
  {
    Serial.println("Sensor 1 not found - check wiring!");
    while (1)
      ;
  }
  if (!sensor2.begin())
  {
    Serial.println("Sensor 2 not found - check wiring!");
    while (1)
      ;
  }
  if (!sensor3.begin())
  {
    Serial.println("Sensor 3 not found - check wiring!");
    while (1)
      ;
  }

  // Configure gain and integration time for all sensors
  configureSensor(sensor1);
  configureSensor(sensor2);
  configureSensor(sensor3);

  delay(1000);
}

void configureSensor(Adafruit_TSL2561_Unified &sensor)
{
  // You can change the gain and integration time based on your light conditions
  sensor.enableAutoRange(true); // Auto-gain

  // Alternatively, set manual gain:
  // sensor.setGain(TSL2561_GAIN_1X);      // No gain (bright light)
  // sensor.setGain(TSL2561_GAIN_16X);     // 16x gain (dim light)

  // Set integration time
  sensor.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);
}

void loop()
{
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y)
{
  return x + y;
}