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
#pragma GCC optimize ("Os")    //code optimisation controls - "O2" & "O3" code performance, "Os" code size

#include <Wire.h>
#include <BH1750FVI.h>
#include <LiquidCrystal_I2C.h> //https://github.com/enjoyneering/LiquidCrystal_I2C


#define LCD_COLUMNS             20    //LCD columns
#define LCD_ROWS                4     //LCD rows
#define LCD_SPACE_SYMBOL        0x20  //space symbol located in LCD ROM, see p.9 of GDM2004D datasheet

#define LIGHT_LEVEL_OFFICE      200   //recommended office light level 200..500 lux
#define LIGHT_LEVEL_PLANT_GROW  12000 //recommended indoor light level for plants growing 12000..20000 lux
#define LIGHT_LEVEL_FLOWER_GROW 12000 //recommended indoor light level for plants growing 35000..40000 lux


BH1750FVI         myBH1750(BH1750_DEFAULT_I2CADDR, BH1750_CONTINUOUS_HIGH_RES_MODE, BH1750_SENSITIVITY_DEFAULT, BH1750_ACCURACY_DEFAULT);
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);


/**************************************************************************/
/*
    setup()

    Main setup
*/
/**************************************************************************/
void setup()
{
  /* LCD initialization */ 
  while (lcd.begin(LCD_COLUMNS, LCD_ROWS, LCD_5x8DOTS) != true) //colums, rows, characters size
  {
    delay(5000);
  }

  /* BH1750 initialization */ 
  while (myBH1750.begin() != true)
  {
    lcd.setCursor(0, 0);
    lcd.print(F("BH1750 error"));

    delay(5000);
  }

  lcd.clear();
  lcd.print(F("BH1750 OK"));

  delay(2000);

  /* prints static text to LCD */
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Light:")); 

  lcd.setCursor(0, 1);
  lcd.print(F("Power:"));
}

/**************************************************************************/
/*
    loop()

    Main loop
*/
/**************************************************************************/
void loop()
{
  float lightLevel = myBH1750.readLightLevel();                           //start measurment -> wait for result -> read result -> retrun result or 4294967295 if communication error is occurred

  lcd.setCursor(6, 0);                                                    //set 7-th colum & 1-st row
  lcd.print(lightLevel);
  lcd.print(F("lx"));
  lcd.write(LCD_SPACE_SYMBOL);                                            //"lcd.write(0x20)" is faster than "lcd.print(" ")"
  lcd.write(LCD_SPACE_SYMBOL);

  lcd.setCursor(6, 1);                                                    //set 7-th colum & 2-nd row
  lcd.print(lightLevel / 683);                                            //power for 555nm wave
  lcd.print(F("Watt/m^2"));
  lcd.write(LCD_SPACE_SYMBOL);
  lcd.write(LCD_SPACE_SYMBOL);

  if ((LCD_ROWS) == 4)
  {
    lcd.printHorizontalGraph('W', 2, lightLevel, LIGHT_LEVEL_OFFICE);     //graph name, row position, current value, maximum value
    lcd.printHorizontalGraph('G', 3, lightLevel, LIGHT_LEVEL_PLANT_GROW); //graph name, row position, current value, maximum value
  }

  delay(1000);
}
