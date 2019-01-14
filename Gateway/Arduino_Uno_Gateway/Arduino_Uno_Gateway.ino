/*
 Chat Server

 A server that distributes any incoming UDP modbus messages to all
 connected slaves.
 Using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 05 Dec 2018
 by Johannes Hellgren
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
//#include <ArduinoModbus.h>

/*
  485 message : 01 03 01 00 3E 00 08 04
  udp message :    01 01 00 3E 00 08 
*/


/*
  ------- UDP -------
*/
#define slaveID 0x01

// IP address and port for Raspberry Pi 3 Client
IPAddress clientIP(192, 168, 1, 10);
unsigned int clientPort = 8888;
// IP address and port for my PC
//IPAddress clientIP(192, 168, 1, 1);
//unsigned int clientPort = 57202;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
unsigned int localPort = 8888;      // local port to listen on

byte packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
int incomingByte;
bool newMessage = false;
int j;

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

/*
  ------- RTU -------
*/
#define MAX485_DE       3
#define MAX485_RE_NEG   2

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
  RS485.noReceive();
  RS485.beginTransmission();
}

void postTransmission()
{
  RS485.endTransmission();
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
  RS485.receive();
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Gateway Startup");
  
  /*
    ------- UDP -------
  */
  // initialize the ethernet device
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  Serial.print("Server address: ");
  Serial.println(Ethernet.localIP());
  delay(50);
  
  // start UDP
  Udp.begin(localPort);
  
  /*
    ------- RTU -------
  */
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  
  // Init in receive node
  RS485.begin(9600);
  postTransmission();
}

void loop() {
  // poll for Modbus UDP query from server, passes the request to slaves
  UDPPoll();
   // poll for Modbus RTU respond from slave, passes the request to server
  RTUPoll();
}

void UDPPoll(){
  /*
    ------- Get message from server -------
  */
  // If there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // Print IP and port
    /*Serial.print("IP: ");
    IPAddress remote = Udp.remoteIP();
    for (int i=0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(", Port: ");
    Serial.println(Udp.remotePort());*/

    // read the packet into packetBufffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    
    /*
    ------- Send message to slaves -------
    */
    Serial.println("Send to slaves");
    byte SOH = 0x01;
    byte EOT = 0x04;
    preTransmission();
    RS485.write(SOH);
    for (int i=0; i<packetSize; i++) {
      RS485.write(packetBuffer[i]);
    }
    RS485.write(EOT);
    postTransmission();
  }
  delay(10);
}

void RTUPoll(){
  /*
    ------- Get message from slave -------
  */
  if (!RS485.available()) {
    return;
  }
  
  j = 0;
  while (RS485.available()) {
    incomingByte = RS485.read();
    if (incomingByte != 0x01 && newMessage != true) {
      return;
    }
    newMessage = true;
    
    packetBuffer[j] = incomingByte;
    j++;
    delay(10);
  }
  Serial.println(" ");
  printBuffer();
  Serial.print("j: ");
  Serial.println(sizeof(packetBuffer));
  Serial.print("testPack: ");
  Serial.println(packetBuffer[j], HEX);
  Serial.write("Buffer Size: ");
  Serial.println(j);
  
  /*
    ------- Send message to server -------
  */
  // Send a reply to the IP address and port of the client (raspberry)
  Serial.println("Reply to server:");
  Udp.beginPacket(clientIP, clientPort);
  
  for (int i=0; i<j; i++) {
    Udp.write(packetBuffer[i]);
  }
  
  Udp.endPacket();
  clearBuffer();
  newMessage = false;
}

void printBuffer() {
  Serial.print("Print buffer: ");
  for (int i=0; i<j; i++) {
    Serial.print(packetBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");
}

void clearBuffer() {
  for (int i=0; i<sizeof(packetBuffer); i++) {
    packetBuffer[i] = 0x00;
  }
}
