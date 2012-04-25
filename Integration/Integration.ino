/*
 * Pins:
 *  0,1: Hardware Serial used by AX.25 Chip
 *  2,3: SoftwareSerial used by GPS
 *  4:   Power for GPS
 */

#include "Arduino.h"
#include "SoftwareSerial.h"

// Use pins 2 and 3 to talk to the GPS. 2 is the TX pin, 3 is the RX pin
SoftwareSerial mySerial = SoftwareSerial(2, 3);

// Use pin 4 to control power to the GPS
#define powerpin 4

// Set the GPSRATE to the baud rate of the GPS module. Most are 4800
// but some are 38400 or other. Check the datasheet!
#define GPSRATE 4800

// The buffer size that will hold a GPS sentence. They tend to be 80 characters long
// so 90 is plenty.
#define BUFFSIZ 90 // plenty big

// gps global variables
char buffer[BUFFSIZ];        // string buffer for the sentence
char *parseptr;              // a character pointer for parsing
char buffidx;                // an indexer into the buffer

// ax.25 global variables
int turnTXOnPin = 12;


void setup() 
{ 
  /*
   *  GPS Setup
   */
  if (powerpin) {
    pinMode(powerpin, OUTPUT);
  }
  
  // Use the pin 13 LED as an indicator
  pinMode(13, OUTPUT);
  
  // connect to the GPS at the desired rate
  mySerial.begin(GPSRATE);
   
  digitalWrite(powerpin, LOW);         // pull low to turn on!

  /*
   *  AX.25 Transmitter Setup
   */
  pinMode(turnTXOnPin, INPUT);
  digitalWrite(turnTXOnPin, HIGH);
  Serial.begin(4800);                      //OPEN SERIAL LINE AT 4800
  delay(3);
  Serial.print("MKC9SPN\r\n");               //SET YOUR CALLSIGN HERE, HERE YOU SEE W1AW
  delay(10);                       
  Serial.print("PWIDE1-1,WIDE2-1\r\n");    //SET DIGIPATH HERE
  delay(10);
  /* Serial.print("L150\r\n"); */
  delay(10);
  Serial.print("!This is RadioShield test message #\r\n");
  delay(8000);
//  Serial.print("@This is RadioShield test message #\r\n");
//  delay(4000);
} 

/* 
 * Readline method used for GPS code. Reads a GPS line into
 * global variable buffer;
 */
void readline(void) {
  char c;
  
  buffidx = 0; // start at begninning
  while (1) {
      c=mySerial.read();
      if (c == -1)
        continue;
      if (c == '\n')
        continue;
      if ((buffidx == BUFFSIZ-1) || (c == '\r')) {
        buffer[buffidx] = 0;
        return;
      }
      buffer[buffidx++]= c;
  }
}

void loop() 
{ 
  uint32_t tmp;
  
//  Serial.print("Read: ");
  readline();

  Serial.print("!");
  Serial.print(buffer);
  Serial.print("\r\n");

  delay(8000);
}