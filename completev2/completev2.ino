#include <iostrean.h>
#include <sstream.h>
#include <SPI.h>
#include <util/crc16.h>
#include <SoftwareSerial.h>

// GPS
SoftwareSerial GPS(4, 5);
byte gps_set_sucess = 0 ;

// RADIO
#define RADIOPIN 9
char datastring[200];

float pressure;
float temperature;
void read_scp(float &pressure, float &temperature);

void setup()
{
  // This is the serial rate for your terminal program. It must be this 
  // fast because we need to print everything before a new sentence 
  // comes in. If you slow it down, the messages might not be valid and 
  // you will likely get checksum errors.
  Serial.begin(9600);

  init_scp();
  Serial.println("Started...waiting");
  init_gps();
  // Enable radio output
  pinMode(RADIOPIN,OUTPUT);

  // give the sensor time to set up:
  delay(100);
}

void loop()
{
  delay(1000);
  Serial.println("here1");
  Serial.flush();
  read_scp(pressure, temperature);
  Serial.println("here1.5");
  sendText("Foo");
  while(GPS.available()) {
    Serial.write(GPS.read()); 
  }
}





