#include <Stepper.h>

const int stepsPerRevolution = 240;  // change this to fit the number of steps per revolution
                                     // for your motor

const int enableA = 6;
const int enableB = 5;

// initialize the stepper library using the default pins on the HBridge Shield:
Stepper myStepper(stepsPerRevolution, 4, 7, 3, 2);

void setup() {
  // set up the enable pins:
  pinMode(enableA, OUTPUT);
  pinMode(enableB, OUTPUT);
  // set the speed at 200 rpm:
  myStepper.setSpeed(200);
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {
  // step one revolution in one direction:
  Serial.println("clockwise");
  digitalWrite(enableA, HIGH);
  digitalWrite(enableB, HIGH);
  myStepper.step(stepsPerRevolution);
  digitalWrite(enableA, LOW);
  digitalWrite(enableB, LOW);
  delay(1000);
  
  // step one revolution in the other direction:
  Serial.println("counterclockwise");
  digitalWrite(enableA, HIGH);
  digitalWrite(enableB, HIGH);
  myStepper.step(-stepsPerRevolution);
  digitalWrite(enableA, LOW);
  digitalWrite(enableB, LOW);
  delay(1000); 
}
