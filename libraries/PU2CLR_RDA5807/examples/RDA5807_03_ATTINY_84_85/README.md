# ATTiny84 and ATTiny85 setup



## Attiny84 

![Schematic - Attiny84 with oled setup ](../../extras/images/circuit_attiny84.png)


### Attiny84 wireup

| RDA5807 pin     | Attiny84 REF pin | Physical pin  | 
| ----------------| -----------------| ------------- | 
| SEEK_UP         |     3            |    10         | 
| SEEK_DOWN       |     5            |     8         |
| ENCODER_PIN_A   |     0            |    13         |
| ENCODER_PIN_B   |     1            |    12         |  
| SDIO / SDA      |     SDA          |     7         |
| SCLK / CLK      |     SCL          |     9         |




## Attiny85 



![Schematic - Attiny85 with oled setup ](../../extras/images/circuit_attiny85.png)


### ATtiny85 and RDA5807 wireup  

| RDA5807 pin     | Attiny85 REF pin | Physical pin | 
| ----------------| -----------------| ------------- | 
| SEEK_UP         |     PB1          |     6         | 
| SEEK_DOWN       |     PB4          |     3         |
| AUDIO_MUTE      |     PB3          |     2         | 
| SDIO / SDA      |     SDA          |     5         |
| SCLK / CLK      |     SCL          |     7         |
   
<BR>

#### ATtiny85 pinout

![Schematic - Attiny85 pinout](../../extras/images/attiny85_pinout.jpg)


#### The example below implements an FM receiver with RDS and audio mute functions.

```cpp
#include <RDA5807.h>
#include <EEPROM.h> // The ATtiny85 contains 512 bytes of data EEPROM memory. The EEPROM has an endurance of at least 100,000 write/erase cycles.
#include <Tiny4kOLED.h>
#define SEEK_UP   PB1     
#define SEEK_DOWN PB4  
#define AUDIO_MUTE PB3  
#define VALID_DATA 85
char *stationName;
uint16_t currentFrequency;
RDA5807 rx;
void setup()
{
  pinMode(SEEK_UP, INPUT_PULLUP);
  pinMode(SEEK_DOWN, INPUT_PULLUP);
  pinMode(AUDIO_MUTE, INPUT_PULLUP);
  oled.begin();
  oled.clear();
  oled.on();
  oled.setFont(FONT8X16);
  rx.setup();
  rx.setVolume(8);  
  // Restores the latest frequency and audio mute statis saved into the EEPROM
  if (EEPROM.read(0) == VALID_DATA ) {
    currentFrequency = EEPROM.read(1) << 8;
    currentFrequency |= EEPROM.read(2);
    rx.setMute(EEPROM.read(3));
  } else {
    currentFrequency = 10390; // default value
  } 
  rx.setFrequency(currentFrequency); 
  rx.setRDS(true);
  rx.setRdsFifo(true);
  showStatus();
}
void showStatus() {
  oled.setCursor(0, 0);
  oled.print(F("FM"));
  oled.setCursor(38, 0);
  oled.clearToEOL();
  oled.setCursor(38, 0);
  oled.print(rx.formatCurrentFrequency()); 
  oled.setCursor(95, 0);
  oled.print(F("MHz"));
  oled.setCursor(0, 2);
  oled.clearToEOL();
}
void loop()
{
  uint8_t  bkey;
  bkey = ((digitalRead(SEEK_UP) << 2) | (digitalRead(SEEK_DOWN) << 1)) | digitalRead(AUDIO_MUTE); // 3, 5 or 6 (considering just one button pressed)  
  if ( bkey != 0b111) { // if none of them is pressed (not igual to 0b011, 0b101 or 0b110) then do nothing.
    if (bkey == 0b011) // 3 
      rx.seek(RDA_SEEK_WRAP,RDA_SEEK_UP, showStatus);
    else if ( bkey == 0b101) // 5
      rx.seek(RDA_SEEK_WRAP,RDA_SEEK_DOWN, showStatus);
    else // 6 
      rx.setMute(!rx.isMuted()); // inverts the audio mute status  
    showStatus();
    delay(200);
    // Saves the current frequency if it has changed. 
    currentFrequency = rx.getFrequency();
    EEPROM.update(0, VALID_DATA); // Says that a valid frequency will be saved  
    EEPROM.update(1, currentFrequency  >> 8);   // stores the current Frequency HIGH byte 
    EEPROM.update(2, currentFrequency & 0xFF);  // stores the current Frequency LOW byte 
    EEPROM.update(3, rx.isMuted());
  }
  if ( rx.getRdsReady() &&  rx.hasRdsInfo() && rx.getRdsFlagAB() == 0 )  {
    stationName = rx.getRdsText0A();
    oled.setCursor(0, 2);
    if ( stationName != NULL ) 
        oled.print(stationName); 
    else 
      oled.clearToEOL();
    delay(70);
  }
  delay(5);
}
```



### Installing ATtiny Core in Arduino IDE 

The ATtiny core board/plataform can be installed using the Arduino IDE boards manager. 
Inserts the URL http://drazzy.com/package_drazzy.com_index.json on board manager. To do that, go to Preferences, enter the above URL in "Additional Boards Manager URLs. To setup ATtiny85 on Arduino IDE, go to Tools Menu, Board, Board Manager and install "ATTinyCore by Spence Konde". 

Board setup: Select Chip = ATtiny85;  Clock Source = 4MHz (Internal); LTO = Enabled; millis() / macros() = Enabled; 

ATTENTION: if you select Clock source 8 MHz, for some reason, the system will work very slow. Maybe a bug. Not sure. 

See [ATtiny Core - 1634, x313, x4, x41, x5, x61, x7, x8 and 828 for Arduino](https://github.com/SpenceKonde/ATTinyCore).

See also [ATtiny85 pinout](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf).


# References 

* [ ATTinyCore](https://github.com/SpenceKonde/ATTinyCore)
* [How To Program ATtiny13/ATtiny13a using Arduino IDE](https://www.electronics-lab.com/project/how-to-program-attiny13attiny13a-using-arduino-ide/)
* [](https://github.com/SpenceKonde/ATTinyCore)





