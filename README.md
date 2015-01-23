# BH1750FVI
This is an Arduino library for the ROHM BH1750FVI Ambient Light Sensor

Supports all sensors features:
- Changing Sensitivity
- Changing Resolution
- Read Illuminance
- Power Down
- Reset

ROHM BH1750FVI uses I2C to communicate, 2 pins are required to interface

Connect BH1750FVI to pins :  SDA  SCL
- Uno, Redboard, Pro:          A4   A5
- Mega2560, Due:               20   21
- Leonardo:                    2    3
