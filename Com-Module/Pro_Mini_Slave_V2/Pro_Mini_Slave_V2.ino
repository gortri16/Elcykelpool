

#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#define MAX485_DE       A4
#define MAX485_RE_NEG   A5

#define numCoils 8
#define numDiscreteInputs 0
#define numHoldingRegisters 0
#define numInputRegisters 0

byte SLAVE_ID = 0x02;
byte SOH = 0x01;
byte EOT = 0x04;
byte ReadCoilStatusCode = 0x01;
byte ForceSingleCoilCode = 0x05;

byte packetBuffer[24];  // buffer to hold incoming packet,
int packetSize = 0;
bool openGarage = false;

// Output - kretskort
#define motor1a 2
#define motorpwm 3
#define motor1b 4
#define relay 5           // on = spänning till lås. 
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
bool bikeRemoved = false;
bool garageOn = false;
bool opened = false;
int doorStage = 0;

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

  /*
    Set by server
    0x3E  -  1=Give bike to customer
    0x3F  -
    Read from server
    0x40  -  1=Bike in garage
    0x41  -  1=Bike availible for rent
    0x42  -  Battery charging status
    0x43  -  1=Bike at top position
    0x44  -
    0x45  -
  */

  // configure coils at address 0x3E
  ModbusRTUServer.configureCoils(0x3E, numCoils);

  // configure discrete inputs at address 0x00
  //ModbusRTUServer.configureDiscreteInputs(0x00, numDiscreteInputs);

  // configure holding registers at address 0x00
  //ModbusRTUServer.configureHoldingRegisters(0x00, numHoldingRegisters);

  // configure input registers at address 0x00
  //ModbusRTUServer.configureInputRegisters(0x00, numInputRegisters);

  ModbusRTUServer.coilWrite(0x3E, 0x00);
  ModbusRTUServer.coilWrite(0x40, 0x00);

  // Motor
  pinMode(motor1a, OUTPUT);
  pinMode(motor1b, OUTPUT);
  pinMode(motorpwm, OUTPUT);
  pinMode(relay, OUTPUT);
  //Leds
  pinMode(greenled, OUTPUT);
  pinMode(redled, OUTPUT);
  //Buttons digital input
  pinMode(buttonup, INPUT_PULLUP);
  pinMode(buttondown, INPUT_PULLUP);
  pinMode(buttonservice, INPUT_PULLUP);
  //Lift location inputs
  pinMode(liftup, INPUT_PULLUP);
  pinMode(buttonup, INPUT_PULLUP);
  pinMode(buttondown, INPUT_PULLUP);
  //Door state
  pinMode(doorclosed, INPUT_PULLUP);
  pinMode(dooropen, INPUT_PULLUP);
  //currentsens

  pinMode(LED_BUILTIN, OUTPUT);

  relayOff();
  delay(500);
  greenLedOff();
  redLedOff();

  lockDoor();

  /*
    while (digitalRead(liftup)) {
    motorUp();
    }
    motorStop();*/

  if (!digitalRead(liftup)) {
    ModbusRTUServer.coilWrite(0x40, 0x01);  // Bike in garage
  } else {
    ModbusRTUServer.coilWrite(0x40, 0x00);  // Bike not in garage
  }

  delay(30);
  Serial.println("Setup Complete");
  delay(30);
}

void loop() {
  // Poll for Modbus RTU Messages
  RTUPoll();

  // Update value in coils from pin inputs
  //updateCoils();

  // Pick up or turn in leased bike
  systemControl();
}

