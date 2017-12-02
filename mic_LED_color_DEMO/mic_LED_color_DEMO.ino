//#include <AudioFrequencyMeter.h>
//#include <sam.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//PINS
#define trigPin1 6
#define echoPin1 A6
#define trigPin2 7
#define echoPin2 A5
#define mic1 A2
#define mic2 A3

//CONSTANTS
int PIXNUM = 60;
int beaconDelay = 40;    //time between for loop for beacon glow
int redDelay=1100;        //starting delay between red spots
int redDecrease=200;       //decrease step to get spots faster
int levelDelay = 50;      //time to fade out in white level mapping
long LOWTHRESH = 4 ;
long HIGHTHRESH = 200;
int STATE = 0;    //3 total states: 0 = above threshold, glowing white beacon
                  //                1 = between 2 thresholds, mapping to levels and freq
                  //                2 = below threshold, red spots
int redCounter=0;           //keep track of how long environment is too active
   int freq = 50; //FAKE VALUE TO TEST

//gloable variables
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
unsigned int sample2;
long distance1;
long distance2;
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(PIXNUM, 2, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(PIXNUM, 3, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(PIXNUM, 4, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip4 = Adafruit_NeoPixel(PIXNUM, 5, NEO_GRBW + NEO_KHZ800);
//AudioFrequencyMeter meter1;
//AudioFrequencyMeter meter2;

//COLORS
uint32_t purple = strip1.Color(155, 149, 216, 0);
uint32_t babyBlue = strip1.Color(181, 223, 224, 0);
uint32_t yellow = strip1.Color(255, 245, 0, 0);
uint32_t teal = strip1.Color(119, 237, 239, 0);
uint32_t currentColor=strip1.Color(200, 0, 200, 0); //starting color for maping


//--------------------------------------------------------------
void setup() {
  Serial.begin (9600);
  Serial.println("Start!");

  //sensor pins
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(mic1, INPUT);
  pinMode(mic2, INPUT);

  //LED init
  strip1.begin();
  strip2.begin();
  strip3.begin();
  strip4.begin();
  showAll(); // Initialize all pixels to 'off'

  //Init Audio meters
  //COMMENTED OUT UNTIL WE FIGURE OUT WHY THIS LIBRARY CRASHES ARDUINO
  /*meter1.begin(mic1, 2000);             // Intialize A3 at sample rate of 45kHz
  meter1.setBandwidth(70.00, 20000);    // Ignore frequency out of this range
  meter2.begin(mic2, 2000);              // Intialize A3 at sample rate of 45kHz
  meter2.setBandwidth(70.00, 20000);    // Ignore frequency out of this range
  */

  Serial.println("Setup complete.");

}

//--------------------------------------------------------------
void loop() {
   //check distance and determine state
   distance1 = getSonar(trigPin1, echoPin1);
   distance2 = getSonar(trigPin2, echoPin2);

//USE DISTANCE TO CHANGE STATES, COMMENTED OUT FOR NOW FOR DEMO, JUST GO FROM 1 STATE TO ANOTHER AFTER SOME TIME
 // if (distance1 < HIGHTHRESH || distance2 < HIGHTHRESH){
    //something is whithin range
   // if (distance1 < LOWTHRESH || distance2 < LOWTHRESH){
      //TOO close, ENTER RED SPOTS STATE
        Serial.println("Enter red state");
        STATE = 2;
        redSpots();
        delay(15000); //TIME DELAY FOR DEMO ONLY, IN MILLISECS
  //  }
 //   else{
      //in between thresholds, enter mapping state
        STATE = 1;
        Serial.println("enter mapping state");
      
      //2-respond to levels for 30 seconds
      for (int i=0; i<40; i++){ //FOR LOOP ONLY DURING DEMO,
                                //MAPWHITELEVELS METHOD IS NORMALLY CALLED WITHIN THE MAPPING METHOD
        int level1 = getMicLevel(mic1);
        int level2 = getMicLevel(mic2);
        mapWhiteLevels(level1, &strip1, &strip2);
        mapWhiteLevels(level2, &strip3, &strip4);
      }
        //show color fades
      Serial.println("Show fade patterns");
      for(int i=0; i<30; i++){ //FOR LOOP ONLY DURING DEMO 
        mapping();
        delay(200);
      }
     // }
    //}
 // }
 // else {
    //not close enough, enter zero state
     STATE = 0;
     Serial.println("enter zero state");
     for(int i=0; i<20; i++){  //FOR LOOP ONLY FOR DEMO
      beaconGlow();
      delay(200);
     }
 // }
}

//STATE METHOD DECLARATIONS ------------------------------------------
void mapping(){

   //1-map white to levels //COMMENTED OUT FOR DEMO
   /*int level1 = getMicLevel(mic1);
   int level2 = getMicLevel(mic2);
  //TODO change setpixel so that white is MASKED on existing colors.
  mapWhiteLevels(level1, &strip1, &strip2);
  mapWhiteLevels(level2, &strip3, &strip4);

  //2-map colors to freq //COMMENTED OUT CUZ THIS LIBRARY CRASHES ARDUINO
  //float freq = meter1.getFrequency();
  //freq = (freq + meter2.getFrequency())/2;
  */
  currentColor = mapColorFreq(freq, currentColor);  
  // MANUALLY CHANGE FREQ TO AVOID USING CRASHY LIBRARY FOR DEMO
  if (freq>=1000){
    freq = 0;
  } else {
  freq = freq + 200;
  }
}

void beaconGlow(){
  Serial.println("in beacon glow");
  for(int i=0; i<PIXNUM; i++){
    //move the lit led up
    strip1.setPixelColor(i, strip1.Color(0, 0, 0, 110));
    strip2.setPixelColor(i, strip1.Color(0, 0, 0, 110));
    strip3.setPixelColor(i, strip1.Color(0, 0, 0, 110));
    strip4.setPixelColor(i, strip1.Color(0, 0, 0, 110));
    showAll();
    if((i>5) && i<(PIXNUM-4)){
      //add fading trail behind moving led
      setWhiteLag(i, &strip1);
      setWhiteLag(i, &strip2);
      setWhiteLag(i, &strip3);
      setWhiteLag(i, &strip4);
      showAll();
    }
    //TODO: add more waves at same time
    delay(beaconDelay);
  }

  for(int i=110; i<200; i++){
    setGroupColor((PIXNUM-10), PIXNUM, strip1.Color(0,0,0,i));
    showAll();
    i=i+5;
    delay(10);
  }
  for(int i=200; i>=0; i--){
    setGroupColor((PIXNUM-10), PIXNUM, strip1.Color(0,0,0,i));
    showAll();
    i=i-3;
    delay(10);
  }
  
    Serial.println("beacon done");
}

void redSpots(){
  Serial.println("red spots triggered");
  redDelay=1100;
  allOff();
  //prepare the random generator
  randomSeed(millis());
  int p=0;

  for(int i=0; i<(int)(PIXNUM/5); i++){
    //strip 1
    p=random(PIXNUM);
    bleed(p, &strip1);

    //strip2
    p=random(PIXNUM);
    bleed(p, &strip2);

    //strip3
    p=random(PIXNUM);
    bleed(p, &strip3);

    //strip4
    p=random(PIXNUM);
    bleed(p, &strip4);
    Serial.println(String("   i: ")+i+String(", out of ")+(int)(PIXNUM/5));

    delay(redDelay);
    if(redDelay>redDecrease){
      redDelay=(redDelay-redDecrease);
    }
  }  
}

//HELPER METHODS------------------------------------------------
long getSonar(int trigPin, int echoPin) {
  long duration, distance;

 // do{
   //send pulse to sensor 
    digitalWrite(trigPin, LOW);  
    delayMicroseconds(2); 
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10); 
    digitalWrite(trigPin, LOW);
   //check echo of sensor
    duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) *0.0343;
 // }while(distance<=0 || distance>400);
  
  /*if (distance >= 200 || distance <= 0){
    Serial.println("Sonar Out of range");
  }
  else {*/
    Serial.print(echoPin + String(":  ") + distance);
    Serial.println(" cm, ");
  //}

  delay(100);
  return distance;
}

int getMicLevel(int mic) {
  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(mic);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }

   //sample collection over, now analyze peaks  
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   //double volts = (peakToPeak * 3.3) / 1024;  // convert to volts

   Serial.println(mic + String(" :   ")+ peakToPeak + String("v"));
   delay(100);
   return peakToPeak;
}


