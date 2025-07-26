/*
 * Rover V3 Control System
 * Using PCA9685 PWM Controller for Motor Control
 * ELRS PWM Input for Steering and Throttle
 * 
 * Hardware Connections:
 * - I2C SDA: GPIO4 (D2)
 * - I2C SCL: GPIO5 (D1)
 * - ELRS Channel 1 (Steering): GPIO12 (D6)
 * - ELRS Channel 2 (Throttle): GPIO13 (D7)
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

// Function declarations
void ICACHE_RAM_ATTR steeringISR();
void ICACHE_RAM_ATTR throttleISR();
void processPWMInputs();
void updateMotors();
void setMotorSpeed(int motor, int speed);
void stopMotors();

// PCA9685 setup
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

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

// ELRS PWM input pins
#define STEERING_PIN  12  // GPIO12 (D6) - Channel 1
#define THROTTLE_PIN  13  // GPIO13 (D7) - Channel 2

// PWM signal variables
volatile unsigned long steering_start = 0;
volatile unsigned long throttle_start = 0;
volatile int steering_pwm = 1500;  // Default center position
volatile int throttle_pwm = 1500;  // Default center position

// Motor control variables
int left_speed = 0;
int right_speed = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Rover V3 - ELRS PWM Control");
  
  // Initialize I2C with custom pins for ESP8266
  Wire.begin(4, 5); // SDA=GPIO4, SCL=GPIO5
  
  // Initialize PCA9685
  pwm.begin();
  pwm.setPWMFreq(PWM_FREQ);
  delay(10);
  
  // Debug: Test PCA9685 communication
  Serial.println("Testing PCA9685 communication...");
  pwm.setPWM(0, 0, 2048); // Test channel 0 with 50% duty cycle
  delay(100);
  pwm.setPWM(0, 0, 0);    // Turn off
  Serial.println("PCA9685 test complete");
  
  // Setup PWM input pins
  pinMode(STEERING_PIN, INPUT);
  pinMode(THROTTLE_PIN, INPUT);
  
  // Attach interrupts for PWM reading
  attachInterrupt(digitalPinToInterrupt(STEERING_PIN), steeringISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(THROTTLE_PIN), throttleISR, CHANGE);
  
  // Initialize motors to stop
  stopMotors();
  
  Serial.println("ELRS PWM Control Ready");
  Serial.print("PWM_MAX: "); Serial.println(PWM_MAX);
  Serial.print("PWM_FREQ: "); Serial.println(PWM_FREQ);
}

void loop() {
  // Read PWM values and convert to motor speeds
  processPWMInputs();
  
  // Update motor speeds
  updateMotors();
  
  // Consolidated debug output
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 500) { // Reduced frequency to avoid spam
    // Single line consolidated output
    Serial.print("ROVER: PWM[ST:"); Serial.print(steering_pwm);
    Serial.print(" TH:"); Serial.print(throttle_pwm);
    Serial.print("] SPEED[L:"); Serial.print(left_speed);
    Serial.print(" R:"); Serial.print(right_speed);
    Serial.print("] ");
    
    // Remove test mode to prevent unwanted motor movement
    if (abs(left_speed) < 25 && abs(right_speed) < 25) {
      Serial.println("MODE:IDLE");
      // Don't run any test motors when no RF input
    } else {
      Serial.println("MODE:ACTIVE");
    }
    
    lastDebug = millis();
  }
  
  delay(20);
}

// Interrupt service routine for steering channel
void ICACHE_RAM_ATTR steeringISR() {
  if (digitalRead(STEERING_PIN) == HIGH) {
    steering_start = micros();
  } else {
    unsigned long pulse_time = micros() - steering_start;
    if (pulse_time > 800 && pulse_time < 2200) {
      steering_pwm = pulse_time;
    }
  }
}

// Interrupt service routine for throttle channel
void ICACHE_RAM_ATTR throttleISR() {
  if (digitalRead(THROTTLE_PIN) == HIGH) {
    throttle_start = micros();
  } else {
    unsigned long pulse_time = micros() - throttle_start;
    if (pulse_time > 800 && pulse_time < 2200) {
      throttle_pwm = pulse_time;
    }
  }
}

void processPWMInputs() {
  // Convert PWM values to motor speeds (-255 to +255)
  int throttle = map(throttle_pwm, 1000, 2000, -255, 255);
  int steering = map(steering_pwm, 1000, 2000, -255, 255);
  
  // Apply deadband for inputs
  if (abs(throttle) < 20) throttle = 0;
  if (abs(steering) < 20) steering = 0;
  
  // Calculate differential steering
  left_speed = throttle + steering;
  right_speed = throttle - steering;
  
  // Constrain speeds
  left_speed = constrain(left_speed, -255, 255);
  right_speed = constrain(right_speed, -255, 255);
  
  // Apply minimum speed threshold to prevent small movements
  #define MIN_SPEED_THRESHOLD 30  // Minimum speed to activate motors
  
  if (abs(left_speed) < MIN_SPEED_THRESHOLD) left_speed = 0;
  if (abs(right_speed) < MIN_SPEED_THRESHOLD) right_speed = 0;
}

void updateMotors() {
  setMotorSpeed(1, left_speed);   // Motor 1 (Left)
  setMotorSpeed(2, right_speed);  // Motor 2 (Right)
}

void setMotorSpeed(int motor, int speed) {
  int in1_channel, in2_channel, ena_channel, enb_channel;
  
  if (motor == 1) {
    in1_channel = MOTOR1_IN1;
    in2_channel = MOTOR1_IN2;
    ena_channel = MOTOR1_ENA;
    enb_channel = MOTOR1_ENB;
  } else {
    in1_channel = MOTOR2_IN1;
    in2_channel = MOTOR2_IN2;
    ena_channel = MOTOR2_ENA;
    enb_channel = MOTOR2_ENB;
  }
  
  int pwm_value = map(abs(speed), 0, 255, 0, PWM_MAX);
  
  // Determine direction
  String direction = "STOP";
  if (speed > 0) direction = "FWD";
  else if (speed < 0) direction = "REV";
  
  // Remove individual motor debug output - only show in main loop
  
  if (speed > 0) {
    // Forward - Set both enables to MAX PWM for full power
    pwm.setPWM(in1_channel, 0, PWM_MAX);
    pwm.setPWM(in2_channel, 0, 0);
    pwm.setPWM(ena_channel, 0, PWM_MAX);
    pwm.setPWM(enb_channel, 0, PWM_MAX);
  } else if (speed < 0) {
    // Reverse - Set both enables to MAX PWM for full power
    pwm.setPWM(in1_channel, 0, 0);
    pwm.setPWM(in2_channel, 0, PWM_MAX);
    pwm.setPWM(ena_channel, 0, PWM_MAX);
    pwm.setPWM(enb_channel, 0, PWM_MAX);
  } else {
    // Stop
    pwm.setPWM(in1_channel, 0, 0);
    pwm.setPWM(in2_channel, 0, 0);
    pwm.setPWM(ena_channel, 0, 0);
    pwm.setPWM(enb_channel, 0, 0);
  }
}

void stopMotors() {
  setMotorSpeed(1, 0);
  setMotorSpeed(2, 0);
}
