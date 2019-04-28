/***************************************************************************************************/
/*
   This is an Arduino library for the ROHM BH1750FVI Ambient Light Sensor

   Power supply voltage: 2.4 - 3.6v
   Default range:        1 - 65'535 lux
   Measurement accuracy: ±20%, possible to calibrate
   Peak wave length:     560nm, yellow-green

   written by : enjoyneering79
   sourse code: https://github.com/enjoyneering/


   This chip uses I2C bus to communicate, specials pins are required to interface
   Board:                                    SDA                    SCL                    Level
   Uno, Mini, Pro, ATmega168, ATmega328..... A4                     A5                     5v
   Mega2560................................. 20                     21                     5v
   Due, SAM3X8E............................. 20                     21                     3.3v
   Leonardo, Micro, ATmega32U4.............. 2                      3                      5v
   Digistump, Trinket, ATtiny85............. 0/physical pin no.5    2/physical pin no.7    5v
   Blue Pill, STM32F103xxxx boards.......... PB7                    PB6                    3.3v/5v
   ESP8266 ESP-01........................... GPIO0/D5               GPIO2/D3               3.3v/5v
   NodeMCU 1.0, WeMos D1 Mini............... GPIO4/D2               GPIO5/D1               3.3v/5v
   ESP32.................................... GPIO21/D21             GPIO22/D22             3.3v

   Frameworks & Libraries:
   ATtiny Core           - https://github.com/SpenceKonde/ATTinyCore
   ESP32 Core            - https://github.com/espressif/arduino-esp32
   ESP8266 Core          - https://github.com/esp8266/Arduino
   ESP8266 I2C lib fixed - https://github.com/enjoyneering/ESP8266-I2C-Driver
   STM32 Core            - https://github.com/rogerclarkmelbourne/Arduino_STM32

   GNU GPL license, all text above must be included in any redistribution,
   see link for details  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
  
#ifndef BH1750FVI_h
#define BH1750FVI_h

#if defined(ARDUINO) && ((ARDUINO) >= 100) //arduino core v1.0 or later
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#if defined(__AVR__)
#include <avr/pgmspace.h>                  //use for PROGMEM Arduino AVR
#elif defined(ESP8266)
#include <pgmspace.h>                      //use for PROGMEM Arduino ESP8266
#elif defined(_VARIANT_ARDUINO_STM32_)
#include <avr/pgmspace.h>                  //use for PROGMEM Arduino STM32
#endif

#include <Wire.h>


#define BH1750_POWER_DOWN           0x00  //low power state
#define BH1750_POWER_ON             0x01  //wating for measurment command
#define BH1750_RESET                0x07  //soft reset

#define BH1750_MEASUREMENT_TIME_H   0x40  //changing measurement time register MSB bits
#define BH1750_MEASUREMENT_TIME_L   0x60  //changing measurement time register LSB bits

#define BH1750_MTREG_DEFAULT        0x45  //default integration/measurement time, 69
#define BH1750_MTREG_MIN            0x1F  //minimun integration/measurement time, 31
#define BH1750_MTREG_MAX            0xFE  //maximum integration/measurement time, 254
#define BH1750_SENSITIVITY_MIN      0.45  //minimun sensitivity
#define BH1750_SENSITIVITY_MAX      3.68  //maximum sensitivity
#define BH1750_SENSITIVITY_DEFAULT  1.00  //default sensitivity

#define BH1750_ACCURACY_DEFAULT     1.2   //typical measurement accuracy, sensor calibration
#define BH1750_ERROR                0x00  //returns 0, if communication error is occurred


typedef enum : uint8_t
{
BH1750_DEFAULT_I2CADDR = 0x23,            //device i2c address if address pin LOW
BH1750_SECOND_I2CADDR  = 0x5C             //device i2c address if address pin HIGH
}
BH1750FVI_ADDRESS;

typedef enum : uint8_t
{
BH1750_CONTINUOUS_HIGH_RES_MODE   = 0x10, //continuous measurement, 1.0 lx resolution
BH1750_CONTINUOUS_HIGH_RES_MODE_2 = 0x11, //continuous measurement, 0.5 lx resolution
BH1750_CONTINUOUS_LOW_RES_MODE    = 0x13, //continuous measurement, 4.0 lx resolution

BH1750_ONE_TIME_HIGH_RES_MODE     = 0x20, //one measurement & power down, 1.0 lx resolution
BH1750_ONE_TIME_HIGH_RES_MODE_2   = 0x21, //one measurement & power down, 0.5 lx resolution
BH1750_ONE_TIME_LOW_RES_MODE      = 0x23  //one measurement & power down, 4.0 lx resolution
}   
BH1750FVI_RESOLUTION;

class BH1750FVI 
{
 public:

  BH1750FVI(BH1750FVI_ADDRESS = BH1750_DEFAULT_I2CADDR, BH1750FVI_RESOLUTION = BH1750_ONE_TIME_HIGH_RES_MODE_2, float sensitivity = BH1750_SENSITIVITY_DEFAULT, float calibration = BH1750_ACCURACY_DEFAULT);

  #if defined(ESP8266)
  bool  begin(uint8_t sda = SDA, uint8_t scl = SCL);
  #else
  bool  begin(void);
  #endif
  void  setResolution(BH1750FVI_RESOLUTION res);
  bool  setSensitivity(float sensitivity);
  float getSensitivity(void);
  float readLightLevel(void);
  void  powerDown(void);
  void  powerOn(void);
  void  reset(void);
  void  setCalibration(float value);
  float getCalibration(void);

 private:
  float _sensitivity;
  float _accuracy;

  BH1750FVI_RESOLUTION _sensorResolution;
  BH1750FVI_ADDRESS    _sensorAddress;

  bool write8(uint8_t data);
};

#endif
