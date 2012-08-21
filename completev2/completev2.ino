#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include<iostrean.h>
#include<sstream.h>
#include <SPI.h>

// Cell Shield
SoftwareSerial cell(2,3);
char mobilenumber[] = "07733676867";  // Replace xxxxxxxx with the recipient's mobile number

// GPS
SoftwareSerial uart_gps(8, 9);
TinyGPS gps;

float pressure;
float temperature;
float latitude;
float longitude;
void transmit(TinyGPS &gps, float &pressure, float &temperature);
void read_scp(float &pressure, float &temperature);

void setup()
{
  // This is the serial rate for your terminal program. It must be this 
  // fast because we need to print everything before a new sentence 
  // comes in. If you slow it down, the messages might not be valid and 
  // you will likely get checksum errors.
  Serial.begin(115200);
  cell.begin(9600);
  uart_gps.begin(4800);

  init_scp();
  Serial.println("Started...waiting");
  for(int i = 0; i < 40; i++)
  {
    delay(1000);
    Serial.println(i + 1);
  }
  Serial.println("");
  Serial.println("GPS Shield QuickStart Example Sketch v12");
  Serial.println("       ...waiting for lock...           ");
  Serial.println("");


  // give the sensor time to set up:
  delay(100);
  uart_gps.listen();
}

// This is the main loop of the code. All it does is check for data on 
// the RX pin of the ardiuno, makes sure the data is valid NMEA sentences, 
// then jumps to the getgps() function.
void loop()
{
  delay(10000);
   Serial.println("here1");
  read_scp(pressure, temperature);
   Serial.println("here1.5");
  if(uart_gps.available())     // While there is data on the RX pin...
  {
     Serial.println("here2");
    int c = uart_gps.read();    // load the data into a variable...
    if(gps.encode(c))      // if there is a new valid sentence...
    Serial.println("here2.5");
    {
      transmit(gps, pressure, temperature);
   Serial.println("here3");  
    }
  }
}



