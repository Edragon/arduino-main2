// nrf24_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_NRF24 class. RH_NRF24 class does not provide for addressing or
// reliability, so you should only use RH_NRF24 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example nrf24_server.
// Tested on Uno with Sparkfun NRF25L01 module
// Tested on Anarduino Mini (http://www.anarduino.com/mini/) with RFM73 module
// Tested on Arduino Mega with Sparkfun WRL-00691 NRF25L01 module

#include <SPI.h>
#include <RH_NRF24.h>

#define RF_CS_PIN 7
#define RF_CE_PIN 9
#define RF_INDICATOR 3
#define VOLTAGE_PIN A1

// Singleton instance of the radio driver
RH_NRF24 nrf24(RF_CE_PIN, RF_CS_PIN);

void setup() 
{
  pinMode(RF_INDICATOR, OUTPUT);
  digitalWrite(RF_INDICATOR, LOW);
  pinMode(VOLTAGE_PIN, INPUT);
  Serial.begin(9600);
  while (!Serial) 
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    
  Serial.println("init OK\r\n");     
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

  // Send a message to nrf24_server
  digitalWrite(RF_INDICATOR, HIGH);
  nrf24.send((uint8_t*)data, strlen(data) + 1);
  
  nrf24.waitPacketSent();
  digitalWrite(RF_INDICATOR, LOW);

  // Now wait for a reply
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (nrf24.waitAvailableTimeout(500))
  { 
    digitalWrite(RF_INDICATOR, HIGH);
    // Should be a reply message for us now   
    if (nrf24.recv(buf, &len))
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
    Serial.println("No reply, is nrf24_server running?");
  }
  delay(400);
}
