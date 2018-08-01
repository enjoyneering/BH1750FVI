# ROHM BH1750FVI
This is an Arduino library for ROHM BH1750FVI Ambient Light Sensor

- Default range:        1 - 65'535 lux
- Measurement accuracy: ±20%, possible to calibrate
- Peak wave length:     560nm, yellow-green
- Power supply voltage: 2.4 - 3.6v

Supports all sensors features:

- *Changing Sensitivity
- Changing Resolution
- Read Illuminance
- Power Down
- Power Up
- Reset
- Calibration

Tested on:

- Arduino AVR
- Arduino ESP8266
- Arduino STM32

*This option is used to compensate the influence of the optical filter. For example, when transmission rate of optical window is 50% (measurement result becomes 0.5 times lower), influence of optical window is ignored by changing sensor sensitivity from default 1.0 to 2.0 times
