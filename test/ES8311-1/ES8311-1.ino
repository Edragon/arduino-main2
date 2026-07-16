#include "AudioTools.h"
#include "AudioBoard.h"

// 1. Define your pins clearly
#define I2S_BCLK 5 // SCK
#define I2S_LRCK 4 // ws 
#define I2S_DOUT 6 // do
#define I2S_MCLK 2 // MCLK

#define I2C_SDA 18
#define I2C_SCL 17

using namespace audio_driver;

// 2. Use the correct class: DriverPins
DriverPins my_pins;

// 3. Initialize the board with the driver and pins
// AudioDriverES8311 is a static instance provided by the library
AudioBoard board(AudioDriverES8311, my_pins);

void setup() {
  Serial.begin(115200);

  // Define I2S Pins
  my_pins.addI2S(PinFunction::CODEC, I2S_MCLK, I2S_BCLK, I2S_LRCK, I2S_DOUT);

  // Initialize Wire and add to pins
  Wire.begin(I2C_SDA, I2C_SCL);
  my_pins.addI2C(PinFunction::CODEC, Wire);

  // Start the board
  if (!board.begin()) {
    Serial.println("Failed to initialize ES8311 Board!");
    while (true)
      ;
  }
  Serial.println("ES8311 Initialized Successfully.");
}

void loop() {
  delay(1000);
}
