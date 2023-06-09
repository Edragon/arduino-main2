///
/// \file RDSParser.cpp
/// \brief RDS Parser class implementation.
///
/// \author Matthias Hertel, http://www.mathertel.de
/// \copyright Copyright (c) 2014 by Matthias Hertel.\n
/// This work is licensed under a BSD style license.\n
/// See http://www.mathertel.de/License.aspx
///
/// \details
///
/// More documentation and source code is available at http://www.mathertel.de/Arduino
///
/// ChangeLog see RDSParser.h.

#include "RDSParser.h"

#define DEBUG_FUNC0(fn) \
  { \
    Serial.print(fn); \
    Serial.println("()"); \
  }

/// Setup the RDS object and initialize private variables to 0.
RDSParser::RDSParser() {
  memset(this, 0, sizeof(RDSParser));
}  // RDSParser()


void RDSParser::init() {
  strcpy(_PSName1, "11111111");
  strcpy(_PSName2, "22222222");
  strcpy(_PSName3, "33333333");
  strcpy(programServiceName, "        ");
  strcpy(lastServiceName, "        ");
  memset(_RDSText, 0, sizeof(_RDSText));
  _lastTextIDX = 0;
}  // init()


void RDSParser::attachServiceNameCallback(receiveServiceNameFunction newFunction) {
  _sendServiceName = newFunction;
}  // attachServiceNameCallback

void RDSParser::attachTextCallback(receiveTextFunction newFunction) {
  _sendText = newFunction;
}  // attachTextCallback


void RDSParser::attachTimeCallback(receiveTimeFunction newFunction) {
  _sendTime = newFunction;
}  // attachTimeCallback


void RDSParser::processData(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4) {
  // DEBUG_FUNC0("process");
  uint8_t idx;  // index of rdsText
  char c1, c2;

  uint16_t mins;  ///< RDS time in minutes
  uint8_t off;    ///< RDS time offset and sign

  // Serial.print('('); Serial.print(block1, HEX); Serial.print(' '); Serial.print(block2, HEX); Serial.print(' '); Serial.print(block3, HEX); Serial.print(' '); Serial.println(block4, HEX);

  if (block1 == 0) {
    // reset all the RDS info.
    init();
    // Send out empty data
    if (_sendServiceName) _sendServiceName(programServiceName);
    if (_sendText) _sendText("");
    return;
  }  // if

  // analyzing Block 2
  rdsGroupType = 0x0A | ((block2 & 0xF000) >> 8) | ((block2 & 0x0800) >> 11);
  rdsTP = (block2 & 0x0400);
  rdsPTY = (block2 & 0x0400);

  switch (rdsGroupType) {
    case 0x0A:
    case 0x0B:
      // The data received is part of the Service Station Name
      idx = 2 * (block2 & 0x0003); // idx = 0, 2, 4, 6

      // new data is 2 chars from block 4
      c1 = block4 >> 8;
      c2 = block4 & 0x00FF;

      // Serial.printf(">%d %c%c %02x %02x\n", idx, c1, c2, c1, c2);

      // shift new data into _PSNameN
      _PSName3[idx] = _PSName2[idx];
      _PSName2[idx] = _PSName1[idx];
      _PSName1[idx] = c1;

      _PSName3[idx+1] = _PSName2[idx+1];
      _PSName2[idx+1] = _PSName1[idx+1];
      _PSName1[idx+1] = c2;

      // check that the data was received successfully twice
      // before publishing the station name
      if (idx == 6) {
        bool isGood = true;
        // create programServiceName with 2 of 3
        for (int n= 0; n < 8; n++) {
          if ((_PSName1[n] == _PSName2[n]) || (_PSName1[n] == _PSName3[n])) {
            programServiceName[n] = _PSName1[n];

          } else if (_PSName2[n] == _PSName3[n]) {
            programServiceName[n] = _PSName2[n];

          } else {
            isGood = false;
          }
        }
        if ((isGood) && (strcmp(lastServiceName, programServiceName) != 0)) {
          strcpy(lastServiceName, programServiceName);
          if (_sendServiceName)
            _sendServiceName(programServiceName);
        }
      } // if
      break;

    case 0x2A:
      // The data received is part of the RDS Text.
      _textAB = (block2 & 0x0010);
      idx = 4 * (block2 & 0x000F);

      if (idx < _lastTextIDX) {
        // the existing text might be complete because the index is starting at the beginning again.
        // now send it to the possible listener.
        if (_sendText)
          _sendText(_RDSText);
      }
      _lastTextIDX = idx;

      if (_textAB != _last_textAB) {
        // when this bit is toggled the whole buffer should be cleared.
        _last_textAB = _textAB;
        memset(_RDSText, 0, sizeof(_RDSText));
        // Serial.println("T>CLEAR");
      }  // if


      // new data is 2 chars from block 3
      _RDSText[idx] = (block3 >> 8);
      idx++;
      _RDSText[idx] = (block3 & 0x00FF);
      idx++;

      // new data is 2 chars from block 4
      _RDSText[idx] = (block4 >> 8);
      idx++;
      _RDSText[idx] = (block4 & 0x00FF);
      idx++;

      // Serial.print(' '); Serial.println(_RDSText);
      // Serial.print("T>"); Serial.println(_RDSText);
      break;

    case 0x4A:
      // Clock time and date
      off = (block4)&0x3F;          // 6 bits
      mins = (block4 >> 6) & 0x3F;  // 6 bits
      mins += 60 * (((block3 & 0x0001) << 4) | ((block4 >> 12) & 0x0F));

      // adjust offset
      if (off & 0x20) {
        mins -= 30 * (off & 0x1F);
      } else {
        mins += 30 * (off & 0x1F);
      }

      if ((_sendTime) && (mins != _lastRDSMinutes)) {
        _lastRDSMinutes = mins;
        _sendTime(mins / 60, mins % 60);
      }  // if
      break;

    case 0x6A:
      // IH
      break;

    case 0x8A:
      // TMC
      break;

    case 0xAA:
      // TMC
      break;

    case 0xCA:
      // TMC
      break;

    case 0xEA:
      // IH
      break;

    default:
      // Serial.print("RDS_GRP:"); Serial.println(rdsGroupType, HEX);
      break;
  }
}  // processData()

// End.