//https://pastebin.com/d98TbWDP

#define FASTLED_ESP8266_NODEMCU_PIN_ORDER

#include <FastLED.h>
#include "ColorsPalets.h"
#define NUM_LEDS 66
#define LED_PIN1     4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB  
CRGB leds[NUM_LEDS]; // sets up block of memory

#define NUM_SPARKS 30 // max number (could be NUM_LEDS / 2);
float sparkPos[NUM_SPARKS];
float sparkVel[NUM_SPARKS];
float sparkCol[NUM_SPARKS];
float flarePos;
#define BRIGHTNESS  100
int currentBrightness = 0;
float gravity = -.003 ; // m/s/s

void setup() {
  Serial.begin(115200);
  currentBrightness = BRIGHTNESS;
  FastLED.addLeds<LED_TYPE, LED_PIN1, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(currentBrightness);
}

/*
 * Main Loop
 */
void loop() {  
  
  // send up flare
  flare();
  
  // explode
  explodeLoop();

  // wait before sending up another
  //delay(random16(1000, 4000));
  delay(50);
  checkBrightnessChange();
}

void checkBrightnessChange() {
  int newBrightness = getPotentiometerVal(1);
  if(newBrightness != currentBrightness) {
    currentBrightness = newBrightness;
     FastLED.setBrightness(currentBrightness);
  }
}

int getPotentiometerVal(int analogPing) {
   int val = analogRead(analogPing);
   return map(val, 0, 1023, 0, 255);  
}
/*
 * Send up a flare
 * 
 */
void flare() {
  paletBlending();
  
  flarePos = 0;
  float flareVel = float(random16(50, 90)) / 100; // trial and error to get reasonable range
  float brightness = 1;

  // initialize launch sparks
  for (int i = 0; i < 5; i++) { 
    sparkPos[i] = 0; sparkVel[i] = (float(random8()) / 255) * (flareVel / 5); // random around 20% of flare velocity 
  sparkCol[i] = sparkVel[i] * 1000; sparkCol[i] = constrain(sparkCol[i], 0, 255);
  }
  // launch
// FastLED.clear(); 
  while (flareVel >= -.2) {
    // sparks
    for (int i = 0; i < 5; i++) {
      sparkPos[i] += sparkVel[i];
      sparkPos[i] = constrain(sparkPos[i], 0, 120);
      sparkVel[i] += gravity;
      sparkCol[i] += -.8;
      sparkCol[i] = constrain(sparkCol[i], 0, 255);
      leds[int(sparkPos[i])] = HeatColor(sparkCol[i]);
      leds[int(sparkPos[i])] %= 50; // reduce brightness to 50/255
    }
    
    // flare
    leds[int(flarePos)] = CHSV(0, 0, int(brightness * 255));
    FastLED.show();
    FastLED.clear();
    flarePos += flareVel;
    flareVel += gravity;
    brightness *= .985;
  }
}

/*
 * Explode!
 * 
 * Explosion happens where the flare ended.
 * Size is proportional to the height.
 */
void explodeLoop() {
  int nSparks = flarePos / 2; // works out to look about right
  
  // initialize sparks
  for (int i = 0; i < nSparks; i++) { 
    sparkPos[i] = flarePos; sparkVel[i] = (float(random16(0, 20000)) / 10000.0) - 1.0; // from -1 to 1 
    sparkCol[i] = abs(sparkVel[i]) * 500; // set colors before scaling velocity to keep them bright 
    sparkCol[i] = constrain(sparkCol[i], 0, 255); 
    sparkVel[i] *= flarePos / NUM_LEDS; // proportional to height 
  } 
  sparkCol[0] = 255; // this will be our known spark 
  float dying_gravity = gravity; 
  float c1 = 120; 
  float c2 = 50; 
  CRGB color1 = palet1(255,255);
  CRGB color2 =  palet2(255,255);
  while(sparkCol[0] > c2/128) { // as long as our known spark is lit, work with all the sparks
    FastLED.clear();
    for (int i = 0; i < nSparks; i++) { 
      sparkPos[i] += sparkVel[i]; 
      sparkPos[i] = constrain(sparkPos[i], 0, NUM_LEDS); 
      sparkVel[i] += dying_gravity; 
      sparkCol[i] *= .99; 
      sparkCol[i] = constrain(sparkCol[i], 0, 255); // red cross dissolve 
      if(sparkCol[i] > c1) { // fade white to yellow
        leds[int(sparkPos[i])] = CRGB((255 * (sparkCol[i] - color1.r)) / (255 - color1.r), (255 * (sparkCol[i] - color1.g)) / (255 - color1.g), (255 * (sparkCol[i] - color1.b)) / (255 - color1.b));
      }
      else if (sparkCol[i] < c2) { // fade from red to black
        leds[int(sparkPos[i])] = CRGB((color2.r * sparkCol[i]) / c2, (color2.g * sparkCol[i]) / c2, (color2.b * sparkCol[i]) / c2);
      }
      else { // fade from yellow to red
        leds[int(sparkPos[i])] = CRGB((color1.r * (sparkCol[i] - color2.r)) / (color1.r - color2.r), (color1.g * (sparkCol[i] - color2.g)) / (color1.g - color2.g), (color1.b * (sparkCol[i] - color2.b)) / (color1.b - color2.b));
      }
    }
    dying_gravity *= .995; // as sparks burn out they fall slower
    FastLED.show();
    paletBlending();
  }
  
  FastLED.clear();
  FastLED.show();
}