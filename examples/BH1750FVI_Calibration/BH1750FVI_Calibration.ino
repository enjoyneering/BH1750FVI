/***************************************************************************************************/
/* 
  Example for ROHM BH1750FVI Ambient Light Sensor library

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

                                           *STM32F103xxxx pins PB6/PB7 are 5v tolerant, but
                                            bi-directional logic level converter is recommended

  Frameworks & Libraries:
  ATtiny Core           - https://github.com/SpenceKonde/ATTinyCore
  ESP8266 Core          - https://github.com/esp8266/Arduino
  ESP8266 I2C lib fixed - https://github.com/enjoyneering/ESP8266-I2C-Driver
  STM32 Core            - https://github.com/rogerclarkmelbourne/Arduino_STM32

  GNU GPL license, all text above must be included in any redistribution, see link below for details:
  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
#include <Wire.h>
#include <BH1750FVI.h>

float calibrationValue = 0;

/*
BH1750FVI(address, resolution, sensitivity, accuracy)

BH1750_DEFAULT_I2CADDR            - address pin LOW
BH1750_SECOND_I2CADDR             - address pin HIGH

BH1750_CONTINUOUS_HIGH_RES_MODE   - continuous measurement, 1.0 lx resolution
BH1750_CONTINUOUS_HIGH_RES_MODE_2 - continuous measurement, 0.5 lx resolution
BH1750_CONTINUOUS_LOW_RES_MODE    - continuous measurement, 0.5 lx resolution
BH1750_ONE_TIME_HIGH_RES_MODE     - one measurement & power down, 1.0 lx resolution
BH1750_ONE_TIME_HIGH_RES_MODE_2   - one measurement & power down, 0.5 lx resolution
BH1750_ONE_TIME_LOW_RES_MODE      - one measurement & power down, 4.0 lx resolution

sensitivity                       - value have to be between 0.45 - 3.68, default 1.00 or use macros BH1750_SENSITIVITY_DEFAULT
accuracy                          - value have to be between 0.96 - 1.44, default 1.20 or use macros BH1750_ACCURACY_DEFAULT
*/
BH1750FVI myBH1750(BH1750_DEFAULT_I2CADDR, BH1750_CONTINUOUS_HIGH_RES_MODE_2, BH1750_SENSITIVITY_DEFAULT, BH1750_ACCURACY_DEFAULT);


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

void loop()
{
  calibrationValue  = random(96, 144);
  calibrationValue /= 100;                              //calibration value range 0.96 - 1.44, typical - 1.20


  Serial.println(F("............"));
  Serial.print(F("Light......:"));
  Serial.print(myBH1750.readLightLevel());
  Serial.println(F(" lx"));

  myBH1750.setCalibration(calibrationValue);

  Serial.print(F("Calibration:"));
  Serial.println(myBH1750.getCalibration());

  Serial.print(F("Light......:"));
  Serial.print(myBH1750.readLightLevel());
  Serial.println(F(" lx"));

  myBH1750.setCalibration(BH1750_MEASUREMENT_ACCURACY); //back to default 1.20

  delay(5000);
}
