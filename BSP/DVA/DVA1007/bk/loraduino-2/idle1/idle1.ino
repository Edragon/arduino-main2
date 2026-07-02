// **** INCLUDES *****
#include "LowPower.h"
#include "SPIFlash.h" //get it here: https://github.com/LowPowerLab/SPIFlash

#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>

SPIFlash flash(8, 0xEF40); // 0xEF30 try both


#include <SPIFlash.h>    //get it here: https://github.com/LowPowerLab/SPIFlash


#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

RH_RF95 driver(10, 2);
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void setup()
{
  Serial.begin(115200);

  //  if (flash.initialize()) {
  //    Serial.println("FLASH Init OK!");
  //  } else {
  //    Serial.println("FLASH Init FAIL!");
  //  }
  //  delay(500);

  if (!manager.init()) {
    Serial.println("RF init failed");
  } else {
    Serial.println("RF init OK");
  }

  driver.setFrequency(434.0);
  delay(500);
}




uint8_t data[] = "Hello World!";
// Dont put this on the stack:
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{

  Serial.println("Sending to rf95_reliable_datagram_server");

  // Send a message to manager_server
  if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
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
  delay(10000);


  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  
  //LowPower.powerDown(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
  //                   SPI_OFF, USART0_OFF, TWI_OFF);

  //LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
  //              SPI_OFF, USART0_OFF, TWI_OFF);

}
