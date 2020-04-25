// HG 2016/11/26: need to rewrite with cooperative multitasking in mind.  invoke the Ewing coding practices!
// this probably means each program has to be much shorter, or at least check for interrupts frequently, 
// and if an interrupt is found, be able to break out of the program and into a new one.



#include "FastLED.h"

#define ledPin 5            // GPIO pin
#define lightStripPin 32    // WS2812 light strip pin
#define numLights 60        // number of WS2812 lights
#define analogPin 35  // Analog input pin , ours is connected to YL-83
#define numTouchPins 4    // number of touch pads/pins
#define numTouchAveraging 4          // moving average filter is this size
#define touchAvgThreshold 200        // theshold value
#define timeLedUpdate 600

// global variables
const byte ledStartAddress = 0;      // first used led in string
const byte ledEndAddress = 59;       // last used led in string
byte ledCountIn = 0;                // for moving LEDs
unsigned long timeNow;              // * consider moving into 'loop()'. will have to think about how to set initial value and updates.
unsigned long timePrev;             // * consider moving into 'loop()'
byte brightnessDivIn = 8;              // brightness divider
byte touchPins[] = { T4, T5, T6, T7 };
byte touch[numTouchPins][numTouchAveraging] = {};
int touchAvg[numTouchPins] = {};
int touchReadTime = 50;
int touchUpdateTime = 2000;
unsigned long timeCapPrev;          // * considered moving into 'loop() like all the comments above but it doesn't work.  gets reinitialized every time.  Will deal with extra vars in mem.
unsigned long timeTouchPrev;
unsigned long timeAdvancedPrev;

CRGB leds[numLights];

// function declarations
void touchPadRead(byte [numTouchPins][numTouchAveraging], int [numTouchAveraging]);
byte touchPadUpdate(int touchAvg[numTouchPins]);

////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  delay(100);
  randomSeed(analogRead(analogPin));

  int timeCapNow = millis();
  timeCapPrev = 0;
  timeTouchPrev = 0;

  FastLED.addLeds<NEOPIXEL, lightStripPin>(leds, numLights);  // GRB ordering is assumed
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()   // This will run the cascade from 'ledStartAddress' to 'ledEndAddress' to 'ledStartAddress' 20 times
{
  byte touchValue;
  byte ledBrightDiv = 10;

// LED section
  timeNow = millis();
  if (timeNow - timePrev > timeLedUpdate) {
    timePrev = timeNow;         // reset the timer
    ledSetPixels();
  } // if

// TOUCHPAD section
  if (timeNow - timeCapPrev > touchReadTime) {
    timeCapPrev = timeNow;
    touchPadRead(touch, touchAvg);
    touchValue = touchPadUpdate(touchAvg);
  } // if
} // loop

void ledSetPixels()
{
  byte colorRed = 120 / brightnessDivIn;
  byte colorGrn = 120 / brightnessDivIn;
  byte colorBlu = 120/ brightnessDivIn;


//  for (byte ledCount=ledStartAddress; ledCount<=ledEndAddress; ledCount++) {
  for (byte ledCount=0; ledCount<=numLights; ledCount++) {
    leds[ledCount].r = colorRed;
    leds[ledCount].g = colorGrn;
    leds[ledCount].b = colorBlu;
//    leds[ledCount] = 0x000000;
  } // for

// move an LED across the strip
/*  ledCountIn++;
  if (ledCountIn==numLights){
    ledCountIn=0;}
  leds[ledCountIn].r = colorRed;leds[ledCountIn].g = colorGrn;leds[ledCountIn].b = colorBlu;
*/

  FastLED.show();
  Serial.print("time="); Serial.print(millis()); Serial.print(", ledCountIn="); Serial.println(ledCountIn);
} // void ledSetPixels(byte)

void touchPadRead(byte touch[numTouchPins][numTouchAveraging], int touchAvg[numTouchPins])    // reads from touchPad, stores, and averages touch values
{
  // first, set all touchAvg values to zero
  for ( byte pin=0; pin<numTouchPins; pin++ ) {
    touchAvg[pin] = 0;
  }
  // next, read touch values, increment the shift register, and set touchAvg values
  for( byte avgCount = 0; avgCount < numTouchAveraging; avgCount++){
    for ( byte pin=0; pin<numTouchPins; pin++ ) {
      if (avgCount==numTouchAveraging-1) { // last value
          touch[pin][avgCount] = touchRead( touchPins[pin] );
          if (touch[pin][avgCount] < 8) {
            touch[pin][avgCount] = 100;
          }
      }
      else {
        touch[pin][avgCount] = touch[pin][avgCount+1];
      }
      touchAvg[pin] = touchAvg[pin] + int(touch[pin][avgCount]);
    }
  }
}

byte touchPadUpdate(int touchAvg[numTouchPins])    // checks touchpad values for touch
{
  byte touchValue = 0;
  for (byte pinCount=0; pinCount<numTouchPins; pinCount++) {
    if (touchAvg[pinCount] < touchAvgThreshold) {
      touchValue = pinCount+1;
    }
  }
  if (touchValue > 0) {
    digitalWrite(ledPin, HIGH);   // turn the blue LED on to signify a touch

    if (timeNow - timeTouchPrev > touchUpdateTime) {    // prevents a touch from quickly cyling through values
      timeTouchPrev = timeNow;
      Serial.print( timeTouchPrev );Serial.print( ", \t" );
      Serial.print( touchValue );Serial.print( ", \t" );
  
      switch (touchValue) {
        case 1: {
          Serial.print( "touchValue=" );Serial.print( touchValue);
          brightnessDivIn = 1;
          break;
        }
        case 2: {
          Serial.print( "touchValue=" );Serial.print( touchValue);
          brightnessDivIn = 3;
          break;
        }
        case 3: {
          Serial.print( "touchValue=" );Serial.print( touchValue);
          brightnessDivIn = 9;
          break;
        }
        case 4: {
          Serial.print( "touchValue=" );Serial.print( touchValue);
          brightnessDivIn = 27;
          break;
        }
      }
      Serial.println();
    }
  }
  else {
    timeTouchPrev = timeNow - touchUpdateTime; // allows a quick succession of touches to quickly change values
    digitalWrite(ledPin, LOW);   // turn the LED on (HIGH is the voltage level)    
  }
  return touchValue;
}
