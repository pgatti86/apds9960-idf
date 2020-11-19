# apds9960-idf

Porting of Sparkfun APDS-9960 Arduino library for esp idf framework.
The original project can be found at this [link](https://github.com/sparkfun/SparkFun_APDS-9960_Sensor_Arduino_Library)

I added an extra class (I2cDevice.h) that you can use to port this library to another system. All i2c specific platform code is now isolated.

This project is based on Espressif IDF v4.0 (stable branch at writing time).
Follow this link to configure your environment: [esp idf v4.0](https://docs.espressif.com/projects/esp-idf/en/v4.0/get-started/index.html)

Note: depending on your python virtualenv version you may encounter an error while running ESP install script (install.sh).
To solve simply remove the invalid option (--no-site-packages) from idf_tools.py 

## examples

See main.cpp for examples.

### connection

You can run 'make menuconfig' to change i2c gpio.

| Esp32 Pin | APDS-9960 Board | Function |
|---|---|---| 
| 3.3V | VCC | Power |
| GND | GND | Ground |
| 23 | SDA | I2C Data |
| 22 | SCL | I2C Clock |
| 2 | INT | Interrupt |
