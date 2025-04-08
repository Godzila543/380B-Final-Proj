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
const float sensor1Pos[2] = {-1.0, 7.2};   // Top
const float sensor2Pos[2] = {-5.0, -7.2}; // Bottom left
const float sensor3Pos[2] = {-4.7, -7.2};  // Bottom right

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
  // Read lux values from all sensors
  float lux1 = readLux(sensor1);
  float lux2 = readLux(sensor2);
  float lux3 = readLux(sensor3);

  // Print lux values for debugging
  Serial.print("Sensor 1: ");
  Serial.print(lux1);
  Serial.println(" lux");
  Serial.print("Sensor 2: ");
  Serial.print(lux2);
  Serial.println(" lux");
  Serial.print("Sensor 3: ");
  Serial.print(lux3);
  Serial.println(" lux");

  // Calculate gradient vector
  float gradientX, gradientY;
  calculateGradient(lux1, lux2, lux3, gradientX, gradientY);

  // Calculate angle from gradient vector
  float angle = atan2(gradientY, gradientX) * 180.0 / PI;

  // Convert angle to servo position (0-180)
  // Map the angle (-180 to 180) to servo range (0-180)
  int servoPosition = map(angle, -180, 180, 0, 180);

  Serial.print("Gradient direction: ");
  Serial.print(angle);
  Serial.println(" degrees");

  Serial.print("Servo position: ");
  Serial.println(servoPosition);

  // Move servo to point in the direction of increasing light
  trackingServo.write(servoPosition);

  // Add a small delay
  delay(500);

  Serial.println();
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
    Serial.println("Error reading light value!");
    return 0;
  }
}

// Calculate the gradient of light field using the three sensor readings
void calculateGradient(float lux1, float lux2, float lux3, float &gradientX, float &gradientY)
{
  // This function calculates the gradient vector of the light field
  // using a planar approximation from three sensor readings

  // Create matrices for least squares solution to find the plane
  // We're solving for a plane of the form: lux = a*x + b*y + c
  // Where (a,b) is our gradient vector

  // Matrix form: [x, y, 1] * [a, b, c]^T = [lux]

  // Simple calculation for the plane coefficients (gradient)
  // Using the formula from planar interpolation
  float denominator = (sensor1Pos[0] * (sensor2Pos[1] - sensor3Pos[1]) +
                       sensor2Pos[0] * (sensor3Pos[1] - sensor1Pos[1]) +
                       sensor3Pos[0] * (sensor1Pos[1] - sensor2Pos[1]));

  if (abs(denominator) < 0.0001)
  {
    // Handle the case where sensors are collinear
    gradientX = 0;
    gradientY = 0;
    return;
  }

  // Calculate partial derivatives (gradient components)
  gradientX = (lux1 * (sensor2Pos[1] - sensor3Pos[1]) +
               lux2 * (sensor3Pos[1] - sensor1Pos[1]) +
               lux3 * (sensor1Pos[1] - sensor2Pos[1])) /
              denominator;

  gradientY = (lux1 * (sensor3Pos[0] - sensor2Pos[0]) +
               lux2 * (sensor1Pos[0] - sensor3Pos[0]) +
               lux3 * (sensor2Pos[0] - sensor1Pos[0])) /
              denominator;

  // Normalize gradient direction (but keep magnitude for strength)
  float magnitude = sqrt(gradientX * gradientX + gradientY * gradientY);

  if (magnitude > 0)
  {
    // We keep the magnitude information for potential future use
    // but normalize for consistency in direction
    float normFactor = magnitude;
    gradientX /= normFactor;
    gradientY /= normFactor;
  }
}