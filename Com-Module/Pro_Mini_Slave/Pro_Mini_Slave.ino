

#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#define MAX485_DE       3
#define MAX485_RE_NEG   2

#define numCoils 4
#define numDiscreteInputs 0
#define numHoldingRegisters 0
#define numInputRegisters 0

byte SLAVE_ID = 0x03;
byte SOH = 0x01;
byte EOT = 0x04;
byte ReadCoilStatusCode = 0x01;
byte ForceSingleCoilCode = 0x05;

byte packetBuffer[24];  // buffer to hold incoming packet,
int packetSize = 0;

// Output - kretskort
#define motor1a 2
#define motorpwm 3
#define motor1b 4
#define relay 5
#define greenled 6
#define redled 7
// Input - kretskort
#define buttonup 8
#define buttondown 9
#define buttonservice 10
#define liftdown 11       // lägesgivare ner
#define liftup 12         // lägesgivare upp
#define currentsens A0    // motor stöm mätning - 4A utan last
#define doorclosed A1
#define dooropen A2
#define chargercurrent A3 // ström laddare - 2A max
int speed = 255;
int current = 0;
int fault = 0;

int j = -1;

void setup() {
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  
  Serial.begin(9600);
  while (!Serial);
  
  // start the Modbus RTU server, with (slave) id
  if (!ModbusRTUServer.begin(SLAVE_ID, 9600)) {
    Serial.println("Failed to start Modbus RTU Server!");
    while (1);
  }

  RS485.begin(9600);
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);
  RS485.receive();
  
  delay(30);
  Serial.print("Slave ");
  Serial.print(SLAVE_ID, HEX);
  Serial.println(" Startup");
  delay(30);
  
  // configure coils at address 0x3E
  ModbusRTUServer.configureCoils(0x3E, numCoils);

  // configure discrete inputs at address 0x00
  //ModbusRTUServer.configureDiscreteInputs(0x00, numDiscreteInputs);

  // configure holding registers at address 0x00
  //ModbusRTUServer.configureHoldingRegisters(0x00, numHoldingRegisters);

  // configure input registers at address 0x00
  //ModbusRTUServer.configureInputRegisters(0x00, numInputRegisters);
}

void loop() {
  
  // Poll for Modbus RTU Messages
  RTUPoll();
  
  // set the value of a coil
  //ModbusRTUServer.coilWrite(0x3E, 1);
  
  /*
  while (fault == 0) {
    if((digitalRead(buttonup) == HIGH) && (digitalRead(buttondown) == LOW)){
      motorUp();
      greenLedOn();
    } else {
      motorStop();
      greenLedOf();
    }
  
    if((digitalRead(buttonup) == LOW) && (digitalRead(buttondown) == HIGH)){
      motorDown();
      redLedOn();
    } else {
      motorStop;redLedOf();
    }
 
    if(digitalRead(buttonservice) == LOW) {
      if(digitalRead(relay)==HIGH) {
        relayOf();
        redLedOf();
        greenLedOf();}
      else {
        relayOn();
        redLedOn();
        greenLedOn();
      }
    delay(1000);
    }
    //getCurrent();
    delay(1000);
    Serial.println((((analogRead(chargercurrent))*0.0049)-2.5)/0.2);
  
    if(current > 100000000){
      motorBreak();
      fault = 1;
    }
  }
  while (fault == 1) {
    greenLedOn();
    redLedOf();
    delay(1000);                      
    greenLedOf();
    redLedOn();
    delay(1000);
    if ((digitalRead(buttonup) == HIGH) && (digitalRead(buttondown) == HIGH)) {
      fault = 0;
    }
  }
  */
}

void RTUPoll() {
  if (!RS485.available()) {
    return;
  }
  
  // Store message in buffer
  j = -1;
  while (RS485.available()) {
    j++;
    packetSize++;
    packetBuffer[j] = RS485.read();
    delay(10);
  }
  printBuffer();
  
  // Read message
  j = 0;
  while (packetBuffer[j] != 0x01) {
    j++;
    if (packetBuffer[j] == NULL) {
      Serial.println("SOH not found");
      clearBuffer();
      return;
    } 
  }
  if (packetBuffer[j+1] != SLAVE_ID) {
      Serial.println("Message not for me");
      clearBuffer();
      return;
  }
  
  // Send response depending on fuction code
  switch (packetBuffer[j+2]) {
    case 0x01:                    // Read coil status
      ReadCoilStatus();
      break;
    case 0x05:                    // Write single coil
      ForceSingleCoil();
      break;
    default:                      // Function code not implemented
      // if nothing else matches, do the default
      Serial.print("Error, Function code: ");
      Serial.print(packetBuffer[2], HEX);
      Serial.println(" is not implemented");
      clearBuffer();
      return;
     break;
  }
  
  clearBuffer();
}

