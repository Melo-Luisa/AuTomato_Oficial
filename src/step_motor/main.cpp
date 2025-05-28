/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-drv8825-stepper-motor-driver
 */

#include <AccelStepper.h>

#define STEP_PIN 12 // The ESP32 pin GPIO12 connected to STEP pin of DRV8825 module
#define DIR_PIN  14 // The ESP32 pin GPIO14 connected to DIR pin of DRV8825 module

// Creates an instance
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

void setup() {
  // set the maximum speed, acceleration factor
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(200);
  // set speed and the target position
  stepper.setSpeed(200);
  stepper.moveTo(200);
}

void loop() {
  // Change direction once the motor reaches target position
  if (stepper.distanceToGo() == 0)
    stepper.moveTo(-stepper.currentPosition());

  stepper.run();  // Move the motor one step
}
