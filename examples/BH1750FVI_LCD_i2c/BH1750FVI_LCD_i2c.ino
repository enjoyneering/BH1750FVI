/***************************************************************************************************/
/* 
  Example for ROHM BH1750FVI Ambient Light Sensor library

  Default range: 1 - 65'535 lx

  written by : enjoyneering79
  sourse code: https://github.com/enjoyneering/

  This sketch uses I2C bus to communicate, specials pins are required to interface
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
#include <Wire.h>                //https://github.com/enjoyneering/ESP8266-I2C-Driver
#include <BH1750FVI.h>
#include <LiquidCrystal_I2C.h>   //https://github.com/enjoyneering/LiquidCrystal_I2C

#define LCD_ROWS           4     //qnt. of lcd rows
#define LCD_COLUMNS        20    //qnt. of lcd columns

#define LCD_SPACE_SYMBOL   0x20  //lcd build-in space symbol

#define LIGHT_LEVEL_OFFICE 500   //normal office light level, lx
#define PLANTS_GROW        12000 //recommended indoor light level for plants growing, 12000 – 20000 lx

const uint8_t in_power_two[8] PROGMEM = {0x1C, 0x04, 0x08, 0x10, 0x1C, 0x00, 0x00, 0x00}; //PROGMEM saves variable to flash & keeps dynamic memory free

float lightLevel = 0;

/*
BH1750FVI(address, resolution, sensitivity)

BH1750_DEFAULT_I2CADDR            - address pin LOW
BH1750_SECOND_I2CADDR             - address pin HIGH

BH1750_CONTINUOUS_HIGH_RES_MODE   - continuous measurement, 1.0 lx resolution
BH1750_CONTINUOUS_HIGH_RES_MODE_2 - continuous measurement, 0.5 lx resolution
BH1750_CONTINUOUS_LOW_RES_MODE    - continuous measurement, 0.5 lx resolution
BH1750_ONE_TIME_HIGH_RES_MODE     - one measurement & power down, 1.0 lx resolution
BH1750_ONE_TIME_HIGH_RES_MODE_2   - one measurement & power down, 0.5 lx resolution
BH1750_ONE_TIME_LOW_RES_MODE      - one measurement & power down, 4.0 lx resolution

sensitivity                       - value have to be between 0.45 - 3.68, default 1.00
*/
BH1750FVI         myBH1750(BH1750_DEFAULT_I2CADDR, BH1750_CONTINUOUS_HIGH_RES_MODE_2, 1.00);
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);


void setup()
{
  Serial.begin(115200);
  Serial.println();

  /* LCD connection check */  
  while (lcd.begin(LCD_COLUMNS, LCD_ROWS, LCD_5x8DOTS) != true) //20x4 display, 5x8 pixels size
  {
    Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal.")); //PROGMEM saves variable to flash & keeps dynamic memory free
    delay(5000);
  }

  lcd.print(F("PCF8574 is OK"));
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

  /* load custom symbol to CGRAM */
  lcd.createChar(0, in_power_two);

  /* prints static text */
  lcd.setCursor(0, 0);
  lcd.print(F("Light:")); 

  lcd.setCursor(0, 1);
  lcd.print(F("Power:"));
}

void loop()
{
  lightLevel = myBH1750.readLightLevel();

  lcd.setCursor(6, 0);
  lcd.print(lightLevel);
  lcd.print(F("lx"));
  lcd.write(LCD_SPACE_SYMBOL);
  lcd.write(LCD_SPACE_SYMBOL);

  lcd.setCursor(6, 1);
  lcd.print(lightLevel/683);
  lcd.print(F("Watt/m"));
  lcd.write(0);                                                     //print custom char "in_power_two
  lcd.write(LCD_SPACE_SYMBOL);
  lcd.write(LCD_SPACE_SYMBOL);

  lcd.printHorizontalGraph('W', 2, lightLevel, LIGHT_LEVEL_OFFICE); //name of the bar, row, current value, max. value
  lcd.printHorizontalGraph('G', 3, lightLevel, PLANTS_GROW);        //name of the bar, row, current value, max. value

  delay(2000);
}
