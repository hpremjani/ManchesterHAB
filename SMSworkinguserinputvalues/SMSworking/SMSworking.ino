#include <SoftwareSerial.h>
#include <iostream.h>
#include <floattostring.h>
#include <string.h>



SoftwareSerial cell(2,3);  // We need to create a serial port on D2/D3 to talk to the GSM module
char mobilenumber[] = "07733676867";  // Replace xxxxxxxx with the recipient's mobile number
float latcell,longcell;
char lat[10];
char lon[10];
String latt;
String lonn;
String latlon;



void setup()
{  //Initialize serial ports for communication.
  cell.begin(9600);
  Serial.begin(57600);
  Serial.println("Started...waiting");
  for(int i = 0; i < 40; i++)
  {
    delay(1000);
    Serial.println(i + 1);
  }

latcell=100.1;
longcell=101.2;

dtostrf(latcell,1,2,lat);
latt=String(lat);

dtostrf(longcell,1,2,lon);
lonn=String(lon);

latlon=(latt + ", " + lonn);

Serial.println(latlon);

}


  
  void loop()
{
  
//str1 = String(latcell);
//str2 = "(floatToString(buffer, longcell , 5);)";


//sendText(longcell);
//sendText(latcell);  

sendText(latlon);
 // sendText(longcell);
  delay(10000);
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


