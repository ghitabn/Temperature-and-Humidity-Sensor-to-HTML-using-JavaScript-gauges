# Temperature and Humidity Sensor to HTML using JavaScript gauges

## 1. Description

The setup allows periodic readings from a DHT11 temperature and humidity sensor using an Arduino Uno development board and an Ethernet shield. Sensor readings (temperature and humidity) as well as computed values (heat indexes) are sent to the client’s browser and displayed using JavaScript gauges. The webpage is hosted on a SD card, data refresh in the client browser is performed through AJAX and XML.

Difficulty level: intermediate.

## 2. Parts

* 1 x Arduino Uno R3;
* 1 x Arduino Ethernet shield (W5100);
* 1 x DHT11;
* 1 x micro-SD card;
* Breadboard + wires.

## 3. Schematics

![Temperature and Humidity Sensor to HTML using JavaScript gauges schem!](/res/schem.jpg "Temperature and Humidity Sensor to HTML using JavaScript gauges schem")

## 4. Assembly (breadboard)

![Temperature and Humidity Sensor to HTML using JavaScript gauges bboard!](/res/bboard.jpg "Temperature and Humidity Sensor to HTML using JavaScript gauges bboard")

## 5. Code

The code implements a minimalist web server. It hosts a single web page (stored and loaded from the SD card) used to display useful data (sensor readings + computed values). The webpage data is updated using AJAX; an XML file containing the values to display is periodically sent from the Arduino to the client web browser. Data is displayed using JavaScript gauges.

Note. The Gauge component is written by Mykhailo Stadnyk (ref. https://github.com/Mikhus/canvas-gauges, https://wwvalue.com/web-dev/pure-javascript-html5-canvas-gauge-canvgauge)

* /src/index.htm – to be placed in the mini-SD card
* /src/gaugeDHT11.ino – to be uploaded to Arduino

Note. In order to avoid SRAM memory saturation, static strings are stored  in flash memory (usage of “F()” in some Serial.println statements).

## 6. Demo

Youtube link:  [MQTT-Node-with-Arduino-Uno-And-W5100-Ethernet-Shield](https://www.youtube.com/watch?v=4xpr895dyb4)

## 7. Additional resources

* Libraries:
    * Adafruit Unified Sensor Driver:
    https://github.com/adafruit/Adafruit_Sensor
    * DHT-sensor-library:
    https://github.com/adafruit/DHT-sensor-library
    * SPI, Ethernet and SD standard libraries (included with the Arduino IDE)

* References:
    * DHT11 datasheet: https://cdn-learn.adafruit.com/downloads/pdf/dht.pdf
    * Arduino Ethernet shield (WT5100) datasheet:
    http://www.mouser.com/catalog/specsheets/A000056_DATASHEET.pdf
    * Arduino Ethernet Shield Web Server Tutorial:
    https://startingelectronics.org/tutorials/arduino/ethernet-shield-web-server-tutorial/
    * The Gauge component: https://github.com/Mikhus/canvas-gauges, https://wwvalue.com/web-dev/pure-javascript-html5-canvas-gauge-canvgauge