/*
 * Servo control implementation
 */

#include <Servo.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include "servo_control.h"
#include "config.h"

// Create objects
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Servo trackingServo;

void initServo()
{
    // Initialize motor shield
    AFMS.begin();

    // Attach servo to specified pin (corresponds to Servo1 on the shield)
    trackingServo.attach(SERVO_PIN);

    // Set to middle position
    trackingServo.write(90);

    Serial.println(F("Servo initialized"));
}

void setServoAngle(int angle)
{
    // Constrain angle to valid range
    angle = constrain(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

    // Set servo position
    trackingServo.write(angle);
}