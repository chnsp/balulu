/* Read GPS, parse string, log to SD card, send APRS packet to radio shield, test for cut-down condition 


 */

#include <NewSoftSerial.h>
#include <avr/pgmspace.h>
#include "AF_SDLog.h"
#include <WProgram.h>

NewSoftSerial gpsSerial =  NewSoftSerial(2, 3); // gps serial data pins

#define powerpin 4
#define redLED 5
#define greenLED 6
#define GPSRATE 4800
// #define RADIOSHIELDRATE 4800



#define BUFFSIZ 100 // length of buffer used by readline() for GPS sentence

AF_SDLog card;
File f;
char fileName[12];

// time APRS packet
long previousMillis = 0;  // last time APRS packet sent
long interval = 10000; // how often to send a packet (10 seconds = 10,000 milliseconds)


void error(uint8_t errno);
boolean restrictedArea(char* latitude, char* latitudeDirection, char* longitude, char* longitudeDirection);
void cutdown();
char* readline(void);
// void setup();


void setup()			  // run once, when the sketch starts
{
  Serial.begin(19200);			   // initialize serial communication with computer - probably need to use for radio shield at 4800
  pinMode(powerpin, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  
  gpsSerial.begin(GPSRATE);
  //  radioShieldSerial.begin(RADIOSHIELDRATE);
  
   
  // prints title with ending line break 
  Serial.println("GPS parser"); 
  
  digitalWrite(powerpin, LOW);         // pull low to turn on!
  
  if (!card.init_card()) {
    Serial.println("Card init. failed!");
    error(1);
  }
  if (!card.open_partition()) {
    Serial.println("No partition!");
    error(2);
  }
  if (!card.open_filesys()) {
    Serial.println("Can't open filesys");
    error(3);
  }
  if (!card.open_dir("/")) {
    Serial.println("Can't open /");
    error(4);
  }
  
  strcpy(fileName, "GPS000.TXT");
  for (fileName[3] = '2'; fileName[3] <= '9'; fileName[3]++) {
    for (fileName[4] = '0'; fileName[4] <= '9'; fileName[4]++) {
      for (fileName[5] = '0'; fileName[5] <= '9'; fileName[5]++) {
        Serial.print("\ntrying to open ");Serial.println(fileName);
        f = card.open_file(fileName);
        if (!f)
          break;
        card.close_file(f);
      }
      if (!f)
        break;
    }
    if (!f)
      break;
  }
  Serial.println(fileName);
  
  if(!card.create_file(fileName)) {
    Serial.print("couldnt create ");
    Serial.println(fileName);
    error(5);
  }
  f = card.open_file(fileName);
  if (!f) {
    Serial.print("error opening ");
    Serial.println(fileName);
    card.close_file(f);
    error(6);
  }
  Serial.print("writing to ");
  Serial.println(fileName);
  Serial.println("ready!");
  
}


// blink out an error code
void error(uint8_t errno) {
  while(1) {
    for (int i=0; i<errno; i++) {
      digitalWrite(greenLED, HIGH);
      digitalWrite(redLED, HIGH);
      delay(100);
      digitalWrite(greenLED, LOW);
      digitalWrite(redLED, LOW);
      delay(100);
    }
    for (int i=0; i<10; i++) {
      delay(200);
    }
  }
}


void loop()			   // run over and over again
{
  digitalWrite(greenLED, HIGH);


  char *nmea_sentence;
  nmea_sentence = readline();
  Serial.println(nmea_sentence);


  char* stringToTokenize;
  stringToTokenize = (char *) malloc(BUFFSIZ);
 
  (void) strcpy(stringToTokenize, nmea_sentence);

  digitalWrite(greenLED, LOW);
  char* i;
  char* time; // 2
  char* latitude; // 3
  char* latDirection; // 4
  char* longitude; // 5
  char* longDirection; // 6
  char* fixQuality; // 7
  char* numSatellites; // 8
  char* HDOP; // 9
  char* altitude; // 10
  char* geoidHeight; // 11
  char* timeSinceLastDGPSUpdate; // 12
  char* DGPSreferenceStationID; // 13
  char* checkSum; // 14

  /*
    $GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
    1    = UTC of Position
    2    = Latitude
    3    = N or S
    4    = Longitude
    5    = E or W
    6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
    7    = Number of satellites in use [not those in view]
    8    = Horizontal dilution of position
    9    = Antenna altitude above/below mean sea level (geoid)
    10   = Meters  (Antenna height unit)
    11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and mean sea level.  -=geoid is below WGS-84 ellipsoid)
    12   = Meters  (Units of geoidal separation)
    13   = Age in seconds since last update from diff. reference station
    14   = Diff. reference station ID#
    15   = Checksum


  */

  if (strncmp(stringToTokenize, "$GPGGA",6) == 0) {  
    strtok_r(stringToTokenize,",", &i); // 1
    time = strtok_r(NULL,",", &i); // 2
    latitude = strtok_r(NULL,",", &i); // 3
    latDirection = strtok_r(NULL,",", &i); // 4
    longitude = strtok_r(NULL,",", &i); // 5
    longDirection = strtok_r(NULL,",", &i); // 6
    fixQuality = strtok_r(NULL,",", &i); // 7
    numSatellites = strtok_r(NULL,",", &i); // 8
    HDOP = strtok_r(NULL,",", &i); // 9
    altitude = strtok_r(NULL,",", &i); // 10
    geoidHeight = strtok_r(NULL,",", &i); // 11
    timeSinceLastDGPSUpdate = strtok_r(NULL,",", &i); // 12
    DGPSreferenceStationID = strtok_r(NULL,",", &i); // 13
    checkSum = strtok_r(NULL,",", &i); // 14
    
    unsigned long currentMillis = millis();
    
    if(atoi(fixQuality) > 0){ 
      digitalWrite(redLED, HIGH);
      
      Serial.println();
      Serial.print("Time: ");
      Serial.println(time);
      Serial.print("Latitude: ");
      Serial.print(latitude);
      Serial.println(latDirection);
      Serial.print("Longitude: ");
      Serial.print(longitude);
      Serial.println(longDirection);
      Serial.print("Altitude: ");
      Serial.println(altitude);
      Serial.print("Fix Quality: ");
      Serial.println(fixQuality);
      Serial.print("Logging: ");
      Serial.println(nmea_sentence);
      if(card.write_file(f, (uint8_t *) nmea_sentence, 100) != 100) {
        Serial.println("can't write!");
        return;
      }
       
      // create APRS packet from NMEA sentence
      if(currentMillis - previousMillis > interval) {  // only send APRS packet every 10 seconds
        previousMillis = currentMillis; // save last time APRS packet sent
        char *aprs_packet;
        aprs_packet = (char *) malloc(BUFFSIZ+14); // max nmea sentence received from GPS + 12 characters for callsign, APRS:
        (void) strcpy(aprs_packet, nmea_sentence);
        //  strcat("KC9SRV>APRS:", aprs_packet); // this line kills everything
        Serial.println(aprs_packet); // this will probably write the packet to pins 0 and 1 as well, which may be the way we have to hook up the shield.  
        free(aprs_packet);
      }
       
      // test for restricted airspace and cut-down if necessary
      if ( restrictedArea(latitude, latDirection, longitude, longDirection) ) { cutdown(); }
       
      // test for sunset and cut-down if neceesary. Defined here as 20:00+ but should be adjusted for proper format/time
      if ( time[0]-48 == 2 ){ cutdown(); } 
       
       
    }else{  // no GPS lock, turn red LED off and send a beacon APRS packet every 10 seconds 
      digitalWrite(redLED, LOW); 
      if(currentMillis - previousMillis > interval) {  // only send APRS packet every 10 seconds
        previousMillis = currentMillis; // save last time APRS packet sent
        char *aprs_packet;
        aprs_packet = (char *) malloc(BUFFSIZ); // max nmea sentence received from GPS + 12 characters for callsign, APRS:
        aprs_packet = "KC9SRV>APRS:somethingtoindicateweexist";
        //       Serial.println(aprs_packet); // this will probably write the packet to pins 0 and 1 as well, which may be the way we have to hook up the shield.
      }
  
    }
    
    
    
    /*  Serial.println(numSatellites);
        Serial.println(HDOP);
        Serial.println(geoidHeight);
        Serial.println(timeSinceLastDGPSUpdate);  
        Serial.println(DGPSreferenceStationID);
        Serial.println(checkSum); 
    */
  }


  free(nmea_sentence);
  free(stringToTokenize);
}
 
 
// return true if we are located within one of several defined geographical lat/lon boxes
boolean restrictedArea(char* latitude, char* latitudeDirection, char* longitude, char* longitudeDirection){
  long lat = long(latitude);
  long lon = long(longitude);
  
  // example Chicago bounds
  if (lat > 42.05 && lat < 42.10 && lon > 86.00 && lon < 87.00){ return true; }
  // Louisville, KY, Cinncinnatti, OH, Columbus, OH, St. Louis, MO, Chicago, IL, 
    
  
  
  return false;
}
 
// trigger relay to switch on Nichrome heating wire to cut down payload and end flight 
void cutdown(){
  return;
} 
 
char* readline(void) {

  char buffidx; // why is this not an int?

  char* readBuffer;
  readBuffer = (char *) malloc(BUFFSIZ);
  for (int i=0; i<BUFFSIZ; i++){ readBuffer[i]=0; }

  char c;
  buffidx = 0; 

  while (1) {
    c=gpsSerial.read();
    if (c == -1)
      continue;
    if (c == '\n')
      continue;
    if ((buffidx == BUFFSIZ-1) || (c == '\r')) {
      readBuffer[buffidx] = NULL;
      return readBuffer;
    }
    readBuffer[buffidx]= c;
    buffidx++;
  }
}

