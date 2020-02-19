
//#include <AudioFrequencyMeter.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//PINS
#define trigPin1 6
#define echoPin1 9
#define trigPin2 7
#define echoPin2 10
#define mic1 A2
#define mic2 A3

//CONSTANTS
int PIXNUM = 60;
int beaconDelay = 40;     //time between for loop for beacon glow
int redDelay=1100;        //starting delay between red spots
int redDecrease=200;      //decrease step to get spots faster
int levelDelay = 50;      //time to fade out in white level mapping (was 50)
int beaconCount=0;
long LOWTHRESH = 10 ;     //(was 10)
long HIGHTHRESH = 200;  //(was 200)
int QUIETTHRESH=10; //was 32
int LOUDTHRESH=120;  //was 95
int STATE = 1;    //3 total states: 0 = above threshold, glowing white beacon
                  //                1 = between 2 thresholds, mapping to levels and freq
                  //                2 = below threshold, red spots
int redCounter=0;           //keep track of how long environment is too active
int freq = 50;              //FAKE VALUE TO TEST
int COLORSTEP = 50;         // number of steps to fade from one color to another (was 50)
int WHITESTEP= 4;          //multiple within steps of color fade to stop and check white levels (was 10)
int COLORHOLD=10;         //delay to hold color before moving on (was 1000)

