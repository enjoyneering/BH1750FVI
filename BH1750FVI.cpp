/**************************************************************************/
/*
This is an Arduino library for the ROHM BH1750FVI Ambient Light Sensor

  written by enjoyneering79

  Wide range:           1 - 65'535 lx (by default)
  Possible to detect:   0.11 - 100'000 lx by changing Sensitivity

  These sensor uses I2C to communicate. Two pins are required to  
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

#include "BH1750FVI.h"


/**************************************************************************/
/*
    Constructor
*/
/**************************************************************************/
BH1750FVI::BH1750FVI(BH1750FVI_Address addr, BH1750FVI_Resolution res, float BH1750FVI_Sensitivity)
{
  _BH1750FVIinitialisation = false;
  _BH1750FVI_Address       = addr;
  _BH1750FVI_Resolution    = res;
  _BH1750FVI_Sensitivity   = BH1750FVI_Sensitivity;
}

/**************************************************************************/
/*
    Initializes I2C and configures the sensor (call this function before
    doing anything else)

    Wire.endTransmission():
    0 - success
    1 - data too long to fit in transmit buffer
    2 - received NACK on transmit of address
    3 - received NACK on transmit of data
    4 - other error
*/
/**************************************************************************/
#if defined(ARDUINO_ARCH_ESP8266) || (ESP8266_NODEMCU)
bool BH1750FVI::begin(uint8_t sda, uint8_t scl)
{
  _sda = sda;
  _scl = scl;

  Wire.begin(_sda, _scl);
#else
  bool BH1750FVI::begin(void) 
{
  Wire.begin();
#endif

  /* Make sure we have sensor on the I2C bus */
  Wire.beginTransmission(_BH1750FVI_Address);
  if (Wire.endTransmission() != 0)
  {
    return false;
  }

  _BH1750FVIinitialisation = true;

  /* Sensor's initial state is Power Down mode after VCC supply */
  _powerON = false;

  setSensitivity(_BH1750FVI_Sensitivity);

  return true;
}

/**************************************************************************/
/*
    Sets sensor Resolution

    Continuous Mode:
    BH1750_CONTINUOUS_HIGH_RES_MODE   - 1.0 lx resolution. Integration time 180ms
    BH1750_CONTINUOUS_HIGH_RES_MODE_2 - 0.5 lx resolution. Integration time 180ms (by default)
    BH1750_CONTINUOUS_LOW_RES_MODE    - 4.0 lx resolution. Integration time 24ms
    
    One time mode (power down after the measurement):
    BH1750_ONE_TIME_HIGH_RES_MODE   - 1.0 lx resolution. Integration time 180ms
    BH1750_ONE_TIME_HIGH_RES_MODE_2 - 0.5 lx resolution. Integration time 180ms
    BH1750_ONE_TIME_LOW_RES_MODE    - 4.0 lx resolution. Integration time 24ms

    NOTE:
    It's possible to detect 0.23 lx in H-resolution  mode with max. sesitivity 3.68
    It's possible to detect 0.11 lx in H2-resolution mode with max. sesitivity 3.68
*/
/**************************************************************************/
void BH1750FVI::setResolution(BH1750FVI_Resolution it)
{
  _BH1750FVI_Resolution = it;
}

/**************************************************************************/
/*
    Sets sensor sensitivity

    This option is used for compensating the influence of the optical filter
    before sensor (if present), or for increasing sensor's dynamic range
    from  1 - 65535 lx to 0.11 - 100000 lx.
    For example, when transmission rate of optical filter is 50% (measurement
    result becomes 0.5 times, if filter is set), influence of optical filter
    is ignored by changing sensor sensitivity from default to 2 times.

    NOTE: 
    - 1.00, default sensitivity
    - 0.45, minimum sensitivity
    - 3.68, maximum sensitivity
*/
/**************************************************************************/
void BH1750FVI::setSensitivity(float BH1750FVI_Sensitivity)
{
  uint8_t value;
  uint8_t measurnentTimeHighBit;
  uint8_t measurnentTimeLowBit;

  if (_BH1750FVIinitialisation != true)
  {
    #if defined(ARDUINO_ARCH_ESP8266) || (ESP8266_NODEMCU)
     begin(_sda, _scl);
    #else
     begin();
    #endif
  }

  value = BH1750_MTREG_DEFAULT * BH1750FVI_Sensitivity + 0.5;

  if (value < BH1750_MTREG_MIN) 
  {
    value = BH1750_MTREG_MIN;
  }
  else if (value > BH1750_MTREG_MAX)
  {
    value = BH1750_MTREG_MAX;
  }
 
  _currentMTreg = value;

  measurnentTimeHighBit = _currentMTreg;
  measurnentTimeLowBit  = _currentMTreg;

  /* HIGH bit manipulation */
  measurnentTimeHighBit >>= 5;
  measurnentTimeHighBit |= BH1750_MEASUREMENT_TIME_H;

  /* LOW bit manipulation */
  measurnentTimeLowBit <<= 3;
  measurnentTimeLowBit >>= 3;
  measurnentTimeLowBit |= BH1750_MEASUREMENT_TIME_L;

  /* Update the sensor Measurment Timer register */
  write8(measurnentTimeHighBit);
  write8(measurnentTimeLowBit);

  /* Update value placeholder */
  _BH1750FVI_Sensitivity = BH1750FVI_Sensitivity;
}

/**************************************************************************/
/*
    Wakes up sensor from sleep
*/
/**************************************************************************/
void BH1750FVI::powerOn(void)
{
  if (_BH1750FVIinitialisation != true)
  {
    #if defined(ARDUINO_ARCH_ESP8266) || (ESP8266_NODEMCU)
     begin(_sda, _scl);
    #else
     begin();
    #endif
  }

  if (_powerON == false)
  {
    write8(BH1750_POWER_ON);
    _powerON = true;
  }
}

/**************************************************************************/
/*
    Puts sensor to sleep
*/
/**************************************************************************/
void BH1750FVI::powerDown(void)
{
  if (_BH1750FVIinitialisation != true)
  {
    #if defined(ARDUINO_ARCH_ESP8266) || (ESP8266_NODEMCU)
     begin(_sda, _scl);
    #else
     begin();
    #endif
  }
  
  if (_powerON == true)
  {
    write8(BH1750_POWER_DOWN);
    _powerON = false;
  }
}

/**************************************************************************/
/*
    Resets the sensor (removes measurement result)

    NOTE:
    Not accepted in POWER_DOWN mode
*/
/**************************************************************************/
void BH1750FVI::reset(void)
{
  if (_powerON == true)
  {
    write8(BH1750_RESET);
  }
  else
  {
    powerOn();
    write8(BH1750_RESET);
    powerDown();
  }
}

/**************************************************************************/
/*
    Reads light level

    NOTE:
    Measurement time (integration time) in "H2" and "H" Resolution Modes is
    so long, so almost any noises, including 50Hz/60Hz noise, are rejected.
*/
/**************************************************************************/
float BH1750FVI::readLightLevel(void)
{

  uint16_t integrationTime;
  uint16_t rawLightLevel;
  float    lightLevel;

  /* send start measurement instruction */
  write8(_BH1750FVI_Resolution);

  /* measurement delay calculation */
  switch(_BH1750FVI_Resolution)
  {
    case BH1750_CONTINUOUS_HIGH_RES_MODE:
      integrationTime = 180 * _BH1750FVI_Sensitivity; //120..180ms
      _powerON = true;
      break;
    case BH1750_CONTINUOUS_HIGH_RES_MODE_2:
      integrationTime = 180 * _BH1750FVI_Sensitivity; //120..180ms
      _powerON = true;
      break;
    case BH1750_CONTINUOUS_LOW_RES_MODE:
      integrationTime = 24 * _BH1750FVI_Sensitivity;  //16..24ms
      _powerON = true;
      break;
    case BH1750_ONE_TIME_HIGH_RES_MODE:
      integrationTime = 180 * _BH1750FVI_Sensitivity; //120..180ms
      _powerON = false;
      break;
    case BH1750_ONE_TIME_HIGH_RES_MODE_2:
      integrationTime = 180 * _BH1750FVI_Sensitivity; //120..180ms
      _powerON = false;
      break;
    case BH1750_ONE_TIME_LOW_RES_MODE:
      integrationTime = 24 * _BH1750FVI_Sensitivity;  //16..24ms
      _powerON = false;
      break;
  }
  delay(integrationTime);

  /* read raw Light Level */
  Wire.beginTransmission(_BH1750FVI_Address);
  Wire.requestFrom(_BH1750FVI_Address, 2);
  #if (ARDUINO >= 100)
    rawLightLevel  = Wire.read() << 8;
    rawLightLevel |= Wire.read();
  #else
    rawLightLevel  = Wire.receive() << 8;
    rawLightLevel |= Wire.receive();
  #endif
  Wire.endTransmission();

  /* Light Level calculation */
  if ((_BH1750FVI_Sensitivity != 1) && (_BH1750FVI_Resolution == BH1750_CONTINUOUS_HIGH_RES_MODE_2) || (_BH1750FVI_Resolution == BH1750_ONE_TIME_HIGH_RES_MODE_2))
  {
    lightLevel = 0.5 * (float)(rawLightLevel * BH1750_MTREG_DEFAULT) / (float)_currentMTreg * BH1750_MEASUREMENT_ACCURACY;
  }
  else if (_BH1750FVI_Sensitivity != 1)
  {
    lightLevel = (float)(rawLightLevel * BH1750_MTREG_DEFAULT) / (float)_currentMTreg * BH1750_MEASUREMENT_ACCURACY;
  }
  else
  {
    lightLevel = (float)rawLightLevel / BH1750_MEASUREMENT_ACCURACY;
  }

  return lightLevel;
}

/**************************************************************************/
/*
    Writes 8 bit value over I2C
*/
/**************************************************************************/
void BH1750FVI::write8(uint8_t value)
{
  Wire.beginTransmission(_BH1750FVI_Address);
  #if (ARDUINO >= 100)
    Wire.write(value);
  #else
    Wire.send(value);
  #endif
    Wire.endTransmission();
}
