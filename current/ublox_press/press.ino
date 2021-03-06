// SPI Pressure sensor
const int PRESSURE = 0x1F;      //3 most significant bits of pressure
const int PRESSURE_LSB = 0x20;  //16 least significant bits of pressure
const int TEMPERATURE = 0x21;   //16 bit temperature reading
const byte READ = 0b11111100;     // SCP1000's read command
const byte WRITE = 0b00000010;   // SCP1000's write command

const int dataReadyPin = 6;
const int chipSelectPin = 7;

void init_scp() {
  SPI.begin();

  pinMode(dataReadyPin, INPUT);
  pinMode(chipSelectPin, OUTPUT);

  writeRegister(0x02, 0x2D);
  writeRegister(0x01, 0x03);
  writeRegister(0x03, 0x02);
}

void read_scp(float &pressure, float &temperature) {
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
     // display the pressure:
    Serial.print("Pressure [Pa]=");
    Serial.println(String(pressure));
    //Serial.println();
  }
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

