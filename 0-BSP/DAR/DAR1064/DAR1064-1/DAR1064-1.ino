#include <Arduino.h>

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 23;     // the number of the pushbutton pin
const int ledPin =  22;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int previousButtonState = 0; // track previous state to detect changes

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  // no need 
  //Serial.swap(3);
  //Serial.pins(9, 8);
  //Serial3.pins(PB0, PB1);


  // initialize Serial on the default USB/UART at 115200
  Serial3.begin(115200);

  // read initial state to detect changes in loop
  previousButtonState = digitalRead(buttonPin);
}

static void printButtonStateChange(int state) {
  if (state == HIGH) {
    Serial3.println("Button pressed");
  } else {
    Serial3.println("Button released");
  }
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // print when the button state changes
  if (buttonState != previousButtonState) {
    printButtonStateChange(buttonState);
    previousButtonState = buttonState;
    // small debounce delay
    delay(50);
  }

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    delay(100);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
}
