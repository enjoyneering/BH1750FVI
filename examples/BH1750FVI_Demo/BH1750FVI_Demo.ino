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
#include <Wire.h>
#include <BH1750FVI.h>

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
BH1750FVI myBH1750(BH1750_DEFAULT_I2CADDR, BH1750_CONTINUOUS_HIGH_RES_MODE_2, 1.00);


void setup()
{
  Serial.begin(115200);
  Serial.flush();

  while (myBH1750.begin() != true)
  {
    Serial.println(F("ROHM BH1750FVI ambient light sensor is not present"));
    delay(5000);
  }
  Serial.println(F("ROHM BH1750FVI ambient light sensor is present"));
}


void loop()
{
  /* DEMO - 1 */
  Serial.println(F("DEMO 1: Sensitivity - 1.0, Continuous High Res II Mode"));
  Serial.print(F("Light level.........: ")); Serial.print(myBH1750.readLightLevel());     Serial.println(F(" +-0.5 lx"));
  Serial.print(F("Power for 555nm wave: ")); Serial.print(myBH1750.readLightLevel()/683); Serial.println(F(" Watt/m^2"));
  myBH1750.reset();
  /* DEMO - 2 */
  Serial.println(F("DEMO 2: Sensitivity - 2.00, Continuous High Res II Mode"));
  myBH1750.setSensitivity(2);

  Serial.print(F("Light level.........: ")); Serial.print(myBH1750.readLightLevel());     Serial.println(F(" +-0.5 lx"));
   /* DEMO - 3 */
  Serial.println(F("DEMO 3: Sensitivity - 0.50, Continuous High Res II Mode"));
  myBH1750.setSensitivity(0.5);

  Serial.print(F("Light level.........: ")); Serial.print(myBH1750.readLightLevel());     Serial.println(F(" +-0.5 lx"));
  /* DEMO - 4 */
  Serial.println(F("DEMO 4: Sensitivity - 3.68, Power Down High Res   Mode"));
  myBH1750.setSensitivity(3.68);
  myBH1750.setResolution(BH1750_ONE_TIME_HIGH_RES_MODE);

  Serial.print(F("Light level.........: ")); Serial.print(myBH1750.readLightLevel());     Serial.println(F(" +-1.0 lx")); 
  /* DEMO - 5 */
  Serial.println(F("DEMO 5: Sensitivity - 0.45, Power Down High Res II Mode"));
  myBH1750.setSensitivity(0.45);
  myBH1750.setResolution(BH1750_CONTINUOUS_HIGH_RES_MODE_2);

  Serial.print(F("Light level.........: ")); Serial.print(myBH1750.readLightLevel());     Serial.println(F(" +-0.5 lx"));
  /* DEMO - 6 */
  Serial.println(F("DEMO 6: Sensitivity - 1.00, Power Down High Res II Mode"));
  myBH1750.setSensitivity(1);
  myBH1750.setResolution(BH1750_ONE_TIME_HIGH_RES_MODE_2);

  Serial.print(F("Light level.........: ")); Serial.print(myBH1750.readLightLevel());     Serial.println(F(" +-0.5 lx"));
  /* DEMO - 7 */ 
  Serial.println(F("DEMO 7: Manual Power Down"));
  myBH1750.powerDown();
  /* DEMO - END */
  Serial.print(F("DEMO: Starts over again in 5 sec."));
  delay(5000);
  
  /* back to lib. default resolution */
  myBH1750.setResolution(BH1750_CONTINUOUS_HIGH_RES_MODE_2);
}
