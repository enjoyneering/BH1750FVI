/**************************************************************************/
/*
This is an Arduino library for the ROHM BH1750FVI Ambient Light Sensor

  written by enjoyneering79

  Wide range:           1 - 65535 lx (in defaul mode)
  Possible to detect:   0.11 lx - 100'000 lx by changing
                        Sensitivity aka Integration Time

  These sensor uses I2C  to communicate, 2 pins are required to  
  interface

  Connect BH1750FVI to pins :  SDA  SCL
  Uno, Redboard, Pro:          A4   A5
  Mega2560, Due:               20   21
  Leonardo:                    2    3

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
 #include <Wire.h>


#define BH1750_POWER_DOWN                   0x00  /* No active state */
#define BH1750_POWER_ON                     0x01  /* Wating for measurment command */
#define BH1750_RESET                        0x07  /* Reset all registers. Not accepted in POWER_DOWN mode */

#define BH1750_MEASUREMENT_TIME_H           0x04  /* High Bit of changing Measurement Time. */
#define BH1750_MEASUREMENT_TIME_L           0x60  /* Low  Bit of changing Measurement Time. */
#define BH1750_MTREG_DEFAULT                0x45     /* Default Integration/Measurement time = 69  */
#define BH1750_MTREG_MIN                    0x1F     /* Min.    Integration/Measurement time = 31  */
#define BH1750_MTREG_MAX                    0xFE     /* Max.    Integration/Measurement time = 254 */


typedef enum
{
BH1750_DEFAULT_I2CADDR 	= 0x23,	/* Device Address on i2c serial bus when address pin LOW */
BH1750_SECOND_I2CADDR 	= 0x5C	/* Device Address on i2c serial bus when address pin HIGH */
}
BH1750FVI_Address;

typedef enum
{
BH1750_CONTINUOUS_HIGH_RES_MODE     = 0x10,  /* Continuous measurement @ 1.0 lx resolution. Integration time is approx 120ms. */
BH1750_CONTINUOUS_HIGH_RES_MODE_2   = 0x11,  /* Continuous measurement @ 0.5 lx resolution. Integration time is approx 120ms. */
BH1750_CONTINUOUS_LOW_RES_MODE      = 0x13,  /* Continuous measurement @ 4.0 lx resolution. Integration time is approx 16ms.  */

BH1750_ONE_TIME_HIGH_RES_MODE     = 0x20,  /* One-time measurement (device sets the Power down after measurement) @ 1.0 lx resolution. Integration time is approx 120ms. */
BH1750_ONE_TIME_HIGH_RES_MODE_2   = 0x21,  /* One-time measurement (device sets the Power down after measurement) @ 0.5 lx resolution. Integration time is approx 120ms. */
BH1750_ONE_TIME_LOW_RES_MODE      = 0x23   /* One-time measurement (device sets the Power down after measurement) @ 4.0 lx resolution. Integration time is approx 16ms.  */
}   
BH1750FVI_Resolution;

class BH1750FVI 
{
 public:

  BH1750FVI(BH1750FVI_Address = BH1750_DEFAULT_I2CADDR, BH1750FVI_Resolution = BH1750_CONTINUOUS_HIGH_RES_MODE_2, float BH1750FVI_Sensitivity = 1.00);

  bool 	begin(void);
  void 	setSensitivity(float BH1750FVI_Sensitivity);
  void 	setResolution(BH1750FVI_Resolution it);
  float readLightLevel(void);
  void 	powerDown(void);
  void  reset(void);

 private:
  uint8_t   _currentMTreg;
  bool      _BH1750FVIinitialisation;
  bool      _powerDown;
  float     _BH1750FVI_Sensitivity;

  BH1750FVI_Resolution  _BH1750FVI_Resolution;
  BH1750FVI_Address     _BH1750FVI_Address;

  void 		powerOn(void);
  void 		write8(uint8_t data);
};

#endif
