int counter = 1;


int turnTXOnPin = 12;

void setup()
{
  pinMode(turnTXOnPin, INPUT);
  digitalWrite(turnTXOnPin, HIGH);
  Serial.begin(4800);                      //OPEN SERIAL LINE AT 4800
  delay(3);
  Serial.print("MW1AW\r\n");               //SET YOUR CALLSIGN HERE, HERE YOU SEE W1AW
  delay(10);                       
  Serial.print("PWIDE1-1,WIDE2-1\r\n");    //SET DIGIPATH HERE
  delay(10);
  Serial.print("L150\r\n");
  delay(10);
}

void loop()
{
  Serial.print("!>This is RadioShield test message #");     //BEGIN MESSAGE BUT DON'T SEND YET...
  Serial.print(counter);                                    //  ...CONCATENATE VALUE OF count TO OUTPUT...
  Serial.print("\r\n");                                     //    ...SEND CR/LF TO COMPLETE AND TRANSMIT PACKET.
  counter++;
  delay(5000);                                             //30,000ms = 30sec

 // Serial.print("@>This is RadioShield test message #");     //BEGIN MESSAGE BUT DON'T SEND YET...
 //  Serial.print(counter);                                    //  ...CONCATENATE VALUE OF count TO OUTPUT...
 //  Serial.print("\r\n");                                     //    ...SEND CR/LF TO COMPLETE AND TRANSMIT PACKET.
 //  counter++;
 //  delay(30000);                                             //30,000ms = 30sec
}

