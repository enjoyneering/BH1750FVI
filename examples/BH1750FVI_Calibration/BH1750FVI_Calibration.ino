/***************************************************************************************************/
/* 
   Example for ROHM BH1750FVI Ambient Light Sensor library

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
   - calibration by changing the accuracy 0.96..1.44
   - typical measurement range depends on resolution mode sensitivity & accuracy values:
     - from 1..32767 to 1..65535 lux
   - typical measurement interval depends on resolution mode & sensitivity:
     - from 81..662 msec to 10..88 msec

   This device uses I2C bus to communicate, specials pins are required to interface
   Board                                     SDA              SCL              Level
   Uno, Mini, Pro, ATmega168, ATmega328..... A4               A5               5v
   Mega2560................................. 20               21               5v
   Due, SAM3X8E............................. 20               21               3.3v
   Leonardo, Micro, ATmega32U4.............. 2                3                5v
   Digistump, Trinket, Gemma, ATtiny85...... PB0/D0           PB2/D2           3.3v/5v
   Blue Pill*, STM32F103xxxx boards*........ PB9/PB7          PB8/PB6          3.3v/5v
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
#include <Wire.h>
#include <BH1750FVI.h>


/**************************************************************************/
/*
    BH1750FVI(address, resolution, sensitivity, accuracy)

    NOTE:
    - address:
      - BH1750_DEFAULT_I2CADDR, address pin LOW (by default)
      - BH1750_SECOND_I2CADDR, address pin HIGH

    - resolution:
      - BH1750_CONTINUOUS_HIGH_RES_MODE,   continuous measurement  with 1.00 lux resolution
      - BH1750_CONTINUOUS_HIGH_RES_MODE_2, continuous measurement  with 0.50 lux resolution
      - BH1750_CONTINUOUS_LOW_RES_MODE,    continuous measurement  with 0.50 lux resolution

      - BH1750_ONE_TIME_HIGH_RES_MODE,     one measurement & sleep with 1.00 lux resolution (by default)
      - BH1750_ONE_TIME_HIGH_RES_MODE_2,   one measurement & sleep with 0.50 lux resolution
      - BH1750_ONE_TIME_LOW_RES_MODE,      one measurement & sleep with 4.00 lux resolution

    - optical filter compensation by changing sensitivity 0.45..3.68 (by default 1.00, no optical filter in front of the sensor)

    - calibration by changing the accuracy 0.96..1.44:
      - typical accuracy values:
        - 1.00, fluorescent light
        - 1.06, white LED & artifical sun
        - 1.15, halogen light
        - 1.18, krypton light light
        - 1.20, incandescent light (by default)
*/
/**************************************************************************/
BH1750FVI myBH1750(BH1750_DEFAULT_I2CADDR, BH1750_ONE_TIME_HIGH_RES_MODE, BH1750_SENSITIVITY_DEFAULT, BH1750_ACCURACY_DEFAULT);


/**************************************************************************/
/*
    setup()

    Main setup
*/
/**************************************************************************/
void setup()
{
  Serial.begin(115200);
  Serial.println();

  while (myBH1750.begin() != true)
  {
    Serial.println(F("ROHM BH1750FVI is not present")); //(F()) saves string to flash & keeps dynamic memory free

    delay(5000);
  }

  Serial.println(F("ROHM BH1750FVI is present"));
}


/**************************************************************************/
/*
    loop()

    Main loop
*/
/**************************************************************************/
void loop()
{
  for (uint8_t i = 0; i < 5; i++)
  {
    setLightType(i);                           //call before "readLightLevel()"

    Serial.println();
    Serial.print(F("Calibration: "));
    Serial.println(myBH1750.getCalibration());

    Serial.print(F("Light......: "));
    Serial.print(myBH1750.readLightLevel());   //start measurment -> wait for result -> read result -> retrun result or 4294967295 if communication error is occurred
    Serial.println(F(" lux"));
  }

  delay(10000);
}


/**************************************************************************/
/*
    setLightType()

    Calibration by changing the accuracy

    NOTE:
    - accuracy range 0.96..1.44:
      - valid argumet:
        - 0, fluorescent light
        - 1, white LED & artifical sun
        - 2, halogen light
        - 3, krypton light light
        - 4, incandescent light (by default)
*/
/**************************************************************************/
void setLightType(uint8_t lightType)
{
  switch (lightType)
  {
    case 0:
      myBH1750.setCalibration(1.00); //fluorescent light
      break;

    case 1:
      myBH1750.setCalibration(1.06); //white LED & artifical sun
      break;

    case 2:
      myBH1750.setCalibration(1.15); //white LED & artifical sun
      break;

    case 3:
      myBH1750.setCalibration(1.18); //krypton light light
      break;


    case 4:
      myBH1750.setCalibration(1.20); //incandescent light
      break;

    default:
      myBH1750.setCalibration(1.00); //fluorescent light
      break;
  }
}
