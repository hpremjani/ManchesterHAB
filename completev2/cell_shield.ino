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
