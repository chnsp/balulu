// A simple sketch to read GPS data and parse the $GPRMC string 
// see http://www.ladyada.net/make/gpsshield for more info

// If using Arduino IDE prior to version 1.0,
// make sure to install newsoftserial from Mikal Hart
// http://arduiniana.org/libraries/NewSoftSerial/
 #include "Arduino.h"
 #include "SoftwareSerial.h"

SoftwareSerial mySerial = SoftwareSerial(2, 3);

// Use pin 4 to control power to the GPS
#define powerpin 4

// Set the GPSRATE to the baud rate of the GPS module. Most are 4800
// but some are 38400 or other. Check the datasheet!
#define GPSRATE 4800
//#define GPSRATE 38400

// The buffer size that will hold a GPS sentence. They tend to be 80 characters long
// so 90 is plenty.
#define BUFFSIZ 90 // plenty big


// global variables
char buffer[BUFFSIZ];        // string buffer for the sentence
char *parseptr;              // a character pointer for parsing
char buffidx;                // an indexer into the buffer

// The time, date, location data, etc.
uint8_t hour, minute, second, year, month, date;
uint32_t latitude, longitude;
uint8_t groundspeed, trackangle;
char latdir, longdir;
char status;

// MOrse code stuff
char letters[] = 
{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 
  'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 
  'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', 
  '4', '5', '6', '7', '8', '9', ' ' };
  
char* morse[] =
{ ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", 
  "-.-", ".-..", "-", "-.", "---", ".--.", "--.-", ".-.", "...", "-", 
  "..-", "...-", ".--", "-..-", "-.--", "--..", "-----", ".----", "..---", "...--", 
  "....-", ".....", "-....", "--...", "---..", "----.", " " };

long ditLength = 100;
int speakerPin = 11;
long toneFreq = 1915;
int turnTXon = 12;
int outputLED = 13;
char myString[] = "kc9spn";


void setup() 
{ 
  if (powerpin) {
    pinMode(powerpin, OUTPUT);
  }
  
  // Use the pin 13 LED as an indicator
  pinMode(13, OUTPUT);
  
  // connect to the serial terminal at 9600 baud
  Serial.begin(9600);
  
  // connect to the GPS at the desired rate
  //mySerial.begin(GPSRATE);
   
  // prints title with ending line break 
  Serial.println("GPS parser"); 
 
   digitalWrite(powerpin, LOW);         // pull low to turn on!

   // Morse Code
   pinMode(speakerPin, OUTPUT);
   pinMode(turnTXon, INPUT);           // set pin to input
   digitalWrite(turnTXon, HIGH); 

} 
 
uint32_t parsedecimal(char *str) {
  uint32_t d = 0;
  
  while (str[0] != 0) {
   if ((str[0] > '9') || (str[0] < '0'))
     return d;
   d *= 10;
   d += str[0] - '0';
   str++;
  }
  return d;
}

