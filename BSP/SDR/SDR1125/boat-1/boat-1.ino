#include <AlfredoCRSF.h>
#include <HardwareSerial.h>

#define PIN_RX 17
#define PIN_TX 16

// Motor Driver DRV8871 Pins
// Motor 1 (Left): IO15, IO18
#define M1_IN1 15
#define M1_IN2 18

// Motor 2 (Right): IO7, IO8
#define M2_IN1 7
#define M2_IN2 8

// Channels
#define CH_THROTTLE 1    // Forward/Backward (CH1)
#define CH_STEERING 3    // Left/Right (CH3)
#define CH_SPEED_MODE 6  // Speed Mode (Low/Mid/High) (CH6)
#define CH_MOSFET1 5     // MOSFET 1 Control (CH5)
#define CH_MOSFET2 8     // MOSFET 2 Control (CH8)

// Motor speed scalers (to compensate for motor variance)
// Current issue: Left is faster, Right is slower
float LEFT_MOTOR_SCALER = 1.0;
float RIGHT_MOTOR_SCALER = 1.0;

// Reserved Servo IOs (not in use)
#define SERVO1_PIN 11
#define SERVO2_PIN 12
#define SERVO3_PIN 13
#define SERVO4_PIN 14

// Reserved Buzzer control pin (not in use)
#define BUZZER_PIN 46

// Reserved Relay IOs (not in use)
#define RELAY1_PIN 9
#define RELAY2_PIN 10

// Reserved MOSFET IOs (not in use)
#define MOSFET1_PIN 5
#define MOSFET2_PIN 6

// Battery Monitor
#define BATTERY_ADC_PIN 36

// Set up a new Serial object
HardwareSerial crsfSerial(1);
AlfredoCRSF crsf;

/**
 * Control a single motor
 * @param speed -255 to 255
 * @param pin1 Primary control pin
 * @param pin2 Secondary control pin
 */
void setMotor(int speed, int pin1, int pin2) {
  if (speed > 0) {
    if (speed > 255) speed = 255;
    analogWrite(pin1, speed);
    analogWrite(pin2, 0);
  } else if (speed < 0) {
    speed = -speed;
    if (speed > 255) speed = 255;
    analogWrite(pin1, 0);
    analogWrite(pin2, speed);
  } else {
    analogWrite(pin1, 0);
    analogWrite(pin2, 0);
  }
}

void setup()
{
  Serial.begin(115200);
  
  // Set PWM frequency to 10kHz to prevent DRV8871 overheating
#ifdef ESP32
  analogWriteFrequency(M1_IN1, 10000);
  analogWriteFrequency(M1_IN2, 10000);
  analogWriteFrequency(M2_IN1, 10000);
  analogWriteFrequency(M2_IN2, 10000);
#else
  analogWriteFreq(10000);
#endif

  Serial.println("Boat Controller initializing...");
  
  // Motor pins setup
  pinMode(M1_IN1, OUTPUT);
  pinMode(M1_IN2, OUTPUT);
  pinMode(M2_IN1, OUTPUT);
  pinMode(M2_IN2, OUTPUT);

  // MOSFET setup
  pinMode(MOSFET1_PIN, OUTPUT);
  pinMode(MOSFET2_PIN, OUTPUT);
  digitalWrite(MOSFET1_PIN, LOW);
  digitalWrite(MOSFET2_PIN, LOW);

  // Battery Monitor setup
  pinMode(BATTERY_ADC_PIN, INPUT);

  // Stop motors initially
  setMotor(0, M1_IN1, M1_IN2);
  setMotor(0, M2_IN1, M2_IN2);
  
#ifdef ESP32
  crsfSerial.begin(CRSF_BAUDRATE, SERIAL_8N1, PIN_RX, PIN_TX);
#else
  crsfSerial.begin(CRSF_BAUDRATE);
#endif

  if (!crsfSerial) {
    while (1) {
      Serial.println("Invalid crsfSerial configuration");
      delay(1000);
    }
  }

  crsf.begin(crsfSerial);
}

void loop()
{
  static bool wasStopped = true;
  crsf.update();

  if (crsf.isLinkUp()) {
    int throttleInput = crsf.getChannel(CH_THROTTLE); 
    int steeringInput = crsf.getChannel(CH_STEERING);
    int speedModeInput = crsf.getChannel(CH_SPEED_MODE);

    digitalWrite(MOSFET1_PIN, crsf.getChannel(CH_MOSFET1) > 1500 ? HIGH : LOW);
    digitalWrite(MOSFET2_PIN, crsf.getChannel(CH_MOSFET2) > 1500 ? HIGH : LOW);

    int maxLimit = 255;
    if (speedModeInput < 1300) maxLimit = 85;
    else if (speedModeInput < 1700) maxLimit = 170;

    int throttle = map(throttleInput, 1000, 2000, maxLimit, -maxLimit);
    int steering = map(steeringInput, 1000, 2000, maxLimit, -maxLimit);

    if (abs(throttle) < 20) throttle = 0;
    if (abs(steering) < 20) steering = 0;

    int leftSpeed = constrain((throttle + steering) * LEFT_MOTOR_SCALER, -255, 255);
    int rightSpeed = constrain((throttle - steering) * RIGHT_MOTOR_SCALER, -255, 255);

    if (leftSpeed != 0 || rightSpeed != 0) {
      if (wasStopped) {
        setMotor(leftSpeed, M1_IN1, M1_IN2);
        delay(150);
        setMotor(rightSpeed, M2_IN1, M2_IN2);
        wasStopped = false;
      } else {
        setMotor(leftSpeed, M1_IN1, M1_IN2);
        setMotor(rightSpeed, M2_IN1, M2_IN2);
      }
    } else {
      setMotor(0, M1_IN1, M1_IN2);
      setMotor(0, M2_IN1, M2_IN2);
      wasStopped = true;
    }

    static unsigned long lp = 0;
    if (millis() - lp > 200) {
      Serial.print("M1:"); Serial.print(crsf.getChannel(CH_MOSFET1));
      Serial.print(" M2:"); Serial.print(crsf.getChannel(CH_MOSFET2));
      Serial.print(" L:"); Serial.print(leftSpeed);
      Serial.print(" R:"); Serial.println(rightSpeed);
      lp = millis();
    }
  } else {
    setMotor(0, M1_IN1, M1_IN2);
    setMotor(0, M2_IN1, M2_IN2);
    digitalWrite(MOSFET1_PIN, LOW);
    digitalWrite(MOSFET2_PIN, LOW);
    wasStopped = true;
  }
}
