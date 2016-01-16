/**************************************************************************/
/* 
  Example of ROHM BH1750FVI Ambient Light Sensor library usage.
  
  These displays use I2C to communicate, 2 pins are required to  
  interface

  Connect BH1750FVI to pins :  SDA  SCL
  Uno, Redboard, Pro:          A4   A5
  Mega2560, Due:               20   21
  Leonardo:                    2    3
  
  BSD license, all text above must be included in any redistribution
*/
/**************************************************************************/

#include <Wire.h>
#include <BH1750FVI.h>

/*
BH1750FVI(address, resolution, sensitivity)

address:
BH1750_DEFAULT_I2CADDR (when address pin LOW)
BH1750_SECOND_I2CADDR  (when address pin HIGH)

resolution:
BH1750_CONTINUOUS_HIGH_RES_MODE   - Continuous measurement @ 1.0 lx res. mode. Integration time 120ms
BH1750_CONTINUOUS_HIGH_RES_MODE_2 - Continuous measurement @ 0.5 lx res. mode. Integration time 120ms
BH1750_CONTINUOUS_LOW_RES_MODE 	  - Continuous measurement @ 4.0 lx res. mode. Integration time 16ms

BH1750_ONE_TIME_HIGH_RES_MODE   - One-time (Power down after measurement) @ 1.0 lx res. mode. Integration time 120ms
BH1750_ONE_TIME_HIGH_RES_MODE_2 - One-time (Power down after measurement) @ 0.5 lx res. mode. Integration time 120ms
BH1750_ONE_TIME_LOW_RES_MODE 	- One-time (Power down after measurement) @ 4.0 lx res. mode. Integration time 16ms

sensitivity:
NOTE: your value have to be between min. and max.
1.00 - default Sensitivity
0.45 - minimum Sensitivity
3.68 - maximum Sensitivity

DEFAULT
BH1750FVI(BH1750_DEFAULT_I2CADDR, BH1750_CONTINUOUS_HIGH_RES_MODE_2, 1.00)
*/

BH1750FVI myBH1750; // or myBH1750FVI(PUT address HERE, PUT resolution HERE, PUT sensitivity HERE)

void setup()
{
  Serial.begin(115200);
  
  while (myBH1750.begin() != true)
  {
    Serial.println(F(""));
    Serial.println(F("ROHM BH1750FVI Ambient Light Sensor is not present"));
    delay(1000);
  }
}

void loop()
{
  Serial.println(F(""));
  Serial.println(F("DEMO: Default settings"));
  
  Serial.print(F("Light level: "));
  Serial.print(myBH1750.readLightLevel());
  Serial.println(F(" lx"));
  
  Serial.print(F("Power for 555nm: "));
  Serial.print(myBH1750.readLightLevel()/683);
  Serial.println(F(" Watt/m^2"));

  myBH1750.setSensitivity(2);
  
  Serial.println(F("DEMO: Sensitivity - 2.0. Resolution - 0.5 lx. Continuous Mode (no auto power down after the measurement)"));
  Serial.print(F("Light level: "));
  Serial.print(myBH1750.readLightLevel());
  Serial.println(F(" lx"));
 
  myBH1750.setSensitivity(0.5);
  
  Serial.println(F("DEMO: Sensitivity - 0.5. Resolution - 0.5 lx. Continuous Mode (no auto power down after the measurement)"));
  Serial.print(F("Light level: "));
  Serial.print(myBH1750.readLightLevel());
  Serial.println(F(" lx"));
  
  myBH1750.setSensitivity(3.68);
  myBH1750.setResolution(BH1750_ONE_TIME_HIGH_RES_MODE);
  
  Serial.println(F("DEMO: Max. Sensitivity - 3.68. Resolution - 1.0 lx. Auto Power Down (after the measurement)"));
  Serial.print(F("Light level: "));
  Serial.print(myBH1750.readLightLevel());
  Serial.println(F(" lx"));
  
  myBH1750.setSensitivity(0.45);
  myBH1750.setResolution(BH1750_CONTINUOUS_HIGH_RES_MODE_2);
  
  Serial.println(F("DEMO: Min. Sensitivity - 0.45. Resolution - 0.5 lx. Continuous Mode (no auto power down after measurement)"));
  Serial.print(F("Light level: "));
  Serial.print(myBH1750.readLightLevel());
  Serial.println(F(" lx"));
  
  /* back to default sensitivity */
  myBH1750.setSensitivity(1);
  
  Serial.println(F("DEMO: Manual Power Down"));
  myBH1750.powerDown();
  
  
  Serial.println(F("DEMO: Start over again in 5 sec."));
  delay(5000);
}
