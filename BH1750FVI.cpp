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

#include "BH1750FVI.h"


/**************************************************************************/
/*
    Constructor
*/
/**************************************************************************/
BH1750FVI::BH1750FVI(sensorAddress it_1, sensorResolution it_2, float sensorSensitivity)
{
  _BH1750FVIinitialisation = false;
  _sensorAddress = it_1;
  _sensorResolution = it_2;
  _sensorSensitivity = sensorSensitivity;
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
bool BH1750FVI::begin(void)
{

  Wire.begin();

  /* Make sure we're actually connected */
  Wire.beginTransmission(_sensorAddress);
  if (Wire.endTransmission() != 0)
  {
    return false;
  }

  _BH1750FVIinitialisation = true;

  setResolution(_sensorResolution);
  setSensitivity(_sensorSensitivity);

  return true;
}

/**************************************************************************/
/*
    Sets sensor Resolution
    Continuous Mode:
    BH1750_CONTINUOUS_HIGH_RES_MODE   - 1.0 lx resolution. Integration time 120ms
    BH1750_CONTINUOUS_HIGH_RES_MODE_2 - 0.5 lx resolution. Integration time 120ms (defaul mode)
    BH1750_CONTINUOUS_LOW_RES_MODE    - 4.0 lx resolution. Integration time 16ms
    
    Device sets the Power down after measurement:
    BH1750_ONE_TIME_HIGH_RES_MODE   - 1.0 lx resolution. Integration time 120ms
    BH1750_ONE_TIME_HIGH_RES_MODE_2 - 0.5 lx resolution. Integration time 120ms
    BH1750_ONE_TIME_LOW_RES_MODE    - 4.0 lx resolution. Integration time 16ms
*/
/**************************************************************************/
void BH1750FVI::setResolution(sensorResolution it)
{

  if (_BH1750FVIinitialisation != true)
  {
    begin();
  }

  write8(it);

  /* Update value placeholders */
  _sensorResolution = it;
}

/**************************************************************************/
/*
    Sets sensor sensitivity
    - 1.00, default Sensitivity (defaul mode)
    - 0.45, minimum Sensitivity
    - 3.68, maximum Sensitivity
*/
/**************************************************************************/
void BH1750FVI::setSensitivity(float sensorSensitivity)
{

  uint8_t value;
  uint8_t measurnentTimeHighBit;
  uint8_t measurnentTimeLowBit;

  if (_BH1750FVIinitialisation != true)
  {
    begin();
  }

  value = BH1750_MTREG_DEFAULT * sensorSensitivity;

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

  measurnentTimeHighBit >>= 5;
  
  measurnentTimeLowBit <<= 3;
  measurnentTimeLowBit >>= 3;

  measurnentTimeHighBit |= BH1750_MEASUREMENT_TIME_H;
  measurnentTimeLowBit |= BH1750_MEASUREMENT_TIME_L;

  /* Update the sensor Measurment Timet register */
  write8(measurnentTimeHighBit);
  write8(measurnentTimeLowBit);

  /* Update value placeholders */
  _sensorSensitivity = sensorSensitivity;
}

/**************************************************************************/
/*
    powerOn()
*/
/**************************************************************************/
void BH1750FVI::powerOn(void)
{

  if (_BH1750FVIinitialisation != true)
  {
    begin();
  }

  write8(BH1750_POWER_ON);
  _powerDown = false;
}

/**************************************************************************/
/*
    powerDown()
*/
/**************************************************************************/
void BH1750FVI::powerDown(void)
{

  if (_BH1750FVIinitialisation != true)
  {
    begin();
  }

  write8(BH1750_POWER_DOWN);
  _powerDown = true;
}

/**************************************************************************/
/*
    reset()

    Removes measurement result. Not accepted in POWER_DOWN mode
*/
/**************************************************************************/
void BH1750FVI::reset(void)
{

  if (_powerDown == false)
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
    readLightLevel()
*/
/**************************************************************************/
float BH1750FVI::readLightLevel(void)
{

  uint16_t  integrationTime;
  uint16_t  rawLightLevel;
  float     LightLevel;

  if (_powerDown == true)
  {
    powerOn();
  }

  setSensitivity(_sensorSensitivity);
  setResolution(_sensorResolution);

  switch(_sensorResolution)
  {
    case BH1750_CONTINUOUS_HIGH_RES_MODE:
      integrationTime = 125 * _sensorSensitivity;
    break;
    case BH1750_CONTINUOUS_HIGH_RES_MODE_2:
      integrationTime = 125 * _sensorSensitivity;
    break;
    case BH1750_CONTINUOUS_LOW_RES_MODE:
      integrationTime = 20 * _sensorSensitivity;
    break;
    case BH1750_ONE_TIME_HIGH_RES_MODE:
      integrationTime = 125 * _sensorSensitivity;
    break;
    case BH1750_ONE_TIME_HIGH_RES_MODE_2:
      integrationTime = 125 * _sensorSensitivity;
    break;
    case BH1750_ONE_TIME_LOW_RES_MODE:
      integrationTime = 20 * _sensorSensitivity;
    break;
  }

  delay(integrationTime);

  /* read raw Light Level */
  Wire.requestFrom(_sensorAddress, 2);
#if (ARDUINO >= 100)
  rawLightLevel = Wire.read();
  rawLightLevel <<= 8;
  rawLightLevel |= Wire.read();
#else
  rawLightLevel = Wire.receive();
  rawLightLevel <<= 8;
  rawLightLevel |= Wire.receive();
#endif

  /* calculate Light Level */
  if (_sensorResolution == BH1750_CONTINUOUS_HIGH_RES_MODE_2 || _sensorResolution == BH1750_ONE_TIME_HIGH_RES_MODE_2)
  {
    LightLevel = (0.5 * rawLightLevel * _currentMTreg) / (1.2 * (float)BH1750_MTREG_DEFAULT);
  }
  else
  {
    LightLevel = (rawLightLevel * _currentMTreg) / (1.2 * (float)BH1750_MTREG_DEFAULT);
  }

  return LightLevel;
}

/**************************************************************************/
/*
    Writes 8 bit value over I2C
*/
/**************************************************************************/
void BH1750FVI::write8(uint8_t value)
{
  Wire.beginTransmission(_sensorAddress);
#if (ARDUINO >= 100)
  Wire.write(value);
#else
  Wire.send(valued);
#endif
  Wire.endTransmission();
}


