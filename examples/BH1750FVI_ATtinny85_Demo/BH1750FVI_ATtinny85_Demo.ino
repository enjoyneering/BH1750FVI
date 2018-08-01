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
#pragma GCC optimize ("Os")    //code optimisation controls - "O2" & "O3" code performance, "Os" code size

#include <Wire.h>
#include <BH1750FVI.h>
#include <LiquidCrystal_I2C.h> //https://github.com/enjoyneering/LiquidCrystal_I2C

#define LCD_ROWS         4     //qnt. of lcd rows
#define LCD_COLUMNS      20    //qnt. of lcd columns

#define LCD_SPACE_SYMBOL 0x20  //space symbol from the lcd ROM, see p.9 of GDM2004D datasheet

#define HIGH_DETAIL_WORK 2000  //recommended indoor light level for high detail work, 1500  - 2000 lx
#define PLANTS_GROW      12000 //recommended indoor light level for plants growing,   12000 – 20000 lx
#define PLANTS_FLOWER    35000 //recommended indoor light level for plants flowering, 35000 – 40000 lx

#define LED              1     //connect led to ATtiny85 pin no.6 in series with 470 Ohm resistor

float   lightLevel = 0;

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
BH1750FVI         myBH1750(BH1750_DEFAULT_I2CADDR, BH1750_CONTINUOUS_HIGH_RES_MODE_2, BH1750_SENSITIVITY_DEFAULT, BH1750_ACCURACY_DEFAULT);
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);


void setup()
{
  pinMode(LED, OUTPUT);

  /* LCD connection check */ 
  while (lcd.begin(LCD_COLUMNS, LCD_ROWS) != true)                //20x4 display
  {
    for (uint8_t i = 0; i > 5; i++)                               //3 blinks if PCF8574/LCD is not connected or lcd pins declaration is wrong
    {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);
      delay(500);
    }
  }

  lcd.print(F("PCF8574 is OK"));                                  //(F()) saves string to flash & keeps dynamic memory free
  delay(2000);

  lcd.clear();

  /* BH1750 connection check */
  while (myBH1750.begin() != true)
  {
    lcd.setCursor(0, 0);
    lcd.print(F("BH1750 error"));
    delay(5000);
  }

  lcd.clear();

  lcd.print(F("BH1750 OK"));
  delay(2000);

  lcd.clear();

  /* prints static text */
  lcd.print(F("Light:"));

  lcd.setCursor(0, 1);                                            //set 1-st colum & 2-nd row, first colum & row started at zero
  lcd.print(F("Power:"));
}

void loop()
{
  lightLevel = myBH1750.readLightLevel();

  /* prints dynamic text */
  lcd.setCursor(6, 0);
  lcd.print(lightLevel);
  lcd.print(F("lx"));
  lcd.write(LCD_SPACE_SYMBOL);
  lcd.write(LCD_SPACE_SYMBOL);

  lcd.setCursor(6, 1);
  lcd.print(lightLevel/683);
  lcd.print(F("Watt/m^2"));                                       //power for 555nm wave
  lcd.write(LCD_SPACE_SYMBOL);
  lcd.write(LCD_SPACE_SYMBOL);

  /* prints horizontal graph from 0 to HIGH_DETAIL_WORK */
  lcd.printHorizontalGraph('W', 2, lightLevel, HIGH_DETAIL_WORK); //name of the bar, row, current value, max. value
  lcd.printHorizontalGraph('G', 3, lightLevel, PLANTS_GROW);      //name of the bar, row, current value, max. value

  delay(1000);
}
