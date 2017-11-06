/***************************************************************************************************/
/* 
  Example for ROHM BH1750FVI Ambient Light Sensor library
  
  written by : enjoyneering79
  sourse code: https://github.com/enjoyneering/

  Wide range:           1 - 65'535 lx (by default)
  Possible to detect:   0.11 - 100'000 lx by changing Sensitivity

  This sensor uses I2C bus to communicate, specials pins are required to interface

  Connect chip to pins:    SDA        SCL
  Uno, Mini, Pro:          A4         A5
  Mega2560, Due:           20         21
  Leonardo:                2          3
  ATtiny85:                0(5)       2/A1(7)   (ATTinyCore  - https://github.com/SpenceKonde/ATTinyCore
                                                 & TinyWireM - https://github.com/SpenceKonde/TinyWireM)
  ESP8266 ESP-01:          GPIO0/D5   GPIO2/D3  (ESP8266Core - https://github.com/esp8266/Arduino)
  NodeMCU 1.0:             GPIO4/D2   GPIO5/D1
  WeMos D1 Mini:           GPIO4/D2   GPIO5/D1

  BSD license, all text above must be included in any redistribution
*/
/***************************************************************************************************/
#include <TinyWireM.h>
#include <BH1750FVI.h>
#include <LiquidCrystal_I2C.h> //https://github.com/enjoyneering/LiquidCrystal_I2C

#define LCD_ROWS         4     //qnt. of lcd rows
#define LCD_COLUMNS      20    //qnt. of lcd columns
#define HIGH_DETAIL_WORK 2000  //recommended indoor light level for high detail work, 1500-2000 lx
#define PLANTS_GROW      12000 //recommended indoor light level for plants growing,   12000 – 20000 lx
#define PLANTS_FLOWER    35000 //recommended indoor light level for plants flowering, 35000 – 40000 lx
#define LED              1     //connect led to ATtiny85 pin no.6 in series with 470 Ohm resistor

float   lightLevel;

/*
BH1750FVI(address, resolution, sensitivity)

address:
BH1750_DEFAULT_I2CADDR (when address pin LOW)
BH1750_SECOND_I2CADDR  (when address pin HIGH)

resolution:
BH1750_CONTINUOUS_HIGH_RES_MODE   - Continuous measurement. Resolution - 1.0 lx.
BH1750_CONTINUOUS_HIGH_RES_MODE_2 - Continuous measurement. Resolution - 0.5 lx.
BH1750_CONTINUOUS_LOW_RES_MODE    - Continuous measurement. Resolution - 4.0 lx.

BH1750_ONE_TIME_HIGH_RES_MODE     - One-time measurement(power down after). Resolution - 1.0 lx.
BH1750_ONE_TIME_HIGH_RES_MODE_2   - One-time measurement(power down after). Resolution - 0.5 lx.
BH1750_ONE_TIME_LOW_RES_MODE      - One-time measurement(power down after). Resolution - 4.0 lx.

sensitivity:
NOTE: your value have to be between min. and max.
1.00 - default Sensitivity
0.45 - minimum Sensitivity
3.68 - maximum Sensitivity
*/
BH1750FVI         myBH1750(BH1750_DEFAULT_I2CADDR, BH1750_CONTINUOUS_HIGH_RES_MODE_2, 1.00);
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);


void setup()
{
  pinMode(LED, OUTPUT);

  /* LCD connection check */ 
  while (lcd.begin(LCD_COLUMNS, LCD_ROWS) != true)                //20x4 display
  {
    for (uint8_t i = 0; i > 3; i++)                               //3 blinks if PCF8574/LCD is not connected or lcd pins declaration is wrong
    {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);
      delay(500);
    }
    delay(2000);
  }
  lcd.print("PCF8574 is OK");
  delay(1000);
  lcd.clear();

  /* BH1750 connection check */
  while (myBH1750.begin() != true)
  {
    lcd.print("BH1750 error");
    delay(5000);
    lcd.setCursor(0, 0);
  }
  lcd.setCursor(0, 0);
  lcd.print("BH1750 OK   ");
  delay(2000);
  lcd.clear();

  /* prints static text */
  lcd.print("Light:");

  lcd.setCursor(0, 1);                                            //set 1-st colum & 2-nd row. NOTE: first colum & row started at zero
  lcd.print("Power:");
}


void loop()
{
  lightLevel = myBH1750.readLightLevel();

  /* prints dynamic text */
  lcd.setCursor(6, 0);
  lcd.print(lightLevel);     lcd.print("lx ");

  lcd.setCursor(6, 1);
  lcd.print(lightLevel/683); lcd.print("Watt/m^2 ");              //power for 555nm wave

  /* prints horizontal graph from 0 to HIGH_DETAIL_WORK */
  lcd.printHorizontalGraph('W', 2, lightLevel, HIGH_DETAIL_WORK); //name of the bar, row, current value, max. value
  lcd.printHorizontalGraph('G', 3, lightLevel, PLANTS_GROW);      //name of the bar, row, current value, max. value

  delay(1000);
}
