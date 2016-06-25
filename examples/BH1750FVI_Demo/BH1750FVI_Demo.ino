/**************************************************************************/
/* 
  Example of ROHM BH1750FVI Ambient Light Sensor library usage.
  
  These sensor uses I2C to communicate. Two pins are required to  
  interface.

  Connect BH1750FVI to pins:  SDA     SCL
  Uno, Mini, Pro:             A4      A5
  Mega2560, Due:              20      21
  Leonardo:                   2       3
  Atiny85:                    0/PWM   2/A1   (TinyWireM)
  NodeMCU 1.0:                D1/ANY  D2/ANY (D1 & D2 by default)
  ESP8266 ESP-01:             ANY     ANY
  
  BSD license, all text above must be included in any redistribution
*/
/**************************************************************************/
#if defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny26__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny45__) || (__AVR_ATtiny84__) || defined(__AVR_ATtiny85__)
 #include <TinyWireM.h>
 #define Wire TinyWireM
#else defined
 #include <Wire.h>
#endif

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

BH1750_ONE_TIME_HIGH_RES_MODE     - One-time measurement with power down after. Resolution - 1.0 lx.
BH1750_ONE_TIME_HIGH_RES_MODE_2   - One-time measurement with power down after. Resolution - 0.5 lx.
BH1750_ONE_TIME_LOW_RES_MODE      - One-time measurement with power down after. Resolution - 4.0 lx.

sensitivity:
NOTE: your value have to be between min. and max.
1.00 - default Sensitivity
0.45 - minimum Sensitivity
3.68 - maximum Sensitivity

DEFAULT
BH1750FVI(BH1750_DEFAULT_I2CADDR, BH1750_CONTINUOUS_HIGH_RES_MODE_2, 1.00)
*/

BH1750FVI myBH1750;

void setup()
{
  Serial.begin(115200);

 #if defined(ARDUINO_ARCH_ESP8266) || (ESP8266_NODEMCU)
  while (myBH1750.begin(D1, D2) != true)
 #else
  while (myBH1750.begin() != true)
 #endif
  {
    Serial.println(F(""));
    Serial.println(F("ROHM BH1750FVI ambient light sensor is not present..."));
    delay(5000);
  }

  Serial.println(F("ROHM BH1750FVI ambient light sensor is present"));
}

void loop()
{
  Serial.println(F(""));
  Serial.println(F("DEMO: Sensitivity - 1.0, Continuous Mode (default settings)"));
  Serial.print(F("Light level: "));
  Serial.print(myBH1750.readLightLevel());
  Serial.println(F(" +-0.5 lx"));
  Serial.print(F("Power for 555nm wave: "));
  Serial.print(myBH1750.readLightLevel()/683);
  Serial.println(F(" Watt/m^2"));
  Serial.println(F(""));
  
  Serial.println(F("DEMO: Manual Power Down"));
  myBH1750.powerDown();
  Serial.println(F(""));

  Serial.println(F("DEMO: Sensitivity - 2.0, Continuous Mode"));
  myBH1750.setSensitivity(2);
  Serial.print(F("Light level: "));
  Serial.print(myBH1750.readLightLevel());
  Serial.println(F(" +-0.5 lx"));
  Serial.println(F(""));
 
  Serial.println(F("DEMO: Sensitivity - 0.5, Continuous Mode"));
  myBH1750.setSensitivity(0.5);
  Serial.print(F("Light level: "));
  Serial.print(myBH1750.readLightLevel());
  Serial.println(F(" +-0.5 lx"));
  Serial.println(F(""));
  
  Serial.println(F("DEMO: Sensitivity - 3.68, Auto Power Down"));
  myBH1750.setSensitivity(3.68);
  myBH1750.setResolution(BH1750_ONE_TIME_HIGH_RES_MODE);
  Serial.print(F("Light level: "));
  Serial.print(myBH1750.readLightLevel());
  Serial.println(F(" +-1.0 lx"));
  Serial.println(F(""));
  
  
  Serial.println(F("DEMO: Sensitivity - 0.45, Auto Power Down"));
  myBH1750.setSensitivity(0.45);
  myBH1750.setResolution(BH1750_CONTINUOUS_HIGH_RES_MODE_2);
  Serial.print(F("Light level: "));
  Serial.print(myBH1750.readLightLevel());
  Serial.println(F(" +-0.5 lx"));
  Serial.println(F(""));
  
  Serial.println(F("DEMO: Sensitivity - 1.0, Auto Power Down"));
  myBH1750.setSensitivity(1);
  myBH1750.setResolution(BH1750_ONE_TIME_HIGH_RES_MODE_2);
  Serial.print(F("Light level: "));
  Serial.print(myBH1750.readLightLevel());
  Serial.println(F(" +-0.5 lx"));
  Serial.println(F(""));
  
  Serial.print(F("DEMO: Starts over again in 8 sec."));
  Serial.println(F(""));
  delay(8000);