void ReadCoilStatus() {
  byte addressHi = packetBuffer[j+3];
  byte addressLo = packetBuffer[j+4];
  byte numberHi =  packetBuffer[j+5];
  byte numberLo =  packetBuffer[j+6];
  
  // Check if coil address is valid
  int coilValue = ModbusRTUServer.coilRead(addressLo);
  if (coilValue != 0 && coilValue != 1) {
    Serial.print("Error, Coil address: ");
    Serial.print(addressLo, HEX);
    Serial.println(" not found");
    clearBuffer();
    return;
  }
  // Check if number of coils requested is valid
  if (numberLo <= 0 || numberLo > numCoils) {
    Serial.print("Error, Number of coils: ");
    Serial.print(numberLo, HEX);
    Serial.println(" is invalid");
    clearBuffer();
    return;
  }
  
  // Read coil values
  byte data[numberLo];
  for (int i=0; i<numberLo; i++) {
    data[i] = ModbusRTUServer.coilRead(addressLo + i);
  }
  
  if (packetBuffer[j+7] != 0x04) {
      Serial.println("EOT not found");
      clearBuffer();
      return;
  }
  
  Serial.println("Sending response");
  // Respond to master with coil values
  preTransmission();
  
  RS485.write(SOH);
  RS485.write(SLAVE_ID);
  RS485.write(ReadCoilStatusCode);
  RS485.write(byte(sizeof(data)));
  for (int i=0; i<sizeof(data); i++) {
    RS485.write(data[i]);
  }
  RS485.write(EOT);
  
  postTransmission();
  
  Serial.println(" ");
  for (int i=0; i<sizeof(data); i++) {
    Serial.println(data[i], HEX);
  }
}

void ForceSingleCoil() {
  byte addressHi = packetBuffer[j+3];
  byte addressLo = packetBuffer[j+4];
  byte dataHi =  packetBuffer[j+5];
  byte dataLo =  packetBuffer[j+6];
  
  // Check if coil address is valid
  int coilValue = ModbusRTUServer.coilRead(addressLo);
  if (coilValue != 0 && coilValue != 1) {
    Serial.print("Error, Coil address: ");
    Serial.print(addressLo, HEX);
    Serial.println(" not found");
    clearBuffer();
    return;
  }
  
  // set the value of the coil
  if (dataLo == 0x01) {
    Serial.println("Set to HIGH");
    ModbusRTUServer.coilWrite(addressLo, 1);
  } 
  else if (dataLo == 0x00) {
    Serial.println("Set to LOW");
    ModbusRTUServer.coilWrite(addressLo, 0);
  } 
  else {
    Serial.print("Error, Force Single Coil data value: ");
    Serial.print(dataLo, HEX);
    Serial.println(" is invalid");
    clearBuffer();
    return;
  }
  
  if (packetBuffer[j+7] != 0x04) {
      Serial.println("EOT not found");
      clearBuffer();
      return;
  }
  
  // Respond to master to confirm data write to coil
  Serial.println("Sending response");
  preTransmission();
  RS485.write(SOH);
  RS485.write(SLAVE_ID);
  RS485.write(ForceSingleCoilCode);
  RS485.write(addressHi);
  RS485.write(addressLo);
  RS485.write(dataHi);
  RS485.write(dataLo);
  RS485.write(EOT);
  postTransmission();
}

void printBuffer() {
  Serial.println("");
  Serial.print("Print buffer: ");
  for (int i=0; i<sizeof(packetBuffer); i++) {
    Serial.print(packetBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");
}

void clearBuffer() {
  for (int i=0; i<sizeof(packetBuffer); i++) {
    packetBuffer[i] = 0x00;
  }
  packetSize = 0;
}

void preTransmission() {
  digitalWrite(MAX485_RE_NEG, HIGH);
  digitalWrite(MAX485_DE, HIGH);
  RS485.noReceive();
  RS485.beginTransmission();
}

void postTransmission() {
  RS485.endTransmission();
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);
  RS485.receive();
}

void motorUp() {
  digitalWrite(motor1a, HIGH);
  digitalWrite(motor1b, LOW);
  analogWrite(motorpwm, speed);
}
void motorDown() {
  digitalWrite(motor1a,LOW);
  digitalWrite(motor1b,HIGH);
  analogWrite(motorpwm,speed);
}
void motorStop() {
  digitalWrite(motor1a,LOW);
  digitalWrite(motor1b,LOW);
  analogWrite(motorpwm,0);
}
void motorBreak() {
  digitalWrite(motor1a,HIGH);
  digitalWrite(motor1b,HIGH);
  analogWrite(motorpwm,0);
}
void speedUp() {
  speed = speed +10;
  if(speed > 255) {
    speed = 255;
    delay(500);
  }
}
void speedDown() {
  speed = speed -10;
  if (speed < 0) {
    speed = 0;
  }
}
void getCurrent() {
  current = analogRead(currentsens);
  Serial.println(current);
}
void greenLedOn() {
  digitalWrite(greenled,HIGH);
}
void greenLedOf() {
  digitalWrite(greenled,LOW);
}
void redLedOn() {
  digitalWrite(redled,HIGH);
}
void redLedOf() {
  digitalWrite(redled,LOW);
}
void relayOn() {
  digitalWrite(relay,HIGH);
}
void relayOf() {
  digitalWrite(relay,LOW);
}
