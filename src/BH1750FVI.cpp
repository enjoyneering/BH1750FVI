/***************************************************************************************************/
/*
   This is an Arduino library for the ROHM BH1750FVI Ambient Light Sensor

   written by : enjoyneering
   sourse code: https://github.com/enjoyneering/

   ROHM BH1750FVI features:
   - power supply voltage +2.4v..+3.6v, absolute maximum +4.5v
   - maximum current 190uA, sleep current 1uA
   - I2C bus speed 100KHz..400KHz, up to 2 sensors on the bus
   - maximum sensitivity at 560nm, yellow-green light
   - 50Hz/60Hz flicker reduction
   - measurement accuracy +-20%
   - optical filter compensation by changing sensitivity* 0.45..3.68
   - calibration by changing the accuracy 0.96..1.44:
     - typical accuracy values:
       - 1.00, fluorescent light
       - 1.06, white LED & artifical sun
       - 1.15, halogen light
       - 1.18, krypton light light
       - 1.20, incandescent light (by default)
   - onetime+sleep and continuous measurement mode
   - typical measurement resolution:
     - 0.5 lux at high resolution mode2
     - 1.0 lux at high resolution mode (by default)
     - 4.0 lux at low resolution mode
   - typical measurement range depends on resolution mode sensitivity & accuracy values:
     - from 1..32767 to 1..65535 lux
   - typical measurement interval depends on resolution mode & sensitivity:
     - from 81..662 msec to 10..88 msec
     *Any optical filter you put in front of the sensor blocks some light. Sensitivity is used
      to compensate the influence of the optical filter. For example, when transmission rate of
      optical window is 50% (measurement result becomes 0.5 times lower), influence of optical
      window is compensated by changing sensor sensitivity from default 1.0 to 2.0 times

   This device uses I2C bus to communicate, specials pins are required to interface
   Board                                     SDA              SCL              Level
   Uno, Mini, Pro, ATmega168, ATmega328..... A4               A5               5v
   Mega2560................................. 20               21               5v
   Due, SAM3X8E............................. 20               21               3.3v
   Leonardo, Micro, ATmega32U4.............. 2                3                5v
   Digistump, Trinket, Gemma, ATtiny85...... PB0/D0           PB2/D2           3.3v/5v
   Blue Pill*, STM32F103xxxx boards*........ PB7/PB9          PB6/PB8          3.3v/5v
   ESP8266 ESP-01**......................... GPIO0            GPIO2            3.3v/5v
   NodeMCU 1.0**, WeMos D1 Mini**........... GPIO4/D2         GPIO5/D1         3.3v/5v
   ESP32***................................. GPIO21/D21       GPIO22/D22       3.3v
                                             GPIO16/D16       GPIO17/D17       3.3v
                                            *hardware I2C Wire mapped to Wire1 in stm32duino
                                             see https://github.com/stm32duino/wiki/wiki/API#I2C
                                           **most boards has 10K..12K pullup-up resistor
                                             on GPIO0/D3, GPIO2/D4/LED & pullup-down on
                                             GPIO15/D8 for flash & boot
                                          ***hardware I2C Wire mapped to TwoWire(0) aka GPIO21/GPIO22 in Arduino ESP32

   Supported frameworks:
   Arduino Core - https://github.com/arduino/Arduino/tree/master/hardware
   ATtiny  Core - https://github.com/SpenceKonde/ATTinyCore
   ESP8266 Core - https://github.com/esp8266/Arduino
   ESP32   Core - https://github.com/espressif/arduino-esp32
   STM32   Core - https://github.com/stm32duino/Arduino_Core_STM32


   GNU GPL license, all text above must be included in any redistribution,
   see link for details - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/

#include "BH1750FVI.h"


/**************************************************************************/
/*
    Constructor
*/
/**************************************************************************/

BH1750FVI::BH1750FVI(BH1750FVI_ADDRESS addr, BH1750FVI_RESOLUTION res, float sensitivity, float accuracy)
{
  _sensorAddress    = addr;
  _sensorResolution = res;
  _sensitivity      = constrain(sensitivity, BH1750_SENSITIVITY_MIN, BH1750_SENSITIVITY_MAX); //sensitivity range 0.45..3.68
  _accuracy         = constrain(accuracy, BH1750_ACCURACY_MIN, BH1750_ACCURACY_MAX);          //accuracy range 0.96..1.44
  _contMeasurement  = false;                                                                  //false=continuous measurement not started yet
}


