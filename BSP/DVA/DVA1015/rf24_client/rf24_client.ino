// rf24_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF24 class. RH_RF24 class does not provide for addressing or
// reliability, so you should only use RH_RF24 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf24_server.
// Tested on Anarduino Mini http://www.anarduino.com/mini/ with RFM24W and RFM26W

#include <SPI.h>
#include <RH_RF24.h>

#define RF_CS_PIN 7
#define RF_IRQ_PIN 2
#define RF_CE_PIN 9
#define RF_INDICATOR 3
#define VOLTAGE_PIN A1

// Singleton instance of the radio driver
RH_RF24 rf24(RF_CS_PIN, RF_IRQ_PIN, RF_CE_PIN);

void setup() 
{
  pinMode(RF_INDICATOR, OUTPUT);
  digitalWrite(RF_INDICATOR, LOW);
  pinMode(VOLTAGE_PIN, INPUT);
  Serial.begin(9600);
  if (!rf24.init())
    Serial.println("init failed");
  // The default radio config is for 30MHz Xtal, 434MHz base freq 2GFSK 5kbps 10kHz deviation
  // power setting 0x10
  // If you want a different frequency mand or modulation scheme, you must generate a new
  // radio config file as per the RH_RF24 module documentation and recompile
  // You can change a few other things programatically:
  //rf24.setFrequency(435.0); // Only within the same frequency band
  //rf24.setTxPower(0x7f);
}


void loop()
{
  int sensorValue = analogRead(VOLTAGE_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  
  char data[32];
  dtostrf(voltage, 4, 2, data);
  strcat(data, "V");

  Serial.print("Sending voltage: ");
  Serial.println(data);

  // Send a message to rf24_server
  digitalWrite(RF_INDICATOR, HIGH);
  rf24.send((uint8_t*)data, strlen(data) + 1);
  
  rf24.waitPacketSent();
  digitalWrite(RF_INDICATOR, LOW);

  // Now wait for a reply
  uint8_t buf[RH_RF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf24.waitAvailableTimeout(500))
  { 
    digitalWrite(RF_INDICATOR, HIGH);
    // Should be a reply message for us now   
    if (rf24.recv(buf, &len))
    {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("recv failed");
    }
    digitalWrite(RF_INDICATOR, LOW);
  }
  else
  {
    Serial.println("No reply, is rf24_server running?");
  }
  delay(400);
}

