[![license-badge][]][license] ![version] [![stars][]][stargazers] [![hit-count][]][count] [![github-issues][]][issues]

# ROHM BH1750FVI
This is an Arduino library for ROHM BH1750FVI Ambient Light Sensor

- Default range:         1 - 65'535 lux
- Measurement accuracy:  ±20%, possible to calibrate
- Peak wave length:      560nm, yellow-green
- Maximun I²C bus speed: 400KHz
- Power supply voltage:  2.4 - 3.6v, absolute maximum 4.5v

Supports all sensors features:

- Changing Sensitivity**
- Changing Resolution
- Read Illuminance*
- Power Down, 1μA
- Power Up, 190μA
- Reset, clears previous measurement
- Calibration

Tested on:

- Arduino AVR
- Arduino ESP8266
- Arduino STM32

*Library returns 0, if there is a communication error has occurred

**Any optical filter you put in front of the sensor blocks some light. **Sensitivity is used to compensate the influence of the optical filter**. For example, when transmission rate of optical window is 50% (measurement result becomes 0.5 times lower), influence of optical window is compensated by changing sensor sensitivity from default 1.0 to 2.0 times.

[license-badge]: https://img.shields.io/badge/License-GPLv3-blue.svg
[license]:       https://choosealicense.com/licenses/gpl-3.0/
[version]:       https://img.shields.io/badge/Version-1.2.6-green.svg
[stars]:         https://img.shields.io/github/stars/enjoyneering/BH1750FVI.svg
[stargazers]:    https://github.com/enjoyneering/BH1750FVI/stargazers
[hit-count]:     http://hits.dwyl.io/enjoyneering/BH1750FVI.svg
[count]:         http://hits.dwyl.io/enjoyneering/BH1750FVI/badges
[github-issues]: https://img.shields.io/github/issues/enjoyneering/BH1750FVI.svg
[issues]:        https://github.com/enjoyneering/BH1750FVI/issues/