void setGroupColor(int first, int last, uint32_t color){
  for(int i=first; i<=last; i++){
    strip1.setPixelColor(i, color);
    strip2.setPixelColor(i, color);
    strip3.setPixelColor(i, color);
    strip4.setPixelColor(i, color);
  }

}

void setWhiteLag(int start, Adafruit_NeoPixel *meter){
  //Serial.println("in white lag function");
  meter->setPixelColor((start-3), strip1.Color(0, 0, 0, 65));
  meter->setPixelColor((start-4), strip1.Color(0, 0, 0, 30));
  meter->setPixelColor((start-5), strip1.Color(0, 0, 0, 15));
  meter->setPixelColor((start-6), strip1.Color(0, 0, 0, 0));
}

void showAll(){
  strip1.show();
  strip2.show();
  strip3.show();
  strip4.show();
}

void allOff(){
  for(int i=0; i<PIXNUM; i++){
    strip1.setPixelColor(i, strip1.Color(0,0,0,0));
    strip2.setPixelColor(i, strip1.Color(0,0,0,0));
    strip3.setPixelColor(i, strip1.Color(0,0,0,0));
    strip4.setPixelColor(i, strip1.Color(0,0,0,0));
  }
  showAll();
}

void bleed(int index, Adafruit_NeoPixel *meter){
  meter->setPixelColor(index, strip1.Color(255,0,0,0));
  meter->show();
  delay(10); //delay a bit for bleed effect
  if( index<(PIXNUM-1) && index>0 ){
    for(int i=10; i<150; i++){
      meter->setPixelColor((index-1), strip1.Color(i,0,0,0));
      meter->setPixelColor((index+1), strip1.Color(i,0,0,0));
      i=i+3;
      meter->show();
    }
  }
  Serial.println(String("bled on ")+index);
}