// Give out or return leased bike
void systemControl() {
  if (openGarage) {
    switch (doorStage) {
      case 0:                    // Closed door from start
        if (!garageOn) {
          greenLedOn();
          redLedOff();
          unlockDoor();
          garageOn = true;
        }
        if (digitalRead(doorclosed)) {
          doorStage = 1;
        }
        break;
      case 1:                    // Door half open
        greenLedOn();
        redLedOn();
        // Door is fully open
        if (!digitalRead(dooropen)) {
          doorStage = 2;
        }
        // Door closed but bike not returned
        if (!digitalRead(doorclosed) && digitalRead(liftup)) {
          doorStage = 3;
        }
        // Door closed and bike returned
        if (!digitalRead(doorclosed) && !digitalRead(liftup)) {
          doorStage = 4;
        }
        break;
      case 2:                    // Door full open
        if (!digitalRead(dooropen)) {
          greenLedOn();
          redLedOff();
          LiftControl();
        } else {
          doorStage = 1;
        }
        break;
      case 3:                    // Door closed after opened, bike not returned
        lockDoor();
        greenLedOff();
        redLedOff();
        // Check if door locked correctly
        if (digitalRead(doorclosed)) {  // Door not closed
          blinkRed();
          unlockDoor();
          doorStage = 1;
          break;
        }
        garageOn = false;
        ModbusRTUServer.coilWrite(0x40, 0x00);  // Bike removed from garage
        ModbusRTUServer.coilWrite(0x3E, 0x00);  // Reset garage
        openGarage = false;
        doorStage = 0;
        break;
      case 4:                    // Door closed after opened, bike returned
        lockDoor();
        greenLedOff();
        redLedOff();
        // Check if door locked correctly
        if (digitalRead(doorclosed)) {  // Door not closed
          blinkRed();
          unlockDoor();
          doorStage = 1;
          break;
        }
        garageOn = false;
        ModbusRTUServer.coilWrite(0x40, 0x01);  // Bike in garage
        ModbusRTUServer.coilWrite(0x3E, 0x00);  // Reset garage
        openGarage = false;
        doorStage = 0;
        break;
      default:                   // Should not reach this stage
        redLedOn();
        return;
        break;
    }
  }
}

// Controls the lift depending on which button is pressed
void LiftControl() {
  if (fault == 0) {
    if ((digitalRead(buttonup) == LOW) && (digitalRead(buttondown) == HIGH)) {
      motorUp();
    } else {
      motorStop();
    }

    if ((digitalRead(buttonup) == HIGH) && (digitalRead(buttondown) == LOW)) {
      motorDown();
    } else {
      motorStop;
    }

    /*
      if(digitalRead(buttonservice) == LOW) {
      if(digitalRead(relay)==HIGH) {
        relayOff();
        redLedOff();
        greenLedOff();
      }
      else {
        relayOn();
        redLedOn();
        greenLedOn();
      }
      delay(1000);
      }*/

    getCurrent();
    //delay(1000);
    //Serial.println((((analogRead(chargercurrent))*0.0049)-2.5)/0.2);

    if (current > 100000000) {
      motorBreak();
      fault = 1;
    }
  }
  else {  // if fault == 1
    greenLedOn();
    redLedOff();
    delay(1000);
    greenLedOff();
    redLedOn();
    delay(1000);
    if (digitalRead(buttonservice) == LOW) {
      fault = 0;
    }
  }
}

// Poll for Modbus RTU Messages
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
  //printBuffer();

  // Read message
  j = 0;
  while (packetBuffer[j] != 0x01) {
    j++;
    if (packetBuffer[j] == NULL) {
      //Serial.println("SOH not found");
      clearBuffer();
      return;
    }
  }
  if (packetBuffer[j + 1] != SLAVE_ID) {
    //Serial.println("Message not for me");
    clearBuffer();
    return;
  }

  // Send response depending on fuction code
  switch (packetBuffer[j + 2]) {
    case 0x01:                    // Read coil status
      ReadCoilStatus();
      break;
    case 0x05:                    // Write single coil
      ForceSingleCoil();
      break;
    default:                      // Function code not implemented
      // if nothing else matches, do the default
      //Serial.print("Error, Function code: ");
      //Serial.print(packetBuffer[2], HEX);
      //Serial.println(" is not implemented");
      clearBuffer();
      return;
      break;
  }

  clearBuffer();
}