void readline(void) {
  char c;
  
  buffidx = 0; // start at begninning
  while (1) {
      c=mySerial.read();
      if (c == -1)
        continue;
      Serial.print(c);
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
  
  mySerial.begin(GPSRATE);
  Serial.print("\n\rRead: ");
  readline();
  
  // check if $GPRMC (global positioning fixed data)
  if (strncmp(buffer, "$GPGGA",6) == 0) {
    
    // hhmmss time data
    parseptr = buffer+7;
    tmp = parsedecimal(parseptr); 
    hour = tmp / 10000;
    minute = (tmp / 100) % 100;
    second = tmp % 100;
    
    parseptr = strchr(parseptr, ',') + 1;
    status = parseptr[0];
    parseptr += 2;
    
    // grab latitude & long data
    // latitude
    latitude = parsedecimal(parseptr);
    if (latitude != 0) {
      latitude *= 10000;
      parseptr = strchr(parseptr, '.')+1;
      latitude += parsedecimal(parseptr);
    }
    parseptr = strchr(parseptr, ',') + 1;
    // read latitude N/S data
    if (parseptr[0] != ',') {
      latdir = parseptr[0];
    }
    
    //Serial.println(latdir);
    
    // longitude
    parseptr = strchr(parseptr, ',')+1;
    longitude = parsedecimal(parseptr);
    if (longitude != 0) {
      longitude *= 10000;
      parseptr = strchr(parseptr, '.')+1;
      longitude += parsedecimal(parseptr);
    }
    parseptr = strchr(parseptr, ',')+1;
    // read longitude E/W data
    if (parseptr[0] != ',') {
      longdir = parseptr[0];
    }
    

    // groundspeed
    //    parseptr = strchr(parseptr, ',')+1;
    //groundspeed = parsedecimal(parseptr);

    // track angle
    //    parseptr = strchr(parseptr, ',')+1;
    //    trackangle = parsedecimal(parseptr);


    // date
    // parseptr = strchr(parseptr, ',')+1;
    // tmp = parsedecimal(parseptr); 
    // date = tmp / 10000;
    // month = (tmp / 100) % 100;
    // year = tmp % 100;
    
    // Serial.print("\n\tTime: ");
    // Serial.print(hour, DEC); Serial.print(':');
    // Serial.print(minute, DEC); Serial.print(':');
    // Serial.println(second, DEC);
    // Serial.print("\tDate: ");
    // Serial.print(month, DEC); Serial.print('/');
    // Serial.print(date, DEC); Serial.print('/');
    // Serial.println(year, DEC);
    
    Serial.print("\tLat: "); 
    if (latdir == 'N')
       Serial.print('+');
    else if (latdir == 'S')
       Serial.print('-');

    Serial.print(latitude/1000000, DEC); Serial.print("* ");
    Serial.print((latitude/10000)%100, DEC); Serial.print('\''); Serial.print(' ');
    Serial.print((latitude%10000)*6/1000, DEC); Serial.print('.');
    Serial.print(((latitude%10000)*6/10)%100, DEC); Serial.println('"');
   
    Serial.print("\tLong: ");
    if (longdir == 'E')
       Serial.print('+');
    else if (longdir == 'W')
       Serial.print('-');
    Serial.print(longitude/1000000, DEC); Serial.print("* ");
    Serial.print((longitude/10000)%100, DEC); Serial.print('\''); Serial.print(' ');
    Serial.print((longitude%10000)*6/1000, DEC); Serial.print('.');
    Serial.print(((longitude%10000)*6/10)%100, DEC); Serial.println('"');
   
  }
  //Serial.println(buffer);
  mySerial.end();

  // Morse code
   for (int i = 0; i < sizeof(myString); i++){
     for (int j = 0; j < sizeof(letters); j++){
      if (letters[j] == myString[i]) {
       Serial.print(letters[j]);
       Serial.print(":");
       //Serial.println(morse[j]);
      //blinkCode(morse[j]);
      playCode(morse[j]);
    
      } 
     }
   }
   delay(100);

}

  
void blinkCode(char code[]){
 digitalWrite(outputLED, LOW);
 for (int i = 0; i < strlen(code); i++){
  if (code[i] == '.') {
    Serial.print('.');
    digitalWrite(outputLED, HIGH);
    delay(ditLength);
    digitalWrite(outputLED, LOW);
   }
   if (code[i] == '-') {
    Serial.print('-');
     digitalWrite(outputLED, HIGH);
    delay(ditLength * 3);
    digitalWrite(outputLED, LOW);
   }
   if (code[i] == ' '){
     Serial.print(' ');
     delay(ditLength*7);
   }

   delay(ditLength);
  }
    Serial.println(); 
  delay(ditLength * 3);  
}


void playTone(long mylength, long mytone){
   for (long i = 0; i < mylength * 1000L; i += mytone * 2) {
      digitalWrite(speakerPin, HIGH);
      delayMicroseconds(mytone);
      digitalWrite(speakerPin, LOW);
      delayMicroseconds(mytone);
   }

}


void playCode(char code[]){
  for (int i = 0; i < strlen(code); i++){
    if (code[i] == '.') {
       playTone(ditLength, toneFreq);
    }   
    if (code[i] == '-') {
       playTone(ditLength*3, toneFreq);
     }
     if (code[i] == ' '){
       delay(ditLength*3);
     }
     delay(ditLength);
    } 
    delay(ditLength * 3);  

}
