#include <Adafruit_NeoPixel.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

ClickEncoder *encoder;
int16_t last, value;

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
int s = 0;
int z = 1;
int colorCounter = 0;
int colorState = 0;
int stage = 0;
int cycles = 0;
bool goSleep = false;
bool sleepWait = false;
bool autoCycle = true;
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
  encoder = new ClickEncoder(A1, A0, A2);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr); 
  last = -1;

  //Serial.begin(9600);
}

void loop() {
  // Some example procedures showing how to display to the pixels:
//  colorWipe(strip.Color(255, 0, 0), 500); // Red
//  colorWipe(strip.Color(0, 255, 0), 500); // Green
//  colorWipe(strip.Color(0, 0, 255), 500); // Blue
//determine if button held
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
    
  switch (stage) {
    case 0: 
    spiralChase(60);
    break;
    case 1: 
    circleChase(100);
    break;
    case 2:
    theaterChase(strip.Color(127, 127, 127), 60); // White
    break;
    case 3:
    theaterChase(strip.Color(127, 0, 0), 60); // Red
    break;
    case 4:
    theaterChase(strip.Color(0, 127, 0), 60); // Blue
    break;
    case 5:
    rainbow(20);
    break;
    case 6:
    rainbowCycle(30);
    break;
    case 7:
    theaterChaseRainbow(70);
    break;}
  
  if (autoCycle == true) {
    //Serial.print("auto cycling!");
    //Serial.print(stage);
    //Serial.print("-");
    stage = (stage + 1) % 8;
    //Serial.println(stage);
      }
  value += encoder->getValue();
  if (value != last && autoCycle == false) {
  last = value;
  //Serial.print("moving!");
  //Serial.println(value);
  stage = value % 8;
  }

//strip.setPixelColor(0, strip.Color(255,255,255));
//strip.show();
}



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
  nextColors();
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
stage = 1;
}

void circleChase(uint8_t wait) {
for(uint8_t g=0; g<20; g++){
  nextColors();
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
stage = 0;
}
void flip(uint8_t wait) {
  }

void drop(uint8_t wait) {
  }
void rise(uint8_t wait) {
  }
void randstar(uint8_t wait) {
  }    

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
      if (colorCounter > 10) {
        colorCounter = 0;} else{
      colorCounter++;
        }
      break;
    }
  }
uint32_t colorTable(uint8_t whichCase) {
 whichCase = whichCase % 6;
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
  return strip.Color(255,0,255);

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

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
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
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
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