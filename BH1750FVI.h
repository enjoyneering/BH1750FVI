/**************************************************************************/
/*
This is an Arduino library for the ROHM BH1750FVI Ambient Light Sensor

  written by enjoyneering79

  Wide range:           1 - 65'535 lx (by default)
  Possible to detect:   0.11 - 100'000 lx by changing Sensitivity

  These sensor uses I2C  to communicate. Two pins are required to  
  interface.

  Connect BH1750FVI to pins:  SDA     SCL
  Uno, Mini, Pro:             A4      A5
  Mega2560, Due:              20      21
  Leonardo:                   2       3
  Atiny85:                    0/PWM   2/A1   (TinyWireM)
  NodeMCU 1.0:                D1/ANY  D2/ANY (D1 & D2 by default)
  ESP8266 ESP-01:             ANY     ANY

  BSD license, all text above must be included in any redistribution
*/
 /**************************************************************************/
  
#ifndef BH1750FVI_h
#define BH1750FVI_h

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#if defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny26__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny45__) || (__AVR_ATtiny84__) || defined(__AVR_ATtiny85__)
 #include <TinyWireM.h>
 #define Wire TinyWireM
#elif defined (ARDUINO_ARCH_ESP8266) || (ESP8266_NODEMCU)
 #include <Wire.h>
#else defined
 #include <avr/pgmspace.h>
 #include <Wire.h>
#endif



#define BH1750_POWER_DOWN           0x00  /* No active state */
#define BH1750_POWER_ON             0x01  /* Wating for measurment command */
#define BH1750_RESET                0x07  /* Reset only illuminance data register. Not accepted in POWER_DOWN mode */

#define BH1750_MEASUREMENT_TIME_H   0x40  /* High Bit of changing Measurement Time. */
#define BH1750_MEASUREMENT_TIME_L   0x60  /* Low  Bit of changing Measurement Time. */

#define BH1750_MTREG_DEFAULT        0x45  /* Default Integration/Measurement time = 69  */
#define BH1750_MTREG_MIN            0x1F  /* Min.    Integration/Measurement time = 31  */
#define BH1750_MTREG_MAX            0xFE  /* Max.    Integration/Measurement time = 254 */

#define BH1750_MEASUREMENT_ACCURACY 1.2   /* Measurement Accuracy. Used for result adjustment/calibration (min - 0.96, max - 1.44, typ - 1.2) */


typedef enum
{
BH1750_DEFAULT_I2CADDR = 0x23,  /* Device Address on i2c serial bus when address pin LOW */
BH1750_SECOND_I2CADDR  = 0x5C   /* Device Address on i2c serial bus when address pin HIGH */
}
BH1750FVI_Address;

typedef enum
{
BH1750_CONTINUOUS_HIGH_RES_MODE   = 0x10,  /* Continuous measurement. 1.0 lx resolution. Integration time is 120..180ms. */
BH1750_CONTINUOUS_HIGH_RES_MODE_2 = 0x11,  /* Continuous measurement. 0.5 lx resolution. Integration time is 120..180ms. */
BH1750_CONTINUOUS_LOW_RES_MODE    = 0x13,  /* Continuous measurement. 4.0 lx resolution. Integration time is 16...24ms.  */

BH1750_ONE_TIME_HIGH_RES_MODE     = 0x20,  /* One-time measurement (Power down after measurement). 1.0 lx resolution. Integration time is 120..180ms. */
BH1750_ONE_TIME_HIGH_RES_MODE_2   = 0x21,  /* One-time measurement (Power down after measurement). 0.5 lx resolution. Integration time is 120..180ms. */
BH1750_ONE_TIME_LOW_RES_MODE      = 0x23   /* One-time measurement (Power down after measurement). 4.0 lx resolution. Integration time is 16...24ms.  */
}   
BH1750FVI_Resolution;

class BH1750FVI 
{
 public:

  BH1750FVI(BH1750FVI_Address = BH1750_DEFAULT_I2CADDR, BH1750FVI_Resolution = BH1750_CONTINUOUS_HIGH_RES_MODE_2, float BH1750FVI_Sensitivity = 1.00);

  #if defined (ARDUINO_ARCH_ESP8266) || (ESP8266_NODEMCU)
    bool   begin(uint8_t sda, uint8_t scl);
  #else
    bool   begin(void);
  #endif
  void 	setSensitivity(float BH1750FVI_Sensitivity);
  void 	setResolution(BH1750FVI_Resolution it);
  float readLightLevel(void);
  void 	powerDown(void);
  void  reset(void);

 private:
  uint8_t   _currentMTreg;
  bool      _BH1750FVIinitialisation;
  bool      _powerON;
  float     _BH1750FVI_Sensitivity;
  #if defined (ARDUINO_ARCH_ESP8266) || (ESP8266_NODEMCU)
    uint8_t _sda, _scl;
  #endif

  BH1750FVI_Resolution _BH1750FVI_Resolution;
  BH1750FVI_Address    _BH1750FVI_Address;

  void powerOn(void);
  void write8(uint8_t data);
};

#endif
