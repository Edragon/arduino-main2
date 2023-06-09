///
/// \file SI4703.h
/// \brief Library header file for the radio library to control the SI4703 radio chip.
///
/// \author Matthias Hertel, http://www.mathertel.de
/// \copyright Copyright (c) 2014-2015 by Matthias Hertel.\n
/// This work is licensed under a BSD style license.\n
/// See http://www.mathertel.de/License.aspx
///
/// This library enables the use of the Radio Chip SI4703.
///
/// More documentation is available at http://www.mathertel.de/Arduino
/// Source Code is available on https://github.com/mathertel/Radio
///
/// History:
/// --------
/// * 05.08.2014 created.
/// * 05.02.2023 clearing RDS data after frequency changes and scan.

#ifndef SI4703_h
#define SI4703_h

#include <Arduino.h>
#include <Wire.h>

#include <radio.h>

// ----- library definition -----

// writeGPIO parameters
// GPIO1-3 Pins
static const uint8_t GPIO1 = 0;  // GPIO1
static const uint8_t GPIO2 = 2;  // GPIO2
static const uint8_t GPIO3 = 4;  // GPIO3

// GPIO1-3 Possible Values
static const uint8_t GPIO_Z = 0b00;     // High impedance (default)
static const uint8_t GPIO_I = 0b01;     // GPIO1-Reserved, GPIO2-STC/RDS int, or GPIO3-Mono/Sterio Indicator
static const uint8_t GPIO_Low = 0b10;   // Low output (GND level)
static const uint8_t GPIO_High = 0b11;  // High output (VIO level)

/// Library to control the SI4703 radio chip.
class SI4703 : public RADIO {
public:
  SI4703();

  void setup(int feature, int value) override;
  bool init() override;  // initialize library and the chip.
  void term() override;  // terminate all radio functions.

  // Control of the audio features

  // Control the volume output of the radio chip
  void setVolume(int8_t newVolume) override;  ///< Control the volume output of the radio chip in the range 0..15.

  // Control mono/stereo mode of the radio chip
  void setMono(bool switchOn) override;  // Switch to mono mode.

  // Control the mute function of the radio chip
  void setMute(bool switchOn) override;  // Switch to mute mode.

  // Control the softMute function of the radio chip
  void setSoftMute(bool switchOn) override;  // Switch to soft mute mode.

  void writeGPIO(int GPIO, int val);

  // Control of the core receiver

  // Control the frequency
  void setBand(RADIO_BAND newBand) override;

  void setFrequency(RADIO_FREQ newF);
  RADIO_FREQ getFrequency(void);

  void seekUp(bool toNextSender = true);    // start seek mode upwards
  void seekDown(bool toNextSender = true);  // start seek mode downwards

  void checkRDS();  // read RDS data from the current station and process when data available.

  // ----- combined status functions -----

  virtual void getRadioInfo(RADIO_INFO *info);  ///< Retrieve some information about the current radio function of the chip.

  virtual void getAudioInfo(AUDIO_INFO *info);  ///< Retrieve some information about the current audio function of the chip.

  // ----- debug Helpers send information to Serial port

  void debugScan();    // Scan all frequencies and report a status
  void debugStatus();  // Report Info about actual Station

  // ----- read/write registers of the chip

  void _readRegisters();  // read all status & data registers
  void _readRegister0A();  // read just status 0x0A register
  void _saveRegisters();  // Save writable registers back to the chip

private:
  // ----- local variables
  int _sdaPin = -1;

  // store the current values of the 16 chip internal 16-bit registers
  uint16_t registers[16];

  // last RDS Poll to prevent polling < 40 
  unsigned long _lastRDSPoll = 0;

  // ----- low level communication to the chip using I2C bus

  void _seek(bool seekUp = true);
  void _waitEnd();
};

#endif
