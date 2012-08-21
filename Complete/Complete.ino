/*
  6-8-10
  Aaron Weiss
  SparkFun Electronics
  
  Example GPS Parser based off of arduiniana.org TinyGPS examples.
  
  Parses NMEA sentences from an EM406 running at 4800bps into readable 
  values for latitude, longitude, elevation, date, time, course, and 
  speed. 
  
  For the SparkFun GPS Shield. Make sure the switch is set to DLINE.
  
  Once you get your longitude and latitude you can paste your 
  coordinates from the terminal window into Google Maps. Here is the 
  link for SparkFun's location.  
  http://maps.google.com/maps?q=40.06477,+-105.20997
  
  Uses the NewSoftSerial library for serial communication with your GPS, 
  so connect your GPS TX and RX pin to any digital pin on the Arduino, 
  just be sure to define which pins you are using on the Arduino to 
  communicate with the GPS module. 
  
  REVISIONS:
  1-14-12 Arduino 1.0+ Compatible
  1-17-11 
    changed values to RXPIN = 2 and TXPIN = to correspond with
    hardware v14+. Hardware v13 used RXPIN = 3 and TXPIN = 2.
  
*/ 

// In order for this sketch to work, you will need to download 
// TinyGPS library from arduiniana.org and put them 
// into the hardware->libraries folder in your ardiuno directory.
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <iostrean.h>
#include <sstream.h>
#include <SPI.h>

// Define which pins you will use on the Arduino to communicate with your 
// GPS. In this case, the GPS module's TX pin will connect to the 
// Arduino's RXPIN which is pin 3.
#define RXPIN 2
#define TXPIN 3
//Set this value equal to the baud rate of your GPS
#define GPSBAUD 4800

const int PRESSURE = 0x1F;      //3 most significant bits of pressure
const int PRESSURE_LSB = 0x20;  //16 least significant bits of pressure
const int TEMPERATURE = 0x21;   //16 bit temperature reading
const byte READ = 0b11111100;     // SCP1000's read command
const byte WRITE = 0b00000010;   // SCP1000's write command

const int dataReadyPin = 6;
const int chipSelectPin = 7;

SoftwareSerial cell(2,3);
char mobilenumber[] = "07733676867";  // Replace xxxxxxxx with the recipient's mobile number
// Create an instance of the TinyGPS object
TinyGPS gps;
// Initialize the NewSoftSerial library to the pins you defined above
SoftwareSerial uart_gps(RXPIN, TXPIN);

// This is where you declare prototypes for the functions that will be 
// using the TinyGPS library.
void getgps(TinyGPS &gps);

// In the setup function, you need to initialize two serial ports; the 
// standard hardware serial port (Serial()) to communicate with your 
// terminal program an another serial port (NewSoftSerial()) for your 
// GPS.
void setup()
{
  // This is the serial rate for your terminal program. It must be this 
  // fast because we need to print everything before a new sentence 
  // comes in. If you slow it down, the messages might not be valid and 
  // you will likely get checksum errors.
  cell.begin(9600);
  Serial.begin(115200);
  //Sets baud rate of your GPS
  uart_gps.begin(GPSBAUD);
  
  SPI.begin();
  
  pinMode(dataReadyPin, INPUT);
  pinMode(chipSelectPin, OUTPUT);
  
  
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

  writeRegister(0x02, 0x2D);
  writeRegister(0x01, 0x03);
  writeRegister(0x03, 0x02);
  // give the sensor time to set up:
  delay(100);
}

// This is the main loop of the code. All it does is check for data on 
// the RX pin of the ardiuno, makes sure the data is valid NMEA sentences, 
// then jumps to the getgps() function.
void loop()
{
  while(uart_gps.available())     // While there is data on the RX pin...
  {
      int c = uart_gps.read();    // load the data into a variable...
      if(gps.encode(c))      // if there is a new valid sentence...
      {
        getgps(gps);         // then grab the data.
      }
  }
  //Select High Resolution Mode
  writeRegister(0x03, 0x0A);

  // don't do anything until the data ready pin is high:
  if (digitalRead(dataReadyPin) == HIGH) {
    //Read the temperature data
    int tempData = readRegister(0x21, 2);

    // convert the temperature to celsius and display it:
    float realTemp = (float)tempData / 20.0;
    Serial.print("Temp[C]=");
    Serial.println(realTemp);

    //Read the pressure data highest 3 bits:
    unsigned long  pressure_data_high = readRegister(0x1F, 1);
    //pressure_data_high &= 0b00000111; //you only needs bits 2 to 0
    pressure_data_high = pressure_data_high << 16;

    //Read the pressure data lower 16 bits:
    unsigned int pressure_data_low = readRegister(0x20, 2);
    
    //combine the two parts into one 19-bit number:
    unsigned long pressure = (pressure_data_high  | pressure_data_low)/4;

    // display the temperature:
    Serial.print("Pressure [Pa]=");
    Serial.println(String(pressure));
    Serial.println();
  }
}