// Modbus function 0x01
void ReadCoilStatus() {
  byte addressHi = packetBuffer[j + 3];
  byte addressLo = packetBuffer[j + 4];
  byte numberHi =  packetBuffer[j + 5];
  byte numberLo =  packetBuffer[j + 6];

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
  for (int i = 0; i < numberLo; i++) {
    data[i] = ModbusRTUServer.coilRead(addressLo + i);
  }

  if (packetBuffer[j + 7] != 0x04) {
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
  for (int i = 0; i < sizeof(data); i++) {
    RS485.write(data[i]);
  }
  RS485.write(EOT);

  postTransmission();

  Serial.println(" ");
  for (int i = 0; i < sizeof(data); i++) {
    Serial.println(data[i], HEX);
  }
}

// Modbus function 0x05
void ForceSingleCoil() {
  byte addressHi = packetBuffer[j + 3];
  byte addressLo = packetBuffer[j + 4];
  byte dataHi =  packetBuffer[j + 5];
  byte dataLo =  packetBuffer[j + 6];

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

  if (packetBuffer[j + 7] != 0x04) {
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

  if (addressLo == 0x3E) {
    openGarage = true;
  }
}

void printBuffer() {
  Serial.println("");
  Serial.print("Print buffer: ");
  for (int i = 0; i < sizeof(packetBuffer); i++) {
    Serial.print(packetBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");
}

void clearBuffer() {
  for (int i = 0; i < sizeof(packetBuffer); i++) {
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

// Update value in coils from pin inputs
void updateCoils() {
  /*
    Set by server
    0x3E  -  1=Give bike to customer
    0x3F  -
    Read from server
    0x40  -  1=Bike in garage
    0x41  -  1=Bike availible for rent
    0x42  -  Battery charging status
    0x43  -  1=Bike at top position
    0x44  -
    0x45  -
  */
  // Battery charging status
  byte value = (((analogRead(chargercurrent)) * 0.0049) - 2.5) / 0.2;
  ModbusRTUServer.coilWrite(0x42, value);
  /*
    // Make bika availible for hire if bike battery is full
    if (ModbusRTUServer.coilRead(0x40) {
    if (value <= 0) {
      ModbusRTUServer.coilWrite(0x41, 0x01);
    }
    else {
      ModbusRTUServer.coilWrite(0x41, 0x00);
    }
    }*/

  // Bike at top postiion
  value = digitalRead(liftup);
  ModbusRTUServer.coilWrite(0x43, value);
}

void lockDoor() {
  relayOn();
  delay(200);
  motorDown();
  delay(200);
  motorStop();
  delay(200);
  relayOff();
  delay(200);
}

void unlockDoor() {
  relayOn();
  delay(200);
  motorUp();
  delay(200);
  motorStop();
  delay(200);
  relayOff();
  delay(200);
}

void blinkRed() {
  redLedOff();
  delay(400);
  redLedOn();
  delay(400);
  redLedOff();
  delay(400);
  redLedOn();
  delay(400);
  redLedOff();
  delay(400);
  redLedOn();
  delay(400);
  redLedOff();
  delay(400);
}

void motorDown() {
  digitalWrite(motor1a, HIGH);
  digitalWrite(motor1b, LOW);
  analogWrite(motorpwm, speed);
}
void motorUp() {
  digitalWrite(motor1a, LOW);
  digitalWrite(motor1b, HIGH);
  analogWrite(motorpwm, speed);
}
void motorStop() {
  digitalWrite(motor1a, LOW);
  digitalWrite(motor1b, LOW);
  analogWrite(motorpwm, 0);
}
void motorBreak() {
  digitalWrite(motor1a, HIGH);
  digitalWrite(motor1b, HIGH);
  analogWrite(motorpwm, 0);
}
void speedUp() {
  speed = speed + 10;
  if (speed > 255) {
    speed = 255;
    delay(500);
  }
}
void speedDown() {
  speed = speed - 10;
  if (speed < 0) {
    speed = 0;
  }
}
void getCurrent() {
  current = analogRead(currentsens);
  Serial.println(current);
}
void greenLedOn() {
  if (!digitalRead(greenled)) {
    digitalWrite(greenled, HIGH);
  }
  delay(10);
}
void greenLedOff() {
  if (digitalRead(greenled)) {
    digitalWrite(greenled, LOW);
  }
  delay(10);
}
void redLedOn() {
  if (!digitalRead(redled)) {
    digitalWrite(redled, HIGH);
  }
  delay(10);
}
void redLedOff() {
  if (digitalRead(redled)) {
    digitalWrite(redled, LOW);
  }
  delay(10);
}
void relayOn() {
  digitalWrite(relay, HIGH);
}
void relayOff() {
  digitalWrite(relay, LOW);
}
