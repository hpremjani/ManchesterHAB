/*  Example 26.3 GSM shield sending a SMS text message
http://tronixstuff.com/tutorials > chapter 26 */
#include <SoftwareSerial.h>
SoftwareSerial cell(2,3);  // We need to create a serial port on D2/D3 to talk to the GSM module
char mobilenumber[] = "07733676867";  // Replace xxxxxxxx with the recipient's mobile number
void setup()
{  //Initialize serial ports for communication.
cell.begin(9600);
delay(35000); // give the GSM module time to initialise, locate network etc.
// this delay time varies. Use example 26.1 sketch to measure the amount
// of time from board reset to SIND: 4, then add five seconds just in case
}
void loop()
{
cell.println("AT+CMGF=1"); // set SMS mode to text
cell.print("AT+CMGS=");  // now send message...
Serial.write(34);
// cell.print(34,BYTE); // ASCII equivalent of "
cell.print(mobilenumber);
Serial.write(34);
//cell.println(34,BYTE);  // ASCII equivalent of "
delay(500); // give the module some thinking time
cell.print("They call me the count... because I like to count! Ah ha ha ha");   // our message to send
Serial.write(26);
//cell.println(26,BYTE);  // ASCII equivalent of Ctrl-Z
delay(15000); // the SMS module needs time to return to OK status
do // You don't want to send out multiple SMSs.... or do you?
{
delay(1);
}
while (1>0);
}
