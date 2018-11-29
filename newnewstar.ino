#include <Adafruit_NeoPixel.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Pin 6 is the data feed to the pixels
#define PIN 6
//Init ClickEncoder
ClickEncoder *encoder;
int16_t last, value;
//Init timer service
void timerIsr() {
  encoder->service();
}
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(26, PIN, NEO_GRB + NEO_KHZ800);
//Set a ton of counting/tracking variables so my stringly random code can survive through main loop cycles
int s = 0;
int z = 1;
int colorCounter = 0;
int colorState = 2;
int stage = 0;
int cycles = 0;
bool goSleep = false;
bool sleepWait = false;
bool autoCycle = true;
//Most of my own display modes reference two colors, these are the initial values
uint32_t theColor = strip.Color(255,255,255);
uint32_t theBackground = strip.Color(0,0,0);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  //Define Click encoder pins
  encoder = new ClickEncoder(A1, A0, A2);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr); 
  last = -1;

  //Serial.begin(9600);
}

void loop() {

//start by determining if button is held
  ClickEncoder::Button b = encoder->getButton();
  if (b == ClickEncoder::Held) {
       //Serial.println("Held!");
       //wake up if state is sleep and held
       if (goSleep == true) {
        goSleep = false;
        autoCycle = true;
        //Serial.println("Waking!");
        loop();
        }
       //if this is the second loop cycle with button held, trigger sleep mode
       if (sleepWait == true) {
         goSleep = true;
         sleepWait = false;
         colorWipe(strip.Color(0, 0, 0), 10);
         strip.show();
         delay(1000);
         //Serial.println("sleep now");
         loop();}
      //during the first loop cycle when button is held, switch autocycle and indicate w/ red      
      if (sleepWait == false) {
        autoCycle = !autoCycle;
        sleepWait = true;
        goSleep = false;
        colorWipe(strip.Color(255, 0, 0), 0);
        delay(1000);
        //Serial.println("auto off");
        loop();
        }
    }
  //if this is sleep mode, wait 4 seconds and start again (w button hold test)
  if (goSleep == true) {
    delay(4000);
    loop();}
  //if we're not asleep and the button isn't held, reset sleepWait so two cycles are necessary to trigger sleep
  if (sleepWait == true) {
      sleepWait = false;
     }
  //do one of these. this switch defines which of the little display functions run and in what order. advance between modes is automatic by default, or input from rotary encoder advances
  switch (stage) {
    case 0: 
     spiralChase(60);
      if (autoCycle == false) {nextColors();}
    break;
    case 1: 
      circleChase(100);
      if (autoCycle == false) {nextColors();}
    break;
    case 2:
      rise(100);
      if (autoCycle == false) {nextColors();}
    break;
    case 3:
      drop(100);
      if (autoCycle == false) {nextColors();}
    break;
    case 4:
     theaterChase(theColor, 60);
      nextColors();
    break;
    case 5:
      rainbow(10);
    break;
    case 6:
      rainbowCycle(20);
    break;
    case 7:
      theaterChaseRainbow(60);
    break;}
  //when autocycle is true, the Stage variable is advanced each time a display function completes, mod 8 to keep it rolling back to the start
  if (autoCycle == true) {
    //Serial.print("auto cycling!");
    //Serial.print(stage);
    //Serial.print("-");
    stage = (stage + 1) % 8;
    //Serial.println(stage);
      }
  //check encoder to see if it has changed
  value += encoder->getValue();
  //if we aren't auto-cycling, increment stage by encoder change
  if (value != last && autoCycle == false) {
  last = value;
  //Serial.print("moving!");
  //Serial.println(value);
  stage = value % 8;
  }
}

//That's it for the main loop. What follows are display functions, some used, some not, some completed, some not. Some of these are from the NeoPixel library example documentation, noted in readme

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void spiralChase(uint8_t wait) {
for(uint8_t g=0; g<20; g++){
  for(s=0; s<8; s++) {

    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i,theBackground);}
      strip.setPixelColor(0, theColor);
      strip.setPixelColor(25, theColor);
      strip.setPixelColor(s+1, theColor);
      strip.setPixelColor(((s+2)%8) + 9, theColor);
      strip.setPixelColor(((s+3)%8) + 17, theColor);
      
      strip.show();
      delay(wait);
  }
}
}

void circleChase(uint8_t wait) {
for(uint8_t g=0; g<20; g++){
  for(s=0; s<8; s++) {
      for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i,theBackground);}
      strip.setPixelColor(0, theColor);
      strip.setPixelColor(25, theColor);
      strip.setPixelColor(s+1, theColor);
      strip.setPixelColor(s+9, theColor);
      strip.setPixelColor(s+17, theColor);
      strip.setPixelColor(((s+4)%8)+1, theColor);
      strip.setPixelColor(((s+4)%8)+9, theColor);
      strip.setPixelColor(((s+4)%8)+17, theColor);
      
      strip.show();
      delay(wait);
  }
}
}
void flip(uint8_t wait) {
  }

void rise(uint8_t wait) {
for(uint8_t g=0; g<20; g++){
  for (s=1; s<6; s++) {
    for(int m=0; m<26; m++) {
      strip.setPixelColor(m, theBackground);
    }
    if (s==1) {strip.setPixelColor(0,theColor);}
    if (s==5) {strip.setPixelColor(24,theColor);}
    if (s>0 && s<5) {
      for(int q=1; q<9; q++) {
        strip.setPixelColor(s*8-8+q, theColor);
      }
    }
    strip.show();
    delay(wait);
    }
   }
}

void drop(uint8_t wait) {
  for(uint8_t g=0; g<20; g++){
  for (s=5; s>0; s--) {
    for(int m=0; m<26; m++) {
      strip.setPixelColor(m, theBackground);
    }
    if (s==1) {strip.setPixelColor(0,theColor);}
    if (s==5) {strip.setPixelColor(24,theColor);}
    if (s>0 && s<5) {
      for(int q=1; q<9; q++) {
        strip.setPixelColor(s*8-8+q, theColor);
      }
    }
    strip.show();
    delay(wait);
    }
   }
  }
void randstar(uint8_t wait) {
  }    

//an attempt at some automated color changes within my functions
void nextColors() {
  switch(colorState) {
    case 1:
      if (z>120) {
      z=1;
      } else {
        z++;
      }
      theColor = strip.Color(z,z,255);
      theBackground = strip.Color(min(150-z,0),0,min(255-z,0));
      break;
    case 2:
      theColor = colorTable(colorCounter);
      theBackground = colorTable(colorCounter-1);
      if (colorCounter == 14) {
        colorCounter = 0;} else{
      colorCounter++;
        }
      break;
    }
  }
uint32_t colorTable(uint8_t whichCase) {
 switch(whichCase) {
  case 1:
  return strip.Color(255,0,0);
  
  case 2:
  return strip.Color(0,255,0);

  case 3:
  return strip.Color(0,0,255);
 
  case 4:
  return strip.Color(255,255,0);

  case 5:
  return strip.Color(0,255,255);

  case 6:
  return strip.Color(6,133,135);
  
  case 7:
  return strip.Color(79,185,142);
  
  case 8:
  return strip.Color(242,177,52);
     
  case 9:
  return strip.Color(237,85,59);
  
  case 10:
  return strip.Color(240,243,189);
     
  case 11:
  return strip.Color(2,128,144);
     
  case 12:
  return strip.Color(243,203,93);
     
  case 13:
  return strip.Color(168,55,56);
     
  case 14:
  return strip.Color(3,165,150);
 }
 }

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*3; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<20; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j=j+2) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