//gloable variables
const int sampleWindow = 10; // Sample window width in mS (50 mS = 20Hz) (was 150)
unsigned int sample;
unsigned int sample2;
long distance1;
long distance2;
int sensorswitch = 1;
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(PIXNUM, 2, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(PIXNUM, 3, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(PIXNUM, 4, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip4 = Adafruit_NeoPixel(PIXNUM, 5, NEO_GRBW + NEO_KHZ800);

//AudioFrequencyMeter meter;
//COLORS
uint32_t fuscia = strip1.Color(144,0,175,0);
uint32_t purple = strip1.Color(100, 0, 255, 0);
uint32_t babyBlue = strip1.Color(50, 40, 255, 0);
uint32_t teal = strip1.Color(0, 230, 60, 0);
uint32_t green = strip1.Color(0,75,15,0);
uint32_t yellow = strip1.Color(250, 150, 10, 0);
//new hues
uint32_t orange = strip1.Color(251, 171, 105, 0);
uint32_t coral = strip1.Color(222, 113, 130, 0);
uint32_t indigo = strip1.Color(16, 3, 134, 0);
uint32_t crimson = strip1.Color(237, 31, 12, 0);

uint32_t currentColor=fuscia; //starting color for mapping


//--------------------------------------------------------------
void setup() {
  Serial.begin (115200);
  Serial.println("Start!");

  //sensor pins
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  //pinMode(A2, INPUT);
  //pinMode(mic2, INPUT);

  //LED init
  strip1.begin();
  strip2.begin();
  strip3.begin();
  strip4.begin();
  showAll(); // Initialize all pixels to 'off'

  Serial.println("Setup complete.");
 // meter.setBandwidth(70.00, 1500);    // Ignore frequency out of this range
  //meter.begin(A4, 45000);

}

//--------------------------------------------------------------
void loop() {
//NORMALLY USE DISTANCE TO CHANGE STATES, COMMENTED OUT FOR NOW FOR TESTS, JUST GO FROM 1 STATE TO ANOTHER AFTER SOME TIME
  //check distance and determine state
  distance1 = getSonar(trigPin1, echoPin1);
  distance2 = getSonar(trigPin2, echoPin2);
  int level1=getMicLevel(mic1);
  int level2=getMicLevel(mic2);

  // if((level1>LOUDTHRESH || level2>LOUDTHRESH) && (distance1<LOWTHRESH || distance2<LOWTHRESH)){
  // //TOO close, ENTER RED SPOTS STATE
  //   Serial.println("Enter red state");
  //   redSpots();
  //   //delay(10000); //TIME DELAY FOR DEMO ONLY
  // }
  // else if((distance1 < 330 && distance1 > 40) || (distance2 < 700 && distance2 > 40)|| level1>QUIETTHRESH || level2>QUIETTHRESH){
  //
  //   //in between thresholds, enter mapping state
      STATE = 1;
      Serial.println("enter mapping state");

      //for(int i=0; i<10; i++){ //FOR LOOP ONLY DURING DEMO
      mapping(); //will set color to freq and white to levels
     // delay(50);
     // }
    //}
    //}
 // }
    // else {
    // //not close enough, enter zero state
    // //TODO: change this to audio thresh
    //  STATE = 0;
    //  if(beaconCount>5){
    //   Serial.println("enter zero state");
    //   beaconCount=0;
    //   beaconGlow();
    //  }
    //  else{
    //   beaconCount=beaconCount+1;
    //   Serial.println(String("Beacon count: ")+beaconCount);
    //  }
  //}
}

//STATE METHOD DECLARATIONS ------------------------------------------
void mapping(){
  Serial.println("inside mapping");

  //1-map colors to mic1 //COMMENTED OUT CUZ THIS LIBRARY CRASHES ARDUINO
  if (sensorswitch==1){
    currentColor = mapColorLevel(getSonar(trigPin1, echoPin1), currentColor);
    sensorswitch=0;
  } else{
    currentColor = mapColorLevel(getSonar(trigPin2, echoPin2), currentColor);
    sensorswitch=1;
  }
delay(0);
  //2-map white to levels, each mic controls 2 strips
  //Serial.println("set whites again");
  //mapWhiteLevels(getMicLevel(mic2), &strip1, &strip2);
  //mapWhiteLevels(getMicLevel(mic2), &strip3, &strip4);
}

// void beaconGlow(){
//   Serial.println("in beacon glow");
//   //TODO: how to get second wave going?
//   for(int i=0; i<PIXNUM; i++){
//     strip1.setPixelColor(i, strip1.Color(0, 0, 0, 110));
//     strip2.setPixelColor(i, strip1.Color(0, 0, 0, 110));
//     strip3.setPixelColor(i, strip1.Color(0, 0, 0, 110));
//     strip4.setPixelColor(i, strip1.Color(0, 0, 0, 110));
//
//     //add fading trail
//     if((i>5) && i<(PIXNUM-4)){
//       //Serial.print(i +String(", "));
//       setWhiteLag(i, &strip1);
//       setWhiteLag(i, &strip2);
//       setWhiteLag(i, &strip3);
//       setWhiteLag(i, &strip4);
//     }
//     showAll();
//     /*if( (i%WHITESTEP)==0 ){
//       if(getMicLevel(mic1)>QUIETTHRESH && getMicLevel(mic2)>QUIETTHRESH){
//         allOff();
//         return ;
//       }
//     }else{*/
//     delay(beaconDelay);
//     //}
//   }
//
//   //add glow burst at the tip
//   for(int i=110; i<200; i++){
//     setGroupColor((PIXNUM-10), PIXNUM, strip1.Color(0,0,0,i));
//     showAll();
//     i=i+5;
//     delay(10);
//   }
//   for(int i=200; i>=0; i--){
//     setGroupColor((PIXNUM-10), PIXNUM, strip1.Color(0,0,0,i));
//     showAll();
//     i=i-3;
//     delay(10);
//   }
//
//     Serial.println("beacon done");
//     //return 0;
// }
//
// void redSpots(){
//   Serial.println("red spots triggered");
//   redDelay=1100;
//   allOff();
//   //prepare the random generator
//   randomSeed(millis());
//   int p=0;
//
//   for(int i=0; i<(int)(PIXNUM/5); i++){
//     //strip 1
//     p=random(PIXNUM);
//     bleed(p, &strip1);
//
//     //strip2
//     p=random(PIXNUM);
//     bleed(p, &strip2);
//
//     //strip3
//     p=random(PIXNUM);
//     bleed(p, &strip3);
//
//     //strip4
//     p=random(PIXNUM);
//     bleed(p, &strip4);
//     Serial.println(String("   i: ")+i+String(", out of ")+(int)(PIXNUM/5));
//
//     delay(redDelay);
//     if(redDelay>redDecrease){
//       redDelay=(redDelay-redDecrease);
//     }
//   }
// }

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

  delay(50);
  return distance;
}

int getMicLevel(int mic) {
  //Serial.println(String(" getting levels from ") +mic);
  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(A3);
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
 //analyze level
 //Serial.println(String("  mapping this to white:  ") + val);
 uint8_t maxLED=0;
 if( val>25 && val<40){
  maxLED=30;
 }
 if( val>40 && val<55){
  maxLED=90;
 }
 if( val>55 && val<70){
  maxLED=110;
 }
 if( val>70 && val<90){
  maxLED=140;
 }
 if( val>90){
  maxLED=180;
 }

// Serial.println(String("level will set white to: ") + maxLED);
 //set white level according to sound level

 //we assume all strips have same color on all the strip from freq mapping,
 //so get color from any pixel, and mask white over it so it doesnt change color.
 uint32_t colorToMask = strip1.getPixelColor(10);
 uint8_t b = (uint8_t)colorToMask;
 uint8_t g = (uint8_t)(colorToMask>>8);
 uint8_t r = (uint8_t)(colorToMask>>16);
 //Serial.print(String(" from color: (")+ r +String(", ")+ g +String(", ")+ b +String(", 0)"));
 colorToMask = (maxLED << 24) | colorToMask;
// Serial.println(String(" --> ")+colorToMask);

 setAllStrip(colorToMask, meter1);
 setAllStrip(colorToMask, meter2);
 showAll();

 //fade out before returning
 for(int i=maxLED; i>=0; i--){
  //Serial.println(i);
  setAllStrip(strip1.Color(r,g,b,i), meter1);
  setAllStrip(strip1.Color(r,g,b,i), meter2);
  meter1->show();
  meter2->show();
  if(i<7){ //to avoid setting i to negative, and ensure strip fades to 0
    i=0;
  }else{
    i=i-(maxLED/5);
  }
  delay(levelDelay);
 }

}

void setAllStrip(uint32_t c, Adafruit_NeoPixel *meter) {
  //ONLY SETS strip color, need to show all after.
  for(uint16_t i=0; i<PIXNUM; i++) {
    meter->setPixelColor(i, c);
  }
}

uint32_t mapColorLevel(int f, uint32_t color1){
  uint32_t color2;
  String Cname;
  Serial.println("In mapColorLevel");

  //pick color based on level
  if(f<(LOWTHRESH+10)){
    //low freq
    color2=fuscia;
    Cname="fuscia";
  }
  else if( f<50){
    color2=purple;
    Cname="purple";
  }
  else if( f<100){
    color2=babyBlue;
    Cname="babyBlue";
  }
  else if( f<150){
    color2=teal;
    Cname="teal";
  }
  else if( f<200){
    color2=green;
    Cname="green";
  }
  else if( f>250){
    color2=yellow;
    Cname="yellow";
  }
  else if( f>250){
    color2=orange;
    Cname="orange";
  }
  else if( f>250){
    color2=coral;
    Cname="coral";
  }
  else if( f>250){
    color2=indigo;
    Cname="indigo";
  }
  else if( f>250){
    color2=crimson;
    Cname="crimson";
  }


  Serial.println("Chosen color is: " + Cname);
  Fade(color1, color2, COLORSTEP);
  Serial.println("wait...");
  delay(COLORHOLD);

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
        //Serial.println( red + String(", ")+green + String(", ")+blue);
        setGroupColor(0, PIXNUM,strip1.Color(red, green, blue, 0));
        showAll();
        //get some white level motion while fading?
        if( (i%WHITESTEP)==0 ){
          //Serial.println(" now go to whites");
          mapWhiteLevels(getMicLevel(mic2), &strip1, &strip2);
          mapWhiteLevels(getMicLevel(mic2), &strip3, &strip4);
        }else{
        delay(20);
        }
     }

}
