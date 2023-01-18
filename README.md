[![license-badge][]][license] ![version] [![stars][]][stargazers] ![hit-count] [![github-issues][]][issues]

# ROHM BH1750FVI
This is an Arduino library for ROHM BH1750FVI Ambient Light Sensor

- power supply voltage +2.4v..+3.6v
- maximum current 190μA, sleep current 1μA
- I²C bus speed 100KHz..400KHz, up to 2 sensors on the bus
- maximum sensitivity at 560nm, yellow-green light
- 50Hz/60Hz flicker reduction
- measurement accuracy ±20%
- measurement angle ±30°
- calibration by changing the accuracy 0.96..1.44 **(1)**
- optical filter compensation by changing sensitivity 0.45..3.68 **(2)** **(6)**
- onetime+sleep and continuous measurement mode
- typical measurement resolution: **(3)**
  - 0.5 lux, high resolution mode2
  - 1.0 lux, high resolution mode
  - 4.0 lux, low resolution mode
- typical measurement range from 1..32767 lux to 1..65535 lux **(4)** **(5)**
- typical measurement interval from 81..662 msec to 10..88 msec **(6)**

Supports all sensors features:
- set sensitivity **(2)**
- set resolution
- calibration **(1)**
- read illuminance in lux **(5)**
- set measurement mode (high/low resolution and onetime/continuous measurement) **(3)**
- sleep, 1μA
- reset (clears previous measurement, not accepted in sleep mode)

Tested on:
- Arduino AVR
- Arduino ESP8266
- Arduino STM32

**(1)** Typical calibration values:<br>
    - 1.00, fluorescent light<br>
    - 1.06, white LED & artifical sun<br>
    - 1.15, halogen light<br>
    - 1.18, krypton light<br>
    - 1.20, incandescent light (by default)<br>
**(2)** Any optical filter you put in front of the sensor blocks some light. **Sensitivity is used to compensate the influence of the optical filter**. For example, when transmission rate of optical window is 50% (measurement result becomes 0.5 times lower), influence of optical window is compensated by changing sensor sensitivity from library default 1.0 to 2.0 times. With this function it is also possible to detect the light intensity from 0.11 lux to 100000 lux, with a loss of sensitivity in the upper or lower measurement range, respectively.<br>
**(3)** Use "high resolution mode" or "high resolution mode 2" if you want to detect light intensity less than 10 lux.<br>
**(4)** Depends on resolution mode, sensitivity and accuracy. The "high resolution mode 2" mode cuts the measurement range in half. Values greater than 1.0x reduce the measurement range, while smaller values increase it.<br>
**(5)** Library returns 4294967295.00 lux if a communication error occurs.<br>
**(6)** Depends on resolution mode and sensitivity. High resolutions increase measurement interval. Sensitivity values less than 1.0x decrease the measurement interval, while larger values increase it.<br>

[license-badge]: https://img.shields.io/badge/License-GPLv3-blue.svg
[license]:       https://choosealicense.com/licenses/gpl-3.0/
[version]:       https://img.shields.io/badge/Version-1.3.0-green.svg
[stars]:         https://img.shields.io/github/stars/enjoyneering/BH1750FVI.svg
[stargazers]:    https://github.com/enjoyneering/BH1750FVI/stargazers
[hit-count]:     https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Fenjoyneering%2FBH1750FVI&count_bg=%2379C83D&title_bg=%23555555&icon=&icon_color=%23E7E7E7&title=hits&edge_flat=false
[github-issues]: https://img.shields.io/github/issues/enjoyneering/BH1750FVI.svg
[issues]:        https://github.com/enjoyneering/BH1750FVI/issues/
