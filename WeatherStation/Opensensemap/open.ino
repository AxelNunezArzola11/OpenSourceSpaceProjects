/*AUTHOR: AXEL NÚÑEZ ARZOLA
 SPACE PROJECTS
 THIS A  WEATHER STATION THAT MEASURES AIR QUALITY AND GREEN HOUSE GASES
 AND IMAGINE THAT YOU CAN SE THIS INFORMATION IN A SPECIFIC PLACE IN ADIGITAL MAP
 LIKE GOOGLE MAPS BUT FOR AIR POLLUTION. THIS USES ESP8266 +MQ SENSORS + MCP3008*/
#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <MCP3008.h>
// Wifi Configuration
const char* server = "ingress.opensensemap.org";
WiFiClient client;
//WIFI CREDENTIALS
const char* ssid = "";
const char* password = "";
//A STRUCT
typedef struct sensor {
    const uint8_t ID[12];
} sensor;

uint8_t sensorsIndex = 0;

// Number of sensors
static const uint8_t NUM_SENSORS = 4;
// senseBox ID and sensor IDs
const uint8_t SENSEBOX_ID[12] = { 0x00, 0xa0, 0xf0, 0xe8, 0xa8, 0x77, 0xb3, 0x00, 0x2b, 0x4b, 0xc7, 0xe6 };

// Do not change order of sensor IDs
const sensor sensors[NUM_SENSORS] = {
  // CO2
  { 0x00, 0xa0, 0xf0, 0xe0, 0xa5, 0x87, 0xb4, 0x00, 0x1b, 0x6b, 0xc8, 0xea },
  // 03
  { 0x00, 0xa0, 0xf0, 0xe0, 0xa3, 0x87, 0xb4, 0x00, 0x1b, 0x6b, 0xc8, 0xec },
  // CH4
  { 0x00, 0xa0, 0xf0, 0xe0, 0xa3, 0x97, 0xb4, 0x00, 0x1b, 0x6b, 0xc8, 0xed },
  // RAYOS UV
  { 0x00, 0xa0, 0xf0, 0xe0, 0xa3, 0x97, 0xb4, 0x00, 0x1b, 0x6b, 0xc8, 0xee }
};
uint8_t contentLength = 0;
float values[NUM_SENSORS];
#define OLED_RESET 4
#define CS_PIN 15
#define CLOCK_PIN 14
#define MOSI_PIN 13
#define MISO_PIN 12
MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);
const unsigned int postingInterval = 60000;
void setup(){

  Serial.begin(9600);
  // start the Wifi connection:
  Serial.println("SenseBox Home software version 2.1");
  Serial.println();
  Serial.print("Starting wifi connection...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
Serial.println("done!");
Serial.println("Starting loop.\n");
}
void loop(){
  float val = adc.readADC(0); // read Chanel 0 from MCP3008 ADC
float sensorco2volt=val/1024*5.0;
addvalue(sensorco2volt);

float val2 = adc.readADC(1); // read Chanel 0 from MCP3008 ADC
float sensoro3volt=val2/1024*5.0;
addvalue(sensoro3volt);

float val3 = adc.readADC(3); // read Chanel 0 from MCP3008 ADC
float sensormetavolt=val3/1024*5.0;

addvalue(sensormetavolt);
float val4 = adc.readADC(4); // read Chanel 0 from MCP3008 ADC
float sensorUVvolt=val4/1024*5.0;

addvalue(sensorUVvolt);
submitValues();
sleep(postingInterval);
  }
void addvalue(const float& value) {
values[sensorsIndex] = value;
sensorsIndex = sensorsIndex + 1;
}
int printHexToStream(const uint8_t* data,
                     uint8_t length,
                     Print& stream)  // prints 8-bit data in hex
{
  byte first;
  int j = 0;
  for (uint8_t i = 0; i < length; i++) {
    first = (data[i] >> 4) | 48;
    if (first > 57) {
      stream.write(first + (byte)39);
    } else {
      stream.write(first);
    }
    j++;

    first = (data[i] & 0x0F) | 48;
    if (first > 57) {
      stream.write(first + (byte)39);
    } else {
      stream.write(first);
    }
    j++;
  }
  return j;
}

int printCsvToStream(Print& stream) {
  int len = 0;
  for (uint8_t i = 0; i < sensorsIndex; i++) {
    if (!isnan(values[i])) {
      len = len + printHexToStream(sensors[i].ID, 12, stream);
      len = len + stream.print(",");
      // do not print digits for illuminance und uv-intensity
      if (i < 3)
        len = len + stream.println(values[i]);
      else
        len = len + stream.println(values[i], 0);
    }
  }
  return len;
}

// millis() rollover fix -
// http://arduino.stackexchange.com/questions/12587/how-can-i-handle-the-millis-rollover
void sleep(unsigned long ms) {     // ms: duration
  unsigned long start = millis();  // start: timestamp
  for (;;) {
    unsigned long now = millis();         // now: timestamp
    unsigned long elapsed = now - start;  // elapsed: duration
    if (elapsed >= ms)                    // comparing durations: OK
      return;
  }
}

void waitForResponse() {
  // if there are incoming bytes from the server, read and print them
  sleep(100);
  String response = "";
  char c;
  boolean repeat = true;
  do {
    if (client.available())
      c = client.read();
    else
      repeat = false;
    response += c;
    if (response == "HTTP/1.1 ")
      response = "";
    if (c == '\n')
      repeat = false;
  } while (repeat);

  Serial.print("Server Response: ");
  Serial.print(response);

  client.flush();
  client.stop();
}

void submitValues() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  Serial.println("__________________________\n");
  if (client.connected()) {
    client.stop();
    sleep(1000);
  }
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP POST request:

    client.print(F("POST /boxes/"));
    printHexToStream(SENSEBOX_ID, 12, client);
    client.println(F("/data HTTP/1.1"));

    // !!!!! DO NOT REMOVE !!!!!
   
    // print once to Serial to get the content-length
    int contentLen = printCsvToStream(Serial);
    // !!!!! DO NOT REMOVE !!!!!
   

    // Send the required header parameters
    client.print(F("Host: "));
    client.println(server);
    client.print(
        F("Content-Type: text/csv\nConnection: close\nContent-Length: "));
    client.println(contentLen);
    client.println();
    printCsvToStream(client);
    client.println();
    Serial.println("done!");

    waitForResponse();

    // reset index
    sensorsIndex = 0;
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed. Restarting System.");
    sleep(5000);
    ESP.restart();
  }
}
