// read out hex data
// baudrate both must be 4800

// output data like: 

// FA 5A 02 FB 48 FE FF 01 00 40 74 00 00 00 52 1B 88 F9 93 72 01 00 00 EB


#include <SoftwareSerial.h>
#define BAUD_RATE 4800
SoftwareSerial mySerial(10, 11); // RX, TX

void setup() {
  // must also be 4800
  
  Serial.begin(BAUD_RATE);
  mySerial.begin(BAUD_RATE);

}

void loop() {
  while (mySerial.available() > 0) {
    Serial.write(mySerial.read());
    yield();
  }
  while (Serial.available() > 0) {
    mySerial.write(Serial.read());
    yield();
  }

}
