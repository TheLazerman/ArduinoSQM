<h1>Arduino SQM </h1>

Arduino based SQM (Sky Quality Meter) using the TSL2591 luminosity sensor and the accompanying library written by Gabe Shaughnessy.
https://github.com/gshau/SQM_TSL2591

This program calculates the SQM in mpsas (Magnitude per square arcsecond) and displays the result. Upon pressing the button, it will take an average over ~5 seconds (may take longer depending on the number of photons recieved by the sensor) and display both the average SQM and the resulting NELM (Naked Eye Limiting Magnitude).

This code assumes you are using a SSD1306 I2C LCD display and a button attached to a valid interrupt pin (I am using pin 7 for the Arduino Pro Micro)

<h2>Librarys</h2>

In addition to the SQM_TSL2591 library you will need the Adafruit-GFX-Library-master and the Adafruit_SSD1306-master Library to run the SSD1306 OLED Display.

TSL2591 Library:
https://github.com/gshau/SQM_TSL2591

Adafruit Sensor Library:
https://github.com/adafruit/Adafruit_Sensor

