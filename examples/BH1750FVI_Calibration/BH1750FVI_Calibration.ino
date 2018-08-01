/***************************************************************************************************/
/* 
  Example for ROHM BH1750FVI Ambient Light Sensor library

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

  myBH1750.setCalibration(BH1750_ACCURACY_DEFAULT);     //back to default 1.20

  delay(5000);
}
