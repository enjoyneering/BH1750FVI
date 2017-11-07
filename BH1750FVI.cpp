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

#include "BH1750FVI.h"


/**************************************************************************/
/*
    Constructor
*/
/**************************************************************************/
BH1750FVI::BH1750FVI(BH1750FVI_Address addr, BH1750FVI_Resolution res, float sensitivity)
{
  _BH1750FVI_Address     = addr;
  _BH1750FVI_Resolution  = res;
  _sensitivity           = sensitivity;
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
#if defined(ESP8266)
bool BH1750FVI::begin(uint8_t sda, uint8_t scl)
{
  Wire.begin(sda, scl);
  Wire.setClock(100000UL);                    //experimental! ESP8266 i2c bus speed: 100kHz..400kHz/100000UL..400000UL, default 100000UL
  Wire.setClockStretchLimit(230);             //experimental! default 230
#else
bool BH1750FVI::begin(void) 
{
  Wire.begin();
  Wire.setClock(100000UL);                    //experimental! AVR i2c bus speed: 31kHz..400kHz/31000UL..400000UL, default 100000UL
#endif

  Wire.beginTransmission(_BH1750FVI_Address); //safety check - make sure the sensor is connected
  if (Wire.endTransmission(true) != 0)
  {
    return false;
  }

  setSensitivity(_sensitivity);
  powerDown();

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
    - possible to detect 0.23 lx in H-resolution  mode with max. sesitivity 3.68
    - possible to detect 0.11 lx in H2-resolution mode with max. sesitivity 3.68
*/
/**************************************************************************/
void BH1750FVI::setResolution(BH1750FVI_Resolution res)
{
  _BH1750FVI_Resolution = res;
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

    NOTE: - 1.00, default sensitivity
          - 0.45, minimum sensitivity
          - 3.68, maximum sensitivity
*/
/**************************************************************************/
void BH1750FVI::setSensitivity(float sensitivity)
{
  uint8_t value                 = 0;
  uint8_t measurnentTimeHighBit = 0;
  uint8_t measurnentTimeLowBit  = 0;

  value = BH1750_MTREG_DEFAULT * sensitivity + 0.5;

  /* safety check - make sure value never exceeds the limits */
  if (value < BH1750_MTREG_MIN) 
  {
    value = BH1750_MTREG_MIN;
    _sensitivity = BH1750_SENSITIVITY_MIN;
  }
  else if (value > BH1750_MTREG_MAX)
  {
    value = BH1750_MTREG_MAX;
    _sensitivity = BH1750_SENSITIVITY_MAX;
  }
  else
  {  
    _sensitivity = sensitivity; 
  }
   _currentMTreg = value;

  measurnentTimeHighBit = _currentMTreg;
  measurnentTimeLowBit  = _currentMTreg;

  /* high bit manipulation */
  measurnentTimeHighBit >>= 5;
  measurnentTimeHighBit |= BH1750_MEASUREMENT_TIME_H;

  /* low bit manipulation */
  measurnentTimeLowBit <<= 3;
  measurnentTimeLowBit >>= 3;
  measurnentTimeLowBit |= BH1750_MEASUREMENT_TIME_L;

  /* update the sensor Measurment Timer register */
  write8(measurnentTimeHighBit);
  write8(measurnentTimeLowBit);
}

/**************************************************************************/
/*
    Gets current sensor sensitivity

    NOTE: - 1.00, default sensitivity
          - 0.45, minimum sensitivity
          - 3.68, maximum sensitivity
*/
/**************************************************************************/
float BH1750FVI::getSensitivity(void)
{
  return _sensitivity;
}

/**************************************************************************/
/*
    Reads light level

    NOTE: Measurement time (integration time) for "H2" and "H" Resolution
          Modes is so long, so almost any noises, including 50Hz/60Hz noise,
          are rejected.
*/
/**************************************************************************/
float BH1750FVI::readLightLevel(void)
{
  int8_t   pollCounter     = BH1750_POLL_LIMIT;
  uint16_t integrationTime = 0;
  uint16_t rawLightLevel   = 0;
  float    lightLevel      = 0;

  /* send measurement instruction */
  write8(_BH1750FVI_Resolution);

  /* measurement delay calculation */
  switch(_BH1750FVI_Resolution)
  {
    case BH1750_CONTINUOUS_HIGH_RES_MODE: case BH1750_CONTINUOUS_HIGH_RES_MODE_2:
      integrationTime = 180 * _sensitivity;        //120..180ms
      _powerON = true;
      break;
    case BH1750_ONE_TIME_HIGH_RES_MODE: case BH1750_ONE_TIME_HIGH_RES_MODE_2:
      integrationTime = 180 * _sensitivity;        //120..180ms
      _powerON = false;
      break;
    case BH1750_CONTINUOUS_LOW_RES_MODE:
      integrationTime = 24 * _sensitivity;         //16..24ms
      _powerON = true;
      break;
    case BH1750_ONE_TIME_LOW_RES_MODE:
      integrationTime = 24 * _sensitivity;         //16..24ms
      _powerON = false;
      break;
  }
  delay(integrationTime);

  do
  {
    pollCounter--;
    if (pollCounter == 0) return BH1750_ERROR;     //error handler

    Wire.requestFrom(_BH1750FVI_Address, 2, true); //true = stop message after transmission & releas the I2C bus  
  }
  while (Wire.available() != 2);                   //check rxBuffer

  /* reads MSB byte, LSB byte from "wire.h" buffer */
  #if (ARDUINO >= 100)
  rawLightLevel  = Wire.read() << 8;
  rawLightLevel |= Wire.read();
  #else
  rawLightLevel  = Wire.receive() << 8;
  rawLightLevel |= Wire.receive();
  #endif

  /* light level calculation */
  if ((_sensitivity != 1) && (_BH1750FVI_Resolution == BH1750_CONTINUOUS_HIGH_RES_MODE_2) || (_BH1750FVI_Resolution == BH1750_ONE_TIME_HIGH_RES_MODE_2))
  {
    lightLevel = 0.5 * (float)(rawLightLevel * BH1750_MTREG_DEFAULT) / (float)_currentMTreg * BH1750_MEASUREMENT_ACCURACY;
  }
  else if (_sensitivity != 1)
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
    Puts sensor to sleep
*/
/**************************************************************************/
void BH1750FVI::powerDown(void)
{
  write8(BH1750_POWER_DOWN);
  _powerON = false;
}

/**************************************************************************/
/*
    Resets the sensor (removes measurement result)

    NOTE: Not accepted in POWER_DOWN mode
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
    Wakes up sensor from sleep
*/
/**************************************************************************/
void BH1750FVI::powerOn(void)
{
  write8(BH1750_POWER_ON);
  _powerON = true;
}

/**************************************************************************/
/*
    Writes 8 bit value over I2C
*/
/**************************************************************************/
void BH1750FVI::write8(uint8_t value)
{
  int8_t pollCounter = BH1750_POLL_LIMIT;

  do
  {
    pollCounter--;
    if (pollCounter == 0) return;               //error handler

    Wire.beginTransmission(_BH1750FVI_Address);
    #if (ARDUINO >= 100)
    Wire.write(value);
    #else
    Wire.send(value);
    #endif
  }
  while (Wire.endTransmission(true) != 0);
}
