
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

int PIN2 = 3; //attached to led strip
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(60, PIN2, NEO_GRBW + NEO_KHZ800);
byte byteRead; //byte we will read from serial
int r=0;
int g=0;
int b=0;
int track=0; //track if u r reading first, second, or third number

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Initializing Strips");
  strip1.begin();
  strip1.show();// Initialize all pixels to 'off'

}

void loop() {
  // put your main code here, to run repeatedly:
 /*serial will read every character as ascii
  * 48 - 0.... 57 - 9
  */
  while (Serial.available() ){
    byteRead = Serial.read();
    Serial.println(String("Read byte: ")+byteRead);
    if(byteRead>47 && byteRead<58){
      //byte is a digit
      switch(track){
        case 0: //red
          r=(r*10)+(byteRead-48);
          break;
        case 1: //green
          g=(g*10)+(byteRead-48);
          break;
        default: //blue
          b=(b*10)+(byteRead-48);
          break;
      }
    }

    if(byteRead == 44){
      //byte is a comma
      if(track<2){
        track=track+1;
      }
      Serial.println(track);
    }
  }
    
    if(track>0){
      Serial.print("Setting color ");
      Serial.println(r + String(", ")+g + String(", ")+b);
      
      for(int x =0; x<30; x++) {
        strip1.setPixelColor(x, strip1.Color(r,g,b,0));
        strip1.show();
        delay(20);
      }
      delay(1000);
      track=0;
      r=0;
      g=0;
      b=0;
    }
}
