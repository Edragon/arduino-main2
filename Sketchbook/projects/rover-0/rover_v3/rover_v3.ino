/*
 * Rover V3 Control System
 * Using PCA9685 PWM Controller for Motor Control
 * 
 * Channel Assignment:
 * - Channel 0: Motor 1 Direction A (IN1)
 * - Channel 1: Motor 1 Direction B (IN2)
 * - Channel 2: Motor 1 Enable A (ENA)
 * - Channel 3: Motor 1 Enable B (ENB)
 * - Channel 4: Motor 2 Direction A (IN1)
 * - Channel 5: Motor 2 Direction B (IN2)
 * - Channel 6: Motor 2 Enable A (ENA)
 * - Channel 7: Motor 2 Enable B (ENB)
 * 
 * Motor Control Logic:
 * - Forward: IN1=HIGH, IN2=LOW, ENA=PWM, ENB=0
 * - Reverse: IN1=LOW, IN2=HIGH, ENA=PWM, ENB=0
 * - Stop: IN1=LOW, IN2=LOW, ENA=0, ENB=0
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// PCA9685 setup
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40); // Set I2C address to 0x40

// Motor channel definitions
#define MOTOR1_IN1    0  // Channel 0 - Motor 1 Direction A
#define MOTOR1_IN2    1  // Channel 1 - Motor 1 Direction B
#define MOTOR1_ENA    2  // Channel 2 - Motor 1 Enable A
#define MOTOR1_ENB    3  // Channel 3 - Motor 1 Enable B

// Motor 2 channel definitions
#define MOTOR2_IN1    4  // Channel 4 - Motor 2 Direction A
#define MOTOR2_IN2    5  // Channel 5 - Motor 2 Direction B
#define MOTOR2_ENA    6  // Channel 6 - Motor 2 Enable A
#define MOTOR2_ENB    7  // Channel 7 - Motor 2 Enable B

// PWM frequency and resolution
#define PWM_FREQ      1000  // 1kHz PWM frequency
#define PWM_MAX       4095  // 12-bit PWM resolution

void setup() {
  Serial.begin(115200);
  Serial.println("Rover V3 - PCA9685 Motor Control");
  
  // Initialize PCA9685
  pwm.begin();
  pwm.setPWMFreq(PWM_FREQ);
  delay(10);
  
  // Set ENA and ENB always HIGH
  pwm.setPWM(MOTOR1_ENA, 0, PWM_MAX);
  pwm.setPWM(MOTOR1_ENB, 0, PWM_MAX);
  pwm.setPWM(MOTOR2_ENA, 0, PWM_MAX);
  pwm.setPWM(MOTOR2_ENB, 0, PWM_MAX);
  
  // Set motor forward
  pwm.setPWM(MOTOR1_IN1, 0, PWM_MAX);      // IN1 HIGH
  pwm.setPWM(MOTOR1_IN2, 0, 0);           // IN2 LOW
  pwm.setPWM(MOTOR2_IN1, 0, PWM_MAX);      // IN1 HIGH
  pwm.setPWM(MOTOR2_IN2, 0, 0);           // IN2 LOW
  
  Serial.println("Motor running forward (ENA/ENB always HIGH)");
}

void loop() {
  // Nothing to do in loop for simple forward run
}
