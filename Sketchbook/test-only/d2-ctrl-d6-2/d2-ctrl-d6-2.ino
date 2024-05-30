/*
  DigitalReadSerial

  Reads a digital input on pin 2, prints the result to the Serial Monitor

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/DigitalReadSerial
*/

// digital pin 2 has a pushbutton attached to it. Give it a name:
int pushButton = 2;
int relaypin = 6;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(19200);
  
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);
  pinMode(relaypin, OUTPUT);
  digitalWrite(relaypin, LOW);
}

// the loop routine runs over and over again forever:
void loop() {

  // read the input pin:
  int buttonState = digitalRead(pushButton);

  if  ( buttonState == 0) {
    Serial.println(buttonState);
    digitalWrite(relaypin, HIGH);

  } else {
    Serial.println(buttonState);
    digitalWrite(relaypin, LOW);
  }
  // print out the state of the button:

  delay(1);        // delay in between reads for stability
}
