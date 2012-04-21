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


int outputLED = 13;
long ditLength = 100;
int speakerPin = 9;
long toneFreq = 1915;
int turnTXon = 12;
char myString[] = "kcrsrv";
  
 void setup(){
   Serial.begin(9600);
   pinMode(speakerPin, OUTPUT);
   pinMode(turnTXon, INPUT);           // set pin to input
   digitalWrite(turnTXon, HIGH); 
   
 }
 
 void loop(){
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
