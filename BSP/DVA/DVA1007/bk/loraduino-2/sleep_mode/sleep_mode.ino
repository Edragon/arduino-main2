
#include <RHReliableDatagram.h>
#include <RH_RF95.h>

#include "LowPower.h"

#include <SPIFlash.h>    //get it here: https://github.com/LowPowerLab/SPIFlash
#include <SPI.h>

SPIFlash flash(8, 0xEF40); // 0xEF30 try both, either works

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2
RH_RF95 driver(10, 2);
RHReliableDatagram manager(driver, CLIENT_ADDRESS);


char volt_char[5];
char text_volt[] = "this board battery level is: ";
char data_volt[80];
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];


void volt_read() {
  float volt = analogRead(A7);
  volt = volt / 217 ;            // calculate battery, linear
  dtostrf(volt, 6, 2, volt_char);         // data to string

  sprintf(data_volt, "%s %s", text_volt, volt_char);   // combine char array

  Serial.println(data_volt);
  delay(100);        // delay in between reads for stability

}

void send() {

  volt_read();
  Serial.println("Sending to rf95_reliable_datagram_server");

  // Send a message to manager_server
  if (manager.sendtoWait(data_volt, sizeof(data_volt), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    { digitalWrite(7, LOW);
      delay(500);
      digitalWrite(7, HIGH);

      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("No reply, is rf95_reliable_datagram_server running?");
    }
  }
  else
    Serial.println("sendtoWait failed");
}

void setup()
{
  pinMode(7, OUTPUT);
  Serial.begin(9600);

  // initialize Flash
  while (!Serial) ; // Wait for serial port to be available

  if (!manager.init()) {
    Serial.println("RF init failed");
  }
  else {
    Serial.println("RF init OK");
  }

  driver.setFrequency(434.0);
  delay(500);

  if (flash.initialize())
  {
    Serial.println("Flash Init OK!");
  }
  else {
    Serial.println("Flash Init FAIL!");
  }

  delay(500);
}


void loop() {
  digitalWrite(7, HIGH);
  delay(500);
  send();
  digitalWrite(7, LOW);
  delay(1000);

  driver.sleep();
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}



//uint8_t data_volt[] = "Hello World!";
//// Dont put this on the stack:
//uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
