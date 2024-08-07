/*------------------------------------------------------------------------------ 
Program:      gaugeDHT11 

Description:  periodic readings from a DHT11 temperature and humidity sensor 
              using an Arduino Uno development board and an Ethernet shield. 
              Sensor readings (temperature and humidity)could be accessed via 
			  a minimalist webpage hosted on a SD card. The webpage refreshes 
			  sensor data using AJAX and XML.

Hardware:     Arduino Uno R3, Arduino Ethernet shield (WT5100), DHT11. Should work 
              with other Arduinos 

Software:     Developed using Arduino 1.8.2 IDE

Libraries:    
              - Adafruit Unified Sensor Driver: 
                https://github.com/adafruit/Adafruit_Sensor
              - DHT-sensor-library: 
                https://github.com/adafruit/DHT-sensor-library
              - SPI, Ethernet and SD standard libraries (included with the Arduino IDE)

References: 
              - DHT11 datasheet: http://www.micropik.com/PDF/dht11.pdf
              - Arduino Ethernet shield (WT5100) datasheet: 
                http://www.mouser.com/catalog/specsheets/A000056_DATASHEET.pdf
              - Arduino Ethernet Shield Web Server Tutorial:
              https://startingelectronics.org/tutorials/arduino/ethernet-shield-web-server-tutorial/
			  - JavaScript gauge: https://github.com/Mikhus/canvas-gauges, 
				https://wwvalue.com/web-dev/pure-javascript-html5-canvas-gauge-canvgauge 

Date:         April 25, 2017

Author:       Gheorghe G., http://www.arduinolab.net
------------------------------------------------------------------------------*/
#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>
#include <SD.h>

// sensor pin and type
const int sensorPin = 3;
const int sensorType = DHT11;
DHT dht(sensorPin, sensorType);

// controller MAC address - use your own values
byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};
// Controller IP address
IPAddress ip(192, 168, 2, 105);

// initialize Ethernet server library
EthernetServer server(80);

// size of the buffer used to capture HTTP requests
#define REQ_BUF_SZ   50

File webFile;                     // the web page file on the SD card
char HTTP_req[REQ_BUF_SZ] = {0};  // buffered HTTP request stored as null terminated string
char req_index = 0;               // index into HTTP_req buffer

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // initialize SD card
  Serial.println(F("initializing SD card..."));
  if (!SD.begin(4)) {
    Serial.println(F("SD card initialization failed"));
    return;
  }
  Serial.println(F("SD card initialized"));

  // check for index.htm file
  if (!SD.exists("index.htm")) {
    Serial.println(F("can't find index.htm file!"));
    return;  // can't find index file
  }
  Serial.println(F("found index.htm file."));

  // start the Ethernet connection and the server
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print(F("server is at "));
  Serial.println(Ethernet.localIP());

  dht.begin(); // initialize sensor
}

void loop()
{
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client)
  {
    Serial.println(F("new client"));
    // any http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())             // client data available to read
      {
        char c = client.read();           // read 1 byte (character) from client
        // buffer first part of HTTP request in HTTP_req array (string)
        // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
        if (req_index < (REQ_BUF_SZ - 1)) {
          HTTP_req[req_index] = c;        // save HTTP request character
          req_index++;
        }
        // last line of client request is blank and ends with \n
        // respond to client only after last line received
        if (c == '\n' && currentLineIsBlank)
        {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          // remainder of header follows below, depending on if
          // web page or XML page is requested
          // Ajax request - send XML file
          if (StrContains(HTTP_req, "ajax_inputs")) {
            // send rest of HTTP header
            client.println("Content-Type: text/xml");
            client.println("Connection: keep-alive");
            client.println();
            // send XML file containing input states
            XML_response(client);
          }
          else {  // web page request
            // send rest of HTTP header
            client.println("Content-Type: text/html");
            client.println("Connection: keep-alive");
            client.println();
            // send web page
            webFile = SD.open("index.htm");        // open web page file
            if (webFile) {
              while (webFile.available()) {
                client.write(webFile.read()); // send web page to client
              }
              webFile.close();
            }
          }
          // display received HTTP request on serial port
          Serial.print(HTTP_req);
          // reset buffer index and all buffer elements to 0
          req_index = 0;
          StrClear(HTTP_req, REQ_BUF_SZ);
          break;
        }
        if (c == '\n') {
          // last character on line of received text
          // starting new line with next character read
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // a text character was received from client
          currentLineIsBlank = false;
        }
      } // end if (client.available())
    }// end while (client.connected())
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println(F("client disconnected"));
  }
}

// send the XML file with values
void XML_response(EthernetClient cl)
{
  Serial.println("XML_response call");
  boolean validValues = true;

  // sensor-specific readings
  float humidity = dht.readHumidity(); // humidity
  float tempC = dht.readTemperature(); // temperature (Celsius)
  float tempF = dht.readTemperature(true); // temperature (Fahrenheit)

  // check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(tempC) || isnan(tempF))
  {
    validValues = false;
  }
  else
  {
    cl.print("<?xml version = \"1.0\" ?>");
    cl.print("<inputs>");
    cl.print("<value1>");
    if (validValues) {
      Serial.print(F("Humidity [%]:    "));
      Serial.println(humidity);
      cl.print(humidity);
    }
    else {
      cl.print("invalid data");
    }
    cl.print("</value1>");
    cl.print("<value2>");
    if (validValues) {
      Serial.print(F("Temperature [C]: "));
      Serial.println(tempC);
      cl.print(tempC);
    }
    else {
      cl.print("invalid data");
    }
    cl.print("</value2>");

    cl.print("<value3>");
    if (validValues) {
      Serial.print(F("Temperature [F]: "));
      Serial.println(tempF);
      cl.print(tempF);
    }
    else {
      cl.print("invalid data");
    }
    cl.print("</value3>");

    cl.print("</inputs>");
  }
}

// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
  for (int i = 0; i < length; i++) {
    str[i] = 0;
  }
}

// searches for the string sfind in the string str
// returns 1 if string found, 0 if string not found
char StrContains(char *str, char *sfind)
{
  char found = 0;
  char index = 0;
  char len;

  len = strlen(str);

  if (strlen(sfind) > len) {
    return 0;
  }
  while (index < len) {
    if (str[index] == sfind[found]) {
      found++;
      if (strlen(sfind) == found) {
        return 1;
      }
    }
    else {
      found = 0;
    }
    index++;
  }
  return 0;
}
