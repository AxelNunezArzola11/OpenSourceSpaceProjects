/*AUTHOR: AXEL NÚÑEZ ARZOLA
 SPACE PROJECTS
 THIS A  WEATHER STATION THAT MEASURES AIR QUALITY AND GREEN HOUSE GASES
 THE ESP8266 CONNECTS TO WIFI AND THEN UPLOAD THIS DATA TO AND IOT SERVER
 "CAYENNE". YOU CAN SEE THE DATA IN GRAPHS ALSO IN NUMERIC VALUE.
 THE USER CAN CHECK THIS DATA IN A WEB PAGE AND IN A APP
 FINALLY THE USER CAN CREATE TRIGGERS WHEN THE LEVELS OF AIR POLLUUTION ARE HIGHERS.
  THIS USES ESP8266 +MQ SENSORS + MCP3008+display i2c*/
 
#include <SPI.h>

#include <Wire.h>

#include <CayenneMQTTESP8266.h>

#define CAYENNE_PRINT Serial

#include <Adafruit_GFX.h>

#include <Adafruit_SSD1306.h>

#include <DHT.h>

#include <Adafruit_Sensor.h>

#include <MCP3008.h>
//put your wifi credentials you can also use your cellphone as a wifi point access
char ssid[] = ""; 

char wifiPassword[] = ""; 

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.

char username[] = "";

char password[] = "";

char clientID[] = "";

#define DHTPIN 16 // what pin we're connected to

#define DHTTYPE DHT11 // DHT 11

#define OLED_RESET 4

#define CS_PIN 15

#define CLOCK_PIN 14

#define MOSI_PIN 13

#define MISO_PIN 12

Adafruit_SSD1306 display(OLED_RESET);

MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);

// Initialize DHT sensor for normal 16mhz Arduino

DHT dht(DHTPIN, DHTTYPE);

int sda=2;

int scl=0;

void setup()

{

Wire.begin( sda,scl);

dht.begin(); // initialize dht

display.begin(SSD1306_SWITCHCAPVCC, 0x3C);// initialize with the I2C addr 0x3C (for the 128x32)(initializing the display)

Serial.begin(9600);

Cayenne.begin(username, password, clientID, ssid, wifiPassword);

}

void loop()

{

Cayenne.loop();

delay(10000);

delay(1000);

displayTempHumid();

display.display();

delay(2000);

display.clearDisplay();

CO2();

display.display();

delay(2000);

display.clearDisplay();

O3();

display.display();

delay(2000);

display.clearDisplay();

metano();

display.display();

delay(2000);

display.clearDisplay();

Rayosuv();

display.display();

delay(2000);

display.clearDisplay();

}

void CO2(){

float val = adc.readADC(0); // read Chanel 0 from MCP3008 ADC

float sensorco2volt=val/1024*5.0;

display.clearDisplay();

display.setTextColor(WHITE);

display.setTextSize(1);

display.setCursor(0,0);

display.print("CO2: ");

display.print(sensorco2volt);

display.print("PPM");

Cayenne.virtualWrite(2,sensorco2volt);

}

void O3(){

float val2 = adc.readADC(1); // read Chanel 0 from MCP3008 ADC

float sensoro3volt=val2/1024*5.0;

display.clearDisplay();

display.setTextColor(WHITE);

display.setTextSize(1);

display.setCursor(0,0);

display.print("O3: ");

display.print(sensoro3volt);

display.print("PPM");

Cayenne.virtualWrite(3,sensoro3volt);

}

void metano(){

float val3 = adc.readADC(3); // read Chanel 0 from MCP3008 ADC

float sensormetavolt=val3/1024*5.0;

display.clearDisplay();

display.setTextColor(WHITE);

display.setTextSize(1);

display.setCursor(0,0);

display.print(": metano");

display.print(sensormetavolt);

display.print("PPM");

Cayenne.virtualWrite(4,sensormetavolt);

}

void Rayosuv(){

float val4 = adc.readADC(4); // read Chanel 0 from MCP3008 ADC

float sensorUVvolt=val4/1024*5.0;

display.clearDisplay();

display.setTextColor(WHITE);

display.setTextSize(1);

display.setCursor(0,0);

display.print("Rayosuv: ");

display.print(sensorUVvolt);

display.print("PPM");

Cayenne.virtualWrite(5,sensorUVvolt);

}

void displayTempHumid(){

delay(2000);

// Reading temperature or humidity takes about 250 milliseconds!

// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

float h = dht.readHumidity();

// Read temperature as Celsius

float t = dht.readTemperature();

// Read temperature as Fahrenheit

float f = dht.readTemperature(true);

// Check if any reads failed and exit early (to try again).

if (isnan(h) || isnan(t) || isnan(f)) {

display.clearDisplay(); // clearing the display

display.setTextColor(WHITE); //setting the color

display.setTextSize(1); //set the font size

display.setCursor(5,0); //set the cursor coordinates

display.print("Failed to read from DHT sensor!");

return;

}

display.clearDisplay();

display.setTextColor(WHITE);

display.setTextSize(1);

display.setCursor(0,0);

display.print("Humidity: ");

display.print(h);

Cayenne.celsiusWrite(1,h);

display.print(" %\t");

display.setCursor(0,10);

display.print("Temperature: ");

display.print(t);

Cayenne.celsiusWrite(0,t);

display.print(" C");

display.setCursor(0,20);

display.print("Temperature: ");

display.print(f);

display.print(" F");

}
