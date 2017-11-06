/***************************************************************************************************/
/*
  This is an Arduino library for the ROHM BH1750FVI Ambient Light Sensor

  written by : enjoyneering79
  sourse code: https://github.com/enjoyneering/

  Wide range:           1 - 65'535 lx (by default)
  Possible to detect:   0.11 - 100'000 lx by changing Sensitivity

  This sensor uses I2C bus to communicate, specials pins are required to interface

  Connect chip to pins:    SDA        SCL
  Uno, Mini, Pro:          A4         A5
  Mega2560, Due:           20         21
  Leonardo:                2          3
  ATtiny85:                0(5)       2/A1(7)   (ATTinyCore  - https://github.com/SpenceKonde/ATTinyCore
                                                 & TinyWireM - https://github.com/SpenceKonde/TinyWireM)
  ESP8266 ESP-01:          GPIO0/D5   GPIO2/D3  (ESP8266Core - https://github.com/esp8266/Arduino)
  NodeMCU 1.0:             GPIO4/D2   GPIO5/D1
  WeMos D1 Mini:           GPIO4/D2   GPIO5/D1

  BSD license, all text above must be included in any redistribution
*/
/***************************************************************************************************/
  
#ifndef BH1750FVI_h
#define BH1750FVI_h

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#if defined(__AVR_ATtinyX4__) || defined(__AVR_ATtinyX5__) || defined(__AVR_ATtinyX313__)
#include <TinyWireM.h>
#define  Wire TinyWireM
#else
#include <Wire.h>
#endif

#if defined(__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif


#define BH1750_POWER_DOWN           0x00  //no active, low power state
#define BH1750_POWER_ON             0x01  //wating for measurment command
#define BH1750_RESET                0x07  //reset only illuminance data register, not accepted in POWER_DOWN mode

#define BH1750_MEASUREMENT_TIME_H   0x40  //high bit of changing measurement time
#define BH1750_MEASUREMENT_TIME_L   0x60  //low  bit of changing measurement time

#define BH1750_MTREG_DEFAULT        0x45  //default integration/measurement time = 69
#define BH1750_MTREG_MIN            0x1F  //min.    integration/measurement time = 31
#define BH1750_MTREG_MAX            0xFE  //max.    integration/measurement time = 254
#define BH1750_SENSITIVITY_MIN      0.45  //min.    sensitivity
#define BH1750_SENSITIVITY_MAX      3.68  //max.    sensitivity
#define BH1750_SENSITIVITY_DEFAULT  1.00  //default sensitivity

#define BH1750_MEASUREMENT_ACCURACY 1.2   //measurement accuracy, for result adjustment/calibration (min - 0.96, max - 1.44, typ - 1.2)
#define BH1750_POLL_LIMIT           8     //i2c retry limit
#define BH1750_ERROR                0x00  //returns 0, if communication error is occurred


typedef enum
{
BH1750_DEFAULT_I2CADDR = 0x23,            //device Address on i2c serial bus when address pin LOW
BH1750_SECOND_I2CADDR  = 0x5C             //device Address on i2c serial bus when address pin HIGH
}
BH1750FVI_Address;

typedef enum
{
BH1750_CONTINUOUS_HIGH_RES_MODE   = 0x10, //continuous measurement @ 1.0 lx resolution. Integration time is 120..180ms
BH1750_CONTINUOUS_HIGH_RES_MODE_2 = 0x11, //continuous measurement @ 0.5 lx resolution. Integration time is 120..180ms
BH1750_CONTINUOUS_LOW_RES_MODE    = 0x13, //Continuous measurement @ 4.0 lx resolution. Integration time is 16...24ms

BH1750_ONE_TIME_HIGH_RES_MODE     = 0x20, //one-time measurement with power down @ 1.0 lx resolution. Integration time is 120..180ms
BH1750_ONE_TIME_HIGH_RES_MODE_2   = 0x21, //one-time measurement with power down @ 0.5 lx resolution. Integration time is 120..180ms
BH1750_ONE_TIME_LOW_RES_MODE      = 0x23  //one-time measurement with power down @ 4.0 lx resolution. Integration time is 16...24ms
}   
BH1750FVI_Resolution;

class BH1750FVI 
{
 public:

  BH1750FVI(BH1750FVI_Address = BH1750_DEFAULT_I2CADDR, BH1750FVI_Resolution = BH1750_CONTINUOUS_HIGH_RES_MODE_2, float sensitivity = BH1750_SENSITIVITY_DEFAULT);

  #if defined(ESP8266)
  bool  begin(uint8_t sda = SDA, uint8_t scl = SCL);
  #else
  bool  begin(void);
  #endif
  void 	setResolution(BH1750FVI_Resolution res);
  void 	setSensitivity(float sensitivity);
  float getSensitivity(void);
  float readLightLevel(void);
  void 	powerDown(void);
  void  reset(void);

 private:
  uint8_t _currentMTreg;
  bool    _powerON;
  float   _sensitivity;

  BH1750FVI_Resolution _BH1750FVI_Resolution;
  BH1750FVI_Address    _BH1750FVI_Address;

  void powerOn(void);
  void write8(uint8_t data);
};

#endif
