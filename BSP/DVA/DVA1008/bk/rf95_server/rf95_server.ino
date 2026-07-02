// rf95_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_client
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with
// the RFM95W, Adafruit Feather M0 with RFM95

#include <SPI.h>
#include <RH_RF95.h>

// Singleton instance of the radio driver
RH_RF95 rf95(10, 2);
// RH_RF95 rf95(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
// RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95

// Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)
// #define Serial SerialUSB

void setup()
{
  // Rocket Scream Mini Ultra Pro with the RFM95W only:
  // Ensure serial flash is not interfering with radio communication on SPI bus

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

  Serial.begin(9600);
  while (!Serial)
    ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  rf95.setFrequency(868.0);

  // rf95.setTxPower(13, false);

  rf95.setModemConfig(RH_RF95::Bw125Cr46Sf9);
  //rf95.setSignalBandwidth(RH_RF95_BW_125KHZ);
  //rf95.setCodingRate4(RH_RF95_CODING_RATE_4_6);
  //rf95.setSpreadingFactor(RH_RF95_SPREADING_FACTOR_512CPS);


  //rf95.setModemConfig(Bw125Cr46Sf9); // Radio default
  //    setModemConfig(Bw125Cr48Sf4096); // slow and reliable?
  // rf95.spiWrite(RH_RF95_REG_1D_MODEM_CONFIG1, 0x72); // Bw125kHz, Cr4/6, Explicit Header
  // rf95.spiWrite(RH_RF95_REG_1E_MODEM_CONFIG2, 0x94); // SF9, CRC off, RxTimeoutMsb=0

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  //  driver.setTxPower(23, false);
  // If you are using Modtronix inAir4 or inAir9,or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for -1 to 14 dBm and with useRFO true.
  // Failure to do that will result in extremely low transmit powers.
  //  driver.setTxPower(14, true);
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      digitalWrite(7, HIGH);
      //      RH_RF95::printBuffer("request: ", buf, len);
      Serial.print("got request: ");
      Serial.println((char *)buf);
      //      Serial.print("RSSI: ");
      //      Serial.println(rf95.lastRssi(), DEC);

      // Send a reply
      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      digitalWrite(7, LOW);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}