void mapWhiteLevels(int val, Adafruit_NeoPixel *meter1, Adafruit_NeoPixel *meter2){
 //TODO: maybe check prev level to do fadeout over various loops
 //analyze level
 int maxLED=0;
 if( val>50 && val<199){
  maxLED=30;
 }
 if( val>200 && val<299){
  maxLED=90;
 }
 if( val>300 && val<499){
  maxLED=110;
 }
 if( val>500 && val<799){
  maxLED=140;
 }
 if( val>800){
  maxLED=180;
 }

 Serial.println(String("level will set white to: ") + maxLED);
  //set white level according to sound level
 setAllStrip(strip1.Color(0,0,0,maxLED), meter1);
 setAllStrip(strip1.Color(0,0,0,maxLED), meter2);
 showAll();
 //Serial.println(String(".......")+millis());
 delay(80);
 //Serial.println(String("lets fade")+millis());

  //fade out before returning
 for(int i=maxLED; i>=0; i--){
  setAllStrip(strip1.Color(0,0,0,i), meter1);
  setAllStrip(strip1.Color(0,0,0,i), meter2);
  meter1->show();
  meter2->show();
  i=i-(maxLED/5);
  delay(levelDelay);
 }
 
}

void setAllStrip(uint32_t c, Adafruit_NeoPixel *meter) {
  //ONLY SETS strip color, need to show all after.
  for(uint16_t i=0; i<PIXNUM; i++) {
    meter->setPixelColor(i, c);
  }
}

uint32_t mapColorFreq(float f, uint32_t color1){
  uint32_t color2;
  String Cname;

  //pick color based on freq
  if(f<100){
    //low freq
    color2=yellow;
    Cname="yellow";
  }
  else if( f<300){
    color2=teal;
    Cname="teal";
  }
  else if( f<500){
    color2=babyBlue;
    Cname="babyblue";
  }
  else if( f<700){
    color2=purple;
    Cname="purple";
  }
  else if( f<900){
    color2=strip1.Color(200, 0, 0);
    Cname="red";
  }


  Fade(color1, color2, 30);
  delay(1000);
  Serial.println(Cname);

  return color2;
}

void Fade(uint32_t color1, uint32_t color2, uint16_t steps){
     uint8_t red;
     uint8_t green;
     uint8_t blue;
     Serial.println("fading..");
     for(int i=0; i<steps; i++){
        //get individual 3 components from colors, divide the difference in number of steps and increment 
        red = ((((uint8_t)(color1 >> 16)) * (steps - i)) + ((uint8_t)(color2 >> 16)) * i) / steps;
        green = ((((uint8_t)(color1 >>  8)) * (steps - i)) + ((uint8_t)(color2 >>  8)) * i) / steps;
        blue = (((uint8_t)color1 * (steps - i)) + ((uint8_t)color2 * i)) / steps;
        setGroupColor(0, PIXNUM,strip1.Color(red, green, blue, 0));
        showAll();
        delay(20);
     }
        
}

