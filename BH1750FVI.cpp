/***************************************************************************************************/
/*
  This is an Arduino library for the ROHM BH1750FVI Ambient Light Sensor

  Default range: 1 - 65'535 lx
  
  written by : enjoyneering79
  sourse code: https://github.com/enjoyneering/

  This sensor uses I2C bus to communicate, specials pins are required to interface
  Board:                                    SDA                    SCL
  Uno, Mini, Pro, ATmega168, ATmega328..... A4                     A5
  Mega2560, Due............................ 20                     21
  Leonardo, Micro, ATmega32U4.............. 2                      3
  Digistump, Trinket, ATtiny85............. 0/physical pin no.5    2/physical pin no.7
  Blue Pill, STM32F103xxxx boards.......... PB7*                   PB6*
  ESP8266 ESP-01:.......................... GPIO0/D5               GPIO2/D3
  NodeMCU 1.0, WeMos D1 Mini............... GPIO4/D2               GPIO5/D1

                                           *STM32F103xxxx pins B7/B7 are 5v tolerant, but bi-directional
                                            logic level converter is recommended

  Frameworks & Libraries:
  ATtiny Core           - https://github.com/SpenceKonde/ATTinyCore
  ESP8266 Core          - https://github.com/esp8266/Arduino
  ESP8266 I2C lib fixed - https://github.com/enjoyneering/ESP8266-I2C-Driver
  STM32 Core            - https://github.com/rogerclarkmelbourne/Arduino_STM32

  GNU GPL license, all text above must be included in any redistribution, see link below for details:
  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/

#include "BH1750FVI.h"


/**************************************************************************/
/*
    Constructor
*/
/**************************************************************************/
BH1750FVI::BH1750FVI(BH1750FVI_ADDRESS addr, BH1750FVI_RESOLUTION res, float sensitivity)
{
  _sensorAddress    = addr;
  _sensorResolution = res;
  _sensitivity      = sensitivity;
  _accuracy         = BH1750_MEASUREMENT_ACCURACY;
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
  Wire.setClock(100000UL);                           //experimental! ESP8266 i2c bus speed: 100kHz..400kHz/100000UL..400000UL, default 100000UL
  Wire.setClockStretchLimit(230);                    //experimental! default 230
#else
bool BH1750FVI::begin(void) 
{
  Wire.begin();
  Wire.setClock(100000UL);                           //experimental! AVR i2c bus speed: 31kHz..400kHz/31000UL..400000UL, default 100000UL
#endif

  Wire.beginTransmission(_sensorAddress);            //safety check - make sure the sensor is connected
  if (Wire.endTransmission(true) != 0) return false;

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
void BH1750FVI::setResolution(BH1750FVI_RESOLUTION res)
{
  _sensorResolution = res;
}

/**************************************************************************/
/*
    Sets sensor sensitivity

    This option is used to compensate the influence of the optical filter.
    For example, when transmission rate of optical window is 50% (measurement
    result becomes 0.5 times lower), influence of optical window is ignored
    by changing sensor sensitivity from default 1.0 to 2.0 times

    NOTE:
    - sensitivity 0.45 - 3.68, default 1.00
    - measurement time register 31 - 254, default 69
*/
/**************************************************************************/
void BH1750FVI::setSensitivity(float sensitivity)
{
  uint8_t valueMTreg            = 0;
  uint8_t measurnentTimeHighBit = 0;
  uint8_t measurnentTimeLowBit  = 0;

  valueMTreg = BH1750_MTREG_DEFAULT * sensitivity; //calculating MTreg value for new sensitivity

  /* safety check - make sure valueMTreg never exceeds the limits */
  if (valueMTreg < BH1750_MTREG_MIN) 
  {
    valueMTreg   = BH1750_MTREG_MIN;
    _sensitivity = BH1750_SENSITIVITY_MIN;
  }
  else if (valueMTreg > BH1750_MTREG_MAX)
  {
    valueMTreg   = BH1750_MTREG_MAX;
    _sensitivity = BH1750_SENSITIVITY_MAX;
  }
  else
  {  
    _sensitivity = sensitivity; 
  }

  measurnentTimeHighBit = valueMTreg;
  measurnentTimeLowBit  = valueMTreg;

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

    NOTE:
    - sensitivity 0.45 - 3.68, default 1.00
*/
/**************************************************************************/
float BH1750FVI::getSensitivity(void)
{
  return _sensitivity;
}

/**************************************************************************/
/*
    Reads light level

    NOTE:
    - measurement/integration time for "H2" and "H" resolution modes is so
     long, so almost any noises including 50Hz/60Hz light fluctuation
     is rejected.
*/
/**************************************************************************/
float BH1750FVI::readLightLevel(void)
{
  int8_t   pollCounter     = BH1750_POLLING_LIMIT;
  uint16_t integrationTime = 0;
  uint16_t rawLightLevel   = 0;
  float    lightLevel      = 0;

  /* send measurement instruction */
  write8(_sensorResolution);

  /* measurement delay */
  switch(_sensorResolution)
  {
    case BH1750_CONTINUOUS_HIGH_RES_MODE: case BH1750_CONTINUOUS_HIGH_RES_MODE_2:
      integrationTime = 180 * _sensitivity;        //120..180ms * (0.45 .. 3.68) 
      break;

    case BH1750_ONE_TIME_HIGH_RES_MODE: case BH1750_ONE_TIME_HIGH_RES_MODE_2:
      integrationTime = 180 * _sensitivity;        //120..180ms  * (0.45 .. 3.68)
      break;

    case BH1750_CONTINUOUS_LOW_RES_MODE:
      integrationTime = 24 * _sensitivity;         //16..24ms * (0.45 .. 3.68)
      break;

    case BH1750_ONE_TIME_LOW_RES_MODE:
      integrationTime = 24 * _sensitivity;         //16..24ms * (0.45 .. 3.68)
      break;
  }
  delay(integrationTime);

  do
  {
    if (pollCounter-- == 0) return BH1750_ERROR;   //error handler, collision on the i2c bus

    #if defined(_VARIANT_ARDUINO_STM32_)
    Wire.requestFrom(_sensorAddress, 2);
    #else
    Wire.requestFrom(_sensorAddress, 2, true);     //true = stop message after transmission & releas the I2C bus
    #endif
  }
  while (Wire.available() != 2);                   //check rxBuffer

  /* reads MSB byte, LSB byte from "wire.h" rxBuffer */
  #if (ARDUINO >= 100)
  rawLightLevel  = Wire.read() << 8;
  rawLightLevel |= Wire.read();
  #else
  rawLightLevel  = Wire.receive() << 8;
  rawLightLevel |= Wire.receive();
  #endif

  /* light level calculation */
  if ((_sensorResolution == BH1750_CONTINUOUS_HIGH_RES_MODE_2) || (_sensorResolution == BH1750_ONE_TIME_HIGH_RES_MODE_2))
  {
    lightLevel = 0.5 * _sensitivity * (float)rawLightLevel / _accuracy;
  }
  else
  {
    lightLevel = _sensitivity * (float)rawLightLevel / _accuracy;
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
}

/**************************************************************************/
/*
    Wakes up sensor from sleep
*/
/**************************************************************************/
void BH1750FVI::powerOn(void)
{
  write8(BH1750_POWER_ON);
}

/**************************************************************************/
/*
    Resets the sensor

    NOTE:
    - reset only illuminance data register/removes measurement result
    - not accepted in power-down mode
*/
/**************************************************************************/
void BH1750FVI::reset(void)
{
  write8(BH1750_RESET);
}

/**************************************************************************/
/*
    Set sensor calibration value

    NOTE:
    - measurement accuracy ±20%
    - accuracy = sensor out lux / actual lux
    - accuracy/calibration range 0.96 - 1.44, typical - 1.2
*/
/**************************************************************************/
void BH1750FVI::setCalibration(float value)
{
  /* safety check - make sure value never exceeds calibration range */
  if      (value < 0.96) _accuracy = 0.96;
  else if (value > 1.44) _accuracy = 1.44;
  else                   _accuracy = value;
}

/**************************************************************************/
/*
    Returns sensor calibration value

    NOTE:
    - calibration range 0.96 - 1.44, typical - 1.2
*/
/**************************************************************************/
float BH1750FVI::getCalibration(void)
{
  return _accuracy;
}

/**************************************************************************/
/*
    Writes 8-bit value over I2C
*/
/**************************************************************************/
void BH1750FVI::write8(uint8_t value)
{
  int8_t pollCounter = BH1750_POLLING_LIMIT;

  do
  {
    pollCounter--;

    Wire.beginTransmission(_sensorAddress);

    #if (ARDUINO >= 100)
    Wire.write(value);
    #else
    Wire.send(value);
    #endif
  }
  while ((Wire.endTransmission(true) != 0) || (pollCounter > 0));
}