// The getgps function will get and print the values we want.

  void getgps(TinyGPS &gps)
  {
   
  // To get all of the data into variables that you can use in your code, 
  // all you need to do is define variables and query the object for the 
  // data. To see the complete list of functions see keywords.txt file in 
  // the TinyGPS and NewSoftSerial libs.
  
  // Define the variables that will be used
  float latitude, longitude, alt;
  char lat[10];
  char lon[10];
  char a[10];
  String latcell;
  String longcell;
  String acell;
  String latlon;
  // Then call this function
  gps.f_get_position(&latitude, &longitude);
  alt=gps.f_altitude();
  
  // Same goes for date and time
  int year;
  byte month, day, hour, minute, second, hundredths;
  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);
  // Print data and time
  Serial.print("Date: "); Serial.print(month, DEC); Serial.print("/"); 
  Serial.print(day, DEC); Serial.print("/"); Serial.print(year);
  Serial.print("  Time: "); Serial.print(hour, DEC); Serial.print(":"); 
  Serial.print(minute, DEC); Serial.print(":"); Serial.print(second, DEC); 
  Serial.print("."); Serial.println(hundredths, DEC);
  //Since month, day, hour, minute, second, and hundr
   
  // Here you can print the altitude and course values directly since 
  // there is only one value for the function
  Serial.print("Altitude (meters): "); Serial.println(alt);  
  // Same goes for course
  Serial.print("Course (degrees): "); Serial.println(gps.f_course()); 
  // And same goes for speed
  Serial.print("Speed(kmph): "); Serial.println(gps.f_speed_kmph());
  Serial.println();
  
  // Here you can print statistics on the sentences.
  unsigned long chars;
  unsigned short sentences, failed_checksum;
  gps.stats(&chars, &sentences, &failed_checksum);
  Serial.print("Failed Checksums: ");Serial.print(failed_checksum);
  Serial.println(); Serial.println();
  
  dtostrf(latitude,1,2,lat);  
  latcell=String(lat);

  dtostrf(longitude,1,2,lon);
  longcell=String(lon);
 
  dtostrf(alt,1,2,a);
  acell=String(a);  
 
  //dtostrf(longitude,1,2,lon);
  //ongcell=String(lon); 
 
  //dtostrf(longitude,1,2,lon);
//  longcell=String(lon); 
 
  //dtostrf(longitude,1,2,lon);
  //longcell=String(lon);
 
  latlon=("Latitude: " + latcell + ", Longitutde: " + longcell + ", Altitude: " + acell + "m");
  
  if (minute%1==0)
    {
       // You can now print variables latitude and longitude
        Serial.print("Lat/Long: "); 
        Serial.print(latitude,5); 
        Serial.print(", "); 
        Serial.println(longitude,5);
        sendText(latlon);
  
      }
  
  
  delay(10000);
  
}


//Read from or write to register from the SCP1000:
unsigned int readRegister(byte thisRegister, int bytesToRead ) {
  byte inByte1 = 0;           // incoming bytes from the SPI
  byte inByte2 = 0;
  unsigned int result = 0;   // result to return
  // SCP1000 expects the register name in the upper 6 bits
  // of the byte. So shift the bits left by two bits:
  thisRegister = thisRegister << 2;
  // now combine the address and the command into one byte
  byte dataToSend = thisRegister & READ;
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);
  // send the device the register you want to read:
  SPI.transfer(dataToSend);
  // send a value of 0 to read the first byte returned:
  inByte1 = SPI.transfer(0x00);
  result = int(inByte1);
  // decrement the number of bytes left to read:
  bytesToRead--;
  // if you still have another byte to read:
  if (bytesToRead > 0) {
    // shift the first byte left, then get the second byte:
    result = result << 8;
    inByte2 = SPI.transfer(0x00);
    // combine the byte you just got with the previous one:
    result = result | inByte2;
    // decrement the number of bytes left to read:
    bytesToRead--;
  }
  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
  // return the result:
  return(result);
}


//Sends a write command to SCP1000

void writeRegister(byte thisRegister, byte thisValue) {

  // SCP1000 expects the register address in the upper 6 bits
  // of the byte. So shift the bits left by two bits:
  thisRegister = thisRegister << 2;
  // now combine the register address and the command into one byte:
  byte dataToSend = thisRegister | WRITE;

  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);

  SPI.transfer(dataToSend); //Send register location
  SPI.transfer(thisValue);  //Send value to record into register

  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
}


  void sendText(String text) 
  {
  cell.println("AT+CMGF=1"); // set SMS mode to text
  cell.print("AT+CMGS=");  // now send message...
  Serial.println("Sent");
  cell.write(34); // ASCII equivalent of "
  cell.print(mobilenumber);
  cell.write(34);  // ASCII equivalent of "
  cell.println();
  delay(500); // give the module some thinking time
  cell.print(text);   // our message to send
  cell.write(26);  // ASCII equivalent of Ctrl-Z
  
  return;
}