/**************************************************************************/
/*
    begin()

    Initialize I2C & sensor

    NOTE:
    - call this function before doing anything else!!!
    - speed in Hz, stretch in usec

    - returned value by "Wire.endTransmission()":
      - 0 success
      - 1 data too long to fit in transmit data buffer
      - 2 received NACK on transmit of address
      - 3 received NACK on transmit of data
      - 4 other error
*/
/**************************************************************************/
#if defined (__AVR__)
bool BH1750FVI::begin(uint32_t speed, uint32_t stretch)
{
  Wire.begin();

  Wire.setClock(speed);                                    //experimental! AVR I2C bus speed 31kHz..400kHz, default 100000Hz

  #if !defined (__AVR_ATtiny85__)                          //for backwards compatibility with ATtiny Core
  Wire.setWireTimeout(stretch, false);                     //experimental! default 25000usec, true=Wire hardware will be automatically reset to default on timeout
  #endif

#elif defined (ESP8266)
bool BH1750FVI::begin(uint8_t sda, uint8_t scl, uint32_t speed, uint32_t stretch)
{
  Wire.begin(sda, scl);

  Wire.setClock(speed);                                    //experimental! ESP8266 I2C bus speed 1kHz..400kHz, default 100000Hz

  Wire.setClockStretchLimit(stretch);                      //experimental! default 150000usec

#elif defined (ESP32)
bool BH1750FVI::begin(int32_t sda, int32_t scl, uint32_t speed, uint32_t stretch) //"int32_t" for Master SDA & SCL, "uint8_t" for Slave SDA & SCL
{
  if (Wire.begin(sda, scl, speed) != true) {return false;} //experimental! ESP32 I2C bus speed ???kHz..400kHz, default 100000Hz

  Wire.setTimeout(stretch / 1000);                         //experimental! default 50msec

#elif defined (ARDUINO_ARCH_STM32)
bool BH1750FVI::begin(uint32_t sda, uint32_t scl, uint32_t speed) //"uint32_t" for pins only, "uint8_t" calls wrong "setSCL(PinName scl)"
{
  Wire.begin(sda, scl);

  Wire.setClock(speed);                                    //experimental! STM32 I2C bus speed ???kHz..400kHz, default 100000Hz

#else
bool BH1750FVI::begin()
{
  Wire.begin();
#endif

  Wire.beginTransmission(_sensorAddress);                  //safety check, make sure the sensor is connected

  if (Wire.endTransmission(true) != 0) {return false;}     //collision on I2C bus, error=sensor didn't return ACK

  setSensitivity(_sensitivity);                            //set sensitivity, see NOTE

  powerDown();                                             //sleep, 1uA

  return true;
}


/**************************************************************************/
/*
    setResolution()

    Set sensor resolution & measurement mode (onetime & sleep, continuous)

    NOTE:
    - for accuracy call before "readLightLevel()"

    - continuous modes:
      - BH1750_CONTINUOUS_HIGH_RES_MODE,   1.00 lux resolution
      - BH1750_CONTINUOUS_HIGH_RES_MODE_2, 0.50 lux resolution
      - BH1750_CONTINUOUS_LOW_RES_MODE,    4.00 lux resolution
    
    - onetime modes:
      - BH1750_ONE_TIME_HIGH_RES_MODE,   1.00 lux resolution
      - BH1750_ONE_TIME_HIGH_RES_MODE_2, 0.50 lux resolution (by default)
      - BH1750_ONE_TIME_LOW_RES_MODE,    4.00 lux resolution

    - possible to detect 0.23 lux in H-resolution  mode at max sesitivity 3.68
    - possible to detect 0.11 lux in H2-resolution mode at max sesitivity 3.68
*/
/**************************************************************************/
void BH1750FVI::setResolution(BH1750FVI_RESOLUTION res)
{
  _sensorResolution = res;
}


/**************************************************************************/
/*
    getResolution()

    Get current sensor sensitivity

    NOTE:
    - see "setResolution" for details
*/
/**************************************************************************/
uint8_t BH1750FVI::getResolution()
{
  return _sensorResolution;
}


/**************************************************************************/
/*
    setSensitivity()

    Set sensor sensitivity

    NOTE:
    - for accuracy call before "readLightLevel()"

    - sensitivity range 0.45..3.68, default 1.00
    - MTreg/measurement time register range 31..254, default 69

    - Sensitivity is used to compensate the influence of the optical filter.
      Any optical filter you put in front of the sensor blocks some light.
      For example, when transmission rate of optical window is 50%
      (measurement result becomes 0.5 times lower), influence of optical
      window is compensated by changing sensor sensitivity from library
      default 1.0 to 2.0 times.

    - With sensitivity it is also possible to detect the light intensity
      from 0.11 lux to 100000 lux, with a loss of sensitivity in the upper
      or lower measurement range, see "readLightLevel()" & "setResolution()"
      for details

    - measurement delay (integration time) depends on sensitivity:
      - 81msec/12Hz..662msec/2Hz at high resolution modes
      - 10msec/100Hz..88msec/11Hz at low resolution mode
*/
/**************************************************************************/
bool BH1750FVI::setSensitivity(float sensitivity)
{
  /* calculate MTreg value */
  sensitivity = constrain(sensitivity, BH1750_SENSITIVITY_MIN, BH1750_SENSITIVITY_MAX); //sensitivity range 0.45..3.68

  uint8_t valueMTreg            = sensitivity * BH1750_MTREG_DEFAULT;                   //calculate MTreg value for new sensitivity, measurement time register range 31..254
  uint8_t measurnentTimeHighBit = valueMTreg;
  uint8_t measurnentTimeLowBit  = valueMTreg;

  /* high bit manipulation */
  measurnentTimeHighBit >>= 5;                                                          //0,0,0,0  0,7-bit,6-bit,5-bit
  measurnentTimeHighBit  |= BH1750_MEASUREMENT_TIME_H;                                  //0,1,0,0  0,7-bit,6-bit,5-bit

  /* low bit manipulation */
  measurnentTimeLowBit <<= 3;                                                           //4-bit,3-bit,2-bit,1-bit  0-bit,0,0,0
  measurnentTimeLowBit >>= 3;                                                           //0,0,0,4-bit  3-bit,2-bit,1-bit,0-bit
  measurnentTimeLowBit  |= BH1750_MEASUREMENT_TIME_L;                                   //0,1,1,4-bit  3-bit,2-bit,1-bit,0-bit

  /* update sensor MTreg register */
  if (_write8(measurnentTimeHighBit) != true) {return false;}                           //collision on I2C bus, error=sensor didn't return ACK
  if (_write8(measurnentTimeLowBit)  != true) {return false;}                           //collision on I2C bus, error=sensor didn't return ACK

  _sensitivity = sensitivity;                                                           //MTreg register update success -> update sensitivity value

  return true;
}


/**************************************************************************/
/*
    getSensitivity()

    Get current sensor sensitivity

    NOTE:
    - see "setSensitivity()" for details
*/
/**************************************************************************/
float BH1750FVI::getSensitivity()
{
  return _sensitivity;
}


/**************************************************************************/
/*
    readLightLevel()

    Read light level, in lux

    NOTE:
    - measurement accuracy ±20%

    - measurement delay (integration time) is very long, so almost any
      noise including light fluctuations of 50Hz/60Hz is rejected by
      the sensor

    - typical measurement range:
      - 1..32767 lux, high resolution mode2 at sensitivity & accuracy 1.0x
      - 1..65535 lux, high resolution mode  at sensitivity & accuracy 1.0x
      - 1..65535 lux, low resolution mode   at sensitivity & accuracy 1.0x
*/
/**************************************************************************/
float BH1750FVI::readLightLevel()
{
  /* send measurement instruction */
  switch(_sensorResolution)                                                   //"switch-case" faster & has smaller footprint than "if-else", see Atmel AVR4027 Application Note
  {
    case BH1750_CONTINUOUS_HIGH_RES_MODE:
    case BH1750_CONTINUOUS_HIGH_RES_MODE_2:
    case BH1750_CONTINUOUS_LOW_RES_MODE:
      if (_contMeasurement != true)                                           //false=continuous measurement not started yet
      {
        if   (_write8(_sensorResolution) == true) {_contMeasurement = true;}  //measurement result continuously updated, no need to call measurement instruction any more
        else                                      {return BH1750_ERROR;}      //collision on I2C bus, error=sensor didn't return ACK
      }
      break;

    case BH1750_ONE_TIME_HIGH_RES_MODE:
    case BH1750_ONE_TIME_HIGH_RES_MODE_2:
    case BH1750_ONE_TIME_LOW_RES_MODE:
      if   (_write8(_sensorResolution) == true)   {_contMeasurement = false;}
      else                                        {return BH1750_ERROR;}      //collision on I2C bus, error=sensor didn't return ACK
      break;
  }

  /* set measurement delay */
  switch(_sensorResolution)
  {
    case BH1750_CONTINUOUS_HIGH_RES_MODE:
    case BH1750_CONTINUOUS_HIGH_RES_MODE_2:
    case BH1750_ONE_TIME_HIGH_RES_MODE:
    case BH1750_ONE_TIME_HIGH_RES_MODE_2:
      delay(_sensitivity * 180);                                              //integration time = (0.45..3.68) * 120..180msec -> 81msec/12Hz..662msec/2Hz (default 180msec/5Hz)
      break;

    case BH1750_CONTINUOUS_LOW_RES_MODE:
    case BH1750_ONE_TIME_LOW_RES_MODE:
      delay(_sensitivity * 24);                                               //integration time = (0.45..3.68) * 16..24msec -> 10msec/100Hz..88msec/11Hz (default 24msec/42Hz)
      break;
  }

  /* read measurement result */                                               //result arter power-up & reset 0x0000
  Wire.requestFrom(_sensorAddress, (uint8_t)2, (uint8_t)true);                //read 2-bytes to "wire.h" rxBuffer, true=send stop after transmission

  if (Wire.available() != 2) {return BH1750_ERROR;}                           //check "wire.h" rxBuffer, error=received data smaller than expected

  uint16_t rawLightLevel  = Wire.read() << 8;                                 //read MSB-byte from "wire.h" rxBuffer 
           rawLightLevel |= Wire.read();                                      //read LSB-byte from "wire.h" rxBuffer

  /* light level calculation, p.11 */
  float lightLevel;

  switch (_sensorResolution)
  {
    case BH1750_ONE_TIME_HIGH_RES_MODE_2:
    case BH1750_CONTINUOUS_HIGH_RES_MODE_2:
      lightLevel = 0.5 * (float)rawLightLevel / _accuracy * _sensitivity;     //0.50 lux resolution but smaller measurement range
      break;

    case BH1750_ONE_TIME_LOW_RES_MODE:
    case BH1750_ONE_TIME_HIGH_RES_MODE:
    case BH1750_CONTINUOUS_LOW_RES_MODE:
    case BH1750_CONTINUOUS_HIGH_RES_MODE:
      lightLevel = (float)rawLightLevel / _accuracy * _sensitivity;           //1.00 lux & 4.00 lux resolution
      break;

    default:
      lightLevel = BH1750_ERROR;
      break;
  }

  return lightLevel;
}


/**************************************************************************/
/*
    powerDown()

    Put sensor to sleep

    NOTE:
    - sleep current 1uA
*/
/**************************************************************************/
void BH1750FVI::powerDown()
{
  if (_write8(BH1750_POWER_DOWN) == true) {_contMeasurement = false;}
}


/**************************************************************************/
/*
    powerOn()

    Wakes up sensor from sleep

    NOTE:
    - wake-up & wait for measurment command
    - possible to omit by calling measurement instruction, see "readLightLevel()"
    - ADDR, SDA, SCL are unstable without 1usec delay
*/
/**************************************************************************/
void BH1750FVI::powerOn()
{
  _write8(BH1750_POWER_ON);

  delayMicroseconds(1);    //see NOTE
}


/**************************************************************************/
/*
    reset()

    Reset sensor

    NOTE:
    - not accepted in power-down mode
    - used for removing previous measurement, reset only illuminance
      data register
    - ADDR, SDA, SCL are unstable without 1usec delay
*/
/**************************************************************************/
void BH1750FVI::reset()
{
  _write8(BH1750_RESET);

  delayMicroseconds(1); //see NOTE
}


/**************************************************************************/
/*
    setCalibration()

    Set sensor calibration value

    NOTE:
    - accuracy range 0.96..1.44

    - typical accuracy values:
      - 1.00, fluorescent light
      - 1.06, white LED & artifical sun
      - 1.15, halogen light
      - 1.18, krypton light light
      - 1.20, incandescent light (by default)

    - accuracy = sensor output lux / actual lux
*/
/**************************************************************************/
void BH1750FVI::setCalibration(float accuracy)
{
  _accuracy = constrain(accuracy, BH1750_ACCURACY_MIN, BH1750_ACCURACY_MAX); //accuracy range 0.96..1.44
}


/**************************************************************************/
/*
    getCalibration()

    Return sensor calibration value

    NOTE:
    - see "setCalibration()" for details
*/
/**************************************************************************/
float BH1750FVI::getCalibration()
{
  return _accuracy;
}


/**************************************************************************/
/*
    Write 8-bits value over I2C

    NOTE:
    - returned value by "Wire.endTransmission()":
      - 0 success
      - 1 data too long to fit in transmit data buffer
      - 2 received NACK on transmit of address
      - 3 received NACK on transmit of data
      - 4 other error
*/
/**************************************************************************/
bool BH1750FVI::_write8(uint8_t value)
{
  Wire.beginTransmission(_sensorAddress);

  Wire.write(value);
  
  return (Wire.endTransmission(true) == 0); //true=success, false=collision on I2C bus
}
