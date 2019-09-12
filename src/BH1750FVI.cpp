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

#include "BH1750FVI.h"


/**************************************************************************/
/*
    Constructor
*/
/**************************************************************************/

BH1750FVI::BH1750FVI(BH1750FVI_ADDRESS addr, BH1750FVI_RESOLUTION res, float sensitivity, float calibration)
{
  _sensorAddress    = addr;
  _sensorResolution = res;
  _sensitivity      = sensitivity;
  _accuracy         = calibration;
}

/**************************************************************************/
/*
    Initializes I2C and configures the sensor, call this function before
    doing anything else

    NOTE:
    - Wire.endTransmission() returned value:
      - 0 success
      - 1 data too long to fit in transmit data16
      - 2 received NACK on transmit of address
      - 3 received NACK on transmit of data
      - 4 other error
*/
/**************************************************************************/
#if defined(ESP8266)
bool BH1750FVI::begin(uint8_t sda, uint8_t scl)
{
  Wire.begin(sda, scl);
  Wire.setClock(100000);                             //experimental! ESP8266 i2c bus speed: 50kHz..400kHz/50000..400000, default 100000
  Wire.setClockStretchLimit(230);                    //experimental! default 230
#else
bool BH1750FVI::begin(void) 
{
  Wire.begin();
  Wire.setClock(100000);                             //experimental! AVR i2c bus speed: 31kHz..400kHz/31000..400000, default 100000
#endif

  Wire.beginTransmission(_sensorAddress);            //safety check, make sure the sensor is connected
  if (Wire.endTransmission(true) != 0) return false;

  setSensitivity(_sensitivity);
  powerDown();

  return true;
}

/**************************************************************************/
/*
    Set sensor Resolution

    Continuous Mode:
    BH1750_CONTINUOUS_HIGH_RES_MODE   - 1.0 lx resolution
    BH1750_CONTINUOUS_HIGH_RES_MODE_2 - 0.5 lx resolution
    BH1750_CONTINUOUS_LOW_RES_MODE    - 4.0 lx resolution
    
    One time mode, one measurement & power down:
    BH1750_ONE_TIME_HIGH_RES_MODE     - 1.0 lx resolution
    BH1750_ONE_TIME_HIGH_RES_MODE_2   - 0.5 lx resolution, by default
    BH1750_ONE_TIME_LOW_RES_MODE      - 4.0 lx resolution

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
    Set sensor sensitivity

    This option is used to compensate the influence of optical filter.
    Any filter you put in front of the sensor blocks some light. For
    example, when transmission rate of optical window is 50% (measurement
    result becomes 0.5 times lower), influence of optical window is ignored
    by changing sensor sensitivity from default 1.0 to 2.0 times.

    NOTE:
    - sensitivity range 0.45 - 3.68, default 1.00
    - MTreg/measurement time register range 31 - 254, default 69
*/
/**************************************************************************/
bool BH1750FVI::setSensitivity(float sensitivity)
{
  float   oldSensitivity        = 0;
  uint8_t valueMTreg            = 0;
  uint8_t measurnentTimeHighBit = 0;
  uint8_t measurnentTimeLowBit  = 0;

  oldSensitivity = _sensitivity;                      //backup current sensitivity
  
  valueMTreg = sensitivity * BH1750_MTREG_DEFAULT;    //calculating MTreg value for new sensitivity

  /* safety check, make sure valueMTreg never exceeds the limits */
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
  measurnentTimeHighBit |= BH1750_MEASUREMENT_TIME_H; //0,1,0,0  0,7-bit,6-bit,5-bit

  /* low bit manipulation */
  measurnentTimeLowBit <<= 3;
  measurnentTimeLowBit >>= 3;
  measurnentTimeLowBit |= BH1750_MEASUREMENT_TIME_L;  //0,1,1,4-bit  3-bit,2-bit,1-bit,0-bit

  /* update sensor Measurment Timer register */
  if ((write8(measurnentTimeHighBit) != true) || (write8(measurnentTimeLowBit) != true))
  {
    _sensitivity = oldSensitivity;                    //collision on the i2c bus, use old value
    return false;
  }

  return true;
}

/**************************************************************************/
/*
    Get current sensor sensitivity

    NOTE:
    - sensitivity range 0.45 - 3.68, default 1.00
*/
/**************************************************************************/
float BH1750FVI::getSensitivity(void)
{
  return _sensitivity;
}

/**************************************************************************/
/*
    Read light level

    NOTE:
    - measurement/integration time for resolution modes "H2" & "H"
      is very long, so almost any noise, including light fluctuations
      of 50Hz/60Hz, is rejected.
*/
/**************************************************************************/
float BH1750FVI::readLightLevel(void)
{
  uint16_t integrationTime = 0;
  uint16_t rawLightLevel   = 0;
  float    lightLevel      = 0;

  /* send measurement instruction */
  if (write8(_sensorResolution) != true) return BH1750_ERROR; //error handler, collision on the i2c bus

  /* measurement delay */
  switch(_sensorResolution)                                   //"switch-case" faster & has smaller footprint than "if-else", see Atmel AVR4027 Application Note
  {
    case BH1750_CONTINUOUS_HIGH_RES_MODE:
    case BH1750_CONTINUOUS_HIGH_RES_MODE_2:
    case BH1750_ONE_TIME_HIGH_RES_MODE:
    case BH1750_ONE_TIME_HIGH_RES_MODE_2:
      integrationTime = _sensitivity * 180;                   //120..180msec * (0.45 .. 3.68) 
      break;

    case BH1750_CONTINUOUS_LOW_RES_MODE:
    case BH1750_ONE_TIME_LOW_RES_MODE:
      integrationTime = _sensitivity * 24;                    //16..24msec * (0.45 .. 3.68)
      break;
  }
  delay(integrationTime);

  #if defined(_VARIANT_ARDUINO_STM32_)
  Wire.requestFrom(_sensorAddress, 2);
  #else
  Wire.requestFrom(_sensorAddress, 2, true);                  //"true" to stop message after transmission & releas I2C bus
  #endif

  if (Wire.available() != 2) return BH1750_ERROR;             //check "wire.h" rxBuffer & collision on the i2c bus

  /* reads MSB byte, LSB byte from "wire.h" rxBuffer */
  #if (ARDUINO >= 100)
  rawLightLevel  = Wire.read() << 8;
  rawLightLevel |= Wire.read();
  #else
  rawLightLevel  = Wire.receive() << 8;
  rawLightLevel |= Wire.receive();
  #endif

  /* light level calculation, p.11 */
  switch (_sensorResolution)
  {
    case BH1750_ONE_TIME_HIGH_RES_MODE_2:
    case BH1750_CONTINUOUS_HIGH_RES_MODE_2:
      lightLevel = 0.5 * (float)rawLightLevel / _accuracy * _sensitivity;
      break;

    case BH1750_ONE_TIME_LOW_RES_MODE:
    case BH1750_ONE_TIME_HIGH_RES_MODE:
    case BH1750_CONTINUOUS_LOW_RES_MODE:
    case BH1750_CONTINUOUS_HIGH_RES_MODE:
      lightLevel = (float)rawLightLevel / _accuracy * _sensitivity;
      break;
  }

  return lightLevel;
}

/**************************************************************************/
/*
    Put sensor to sleep
*/
/**************************************************************************/
void BH1750FVI::powerDown(void)
{
  write8(BH1750_POWER_DOWN);
}

/**************************************************************************/
/*
    Wakes up sensor from sleep

    NOTE:
    - wating for measurment command
*/
/**************************************************************************/
void BH1750FVI::powerOn(void)
{
  write8(BH1750_POWER_ON);
}

/**************************************************************************/
/*
    Reset sensor

    NOTE:
    - used for removing previous measurement, reset only illuminance
      data register
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
    - accuracy = sensor output lux / actual lux
    - accuracy/calibration range 0.96 - 1.44, typical - 1.2
*/
/**************************************************************************/
void BH1750FVI::setCalibration(float value)
{
  /* safety check, make sure value never exceeds calibration range */
  if      (value < 0.96) _accuracy = 0.96;
  else if (value > 1.44) _accuracy = 1.44;
  else                   _accuracy = value;
}

/**************************************************************************/
/*
    Return sensor calibration value

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
    Write 8-bits value over I2C
*/
/**************************************************************************/
bool BH1750FVI::write8(uint8_t value)
{
  Wire.beginTransmission(_sensorAddress);

  #if (ARDUINO >= 100)
  Wire.write(value);
  #else
  Wire.send(value);
  #endif
  
  if (Wire.endTransmission(true) == 0) return true;  //"true" sends stop message after transmission & releases I2C bus
                                       return false;
}
