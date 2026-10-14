#include <SPI.h>
#include <RH_RF95.h>


RH_RF95 rf95(10, 2);

void setup()
{

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("RF init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  rf95.setFrequency(868.0);
  rf95.setTxPower(22, false);
  // rf95.setModemConfig(RH_RF95::Bw125Cr46Sf9);
  rf95.spiWrite(RH_RF95_REG_1D_MODEM_CONFIG1, 0x72); // Bw125kHz, Cr4/6, Explicit Header
  rf95.spiWrite(RH_RF95_REG_1E_MODEM_CONFIG2, 0x94); // SF9, CRC off, RxTimeoutMsb=0

}

void loop()
{
  Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  uint8_t data[] = "Hello World!";
  rf95.send(data, sizeof(data));

  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(3000))
  {
    // Should be a reply message for us now
    if (rf95.recv(buf, &len))
    {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
  }
  delay(400);
}
