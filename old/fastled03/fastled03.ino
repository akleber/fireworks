/* A 1D fireworks routine
 * 
 * Found at: http://www.anirama.com/1000leds/1d-fireworks/
 *  
 *  Kind of fixed by: /u/AaronMickDee
 *  
 *  Last fixes by: Andrew Tuline
 *  
 *  In this thread: https://www.reddit.com/r/FastLED/comments/c9ibnb/does_anybody_have_a_fireworks_effect_fastled/
 *  
 *  I didn't write this, I just hacked a couple of fixes for it.
 *  
 *  Version Below working on 240 ws2812 LEds running on ESP32 #ImSoDopeAHedron
 */

#define FASTLED_ESP8266_NODEMCU_PIN_ORDER

#include <FastLED.h>
#define NUM_LEDS 66
#define DATA_PIN 4       // Used on my ESP8266
CRGB leds[NUM_LEDS]; // sets up block of memory
 
#define NUM_SPARKS 5 // max number (could be NUM_LEDS / 2);
float sparkPos[NUM_SPARKS];
float sparkVel[NUM_SPARKS];
float sparkCol[NUM_SPARKS];
float flarePos;
float flarePos2;
 
float gravity = -.004; // m/s/s
 
void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
}
 
/*
 * Main Loop
 */
void loop() {  
  // send up flare
  flare();
  Serial.println("END flare loop");
 
 
  // explode
  explodeLoop();
  Serial.println("END EXPLODE loop");
 
  // wait before sending up another
  delay(random16(1000, 4000));
}
 
 
/*
 * Send up a flare
 *
 */
void flare() {
 Serial.println("Begin flareloop");
 
  flarePos = 0;
  float flareVel = float(random16(80, 120)) / 100; // trial and error to get reasonable range  70-56  80-80 130-211
  float brightness = 1;
 
  // initialize launch sparks
  for (int i = 0; i < 5; i++) {
    sparkPos[i] = 0; 
    sparkVel[i] = (float(random8()) / 255) * (flareVel / 5); // random around 20% of flare velocity
  sparkCol[i] = sparkVel[i] * 1000; 
  sparkCol[i] = constrain(sparkCol[i], 0, 255);
  }
  // launch
// FastLED.clear();
  while (flareVel >= -0.2) {
     Serial.println(flarePos);
     delay(0);
    // sparks
    for (int i = 0; i < 20; i++) {
      sparkPos[i] += sparkVel[i];
      sparkPos[i] = constrain(sparkPos[i], 0, 240);
      sparkVel[i] += gravity;
      sparkCol[i] += -.8;
      sparkCol[i] = constrain(sparkCol[i], 0, 255);
      leds[int(sparkPos[i])] = HeatColor(sparkCol[i]);
      leds[int(sparkPos[i])] %= 50; // reduce brightness to 50/255
    
  }
    // flare
    leds[int(flarePos)] = CHSV(0, 0, int(brightness * 255));
    FastLED.show();
    delay(5);
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
  Serial.println("EXPLODE Loop Start");

  int nSparks = flarePos / 2; // works out to look about right
 
  // initialize sparks
  for (int i = 0; i < nSparks; i++) {
    sparkPos[i] = flarePos; 
    sparkVel[i] = (float(random16(0, 20000)) / 10000.0) - 1.0; // from -1 to 1
    //sparkCol[i] = abs(sparkVel[i]) * 5000; // set colors before scaling velocity to keep them bright
    //Serial.println(sparkCol[i]);
    //sparkCol[i] = constrain(sparkCol[i], 0, 255);
     sparkCol[i] = random8(200, 255);
    sparkVel[i] *= flarePos / NUM_LEDS-1; // proportional to height
    
  }
  sparkCol[0] = 255; // this will be our known spark
  float dying_gravity = gravity;
  float c1 = 120;
  float c2 = 50;
  while(sparkCol[0] > c2/128) { // as long as our known spark is lit, work with all the sparks
    delay(0);
    FastLED.clear();
    for (int i = 0; i < nSparks; i++) {
      sparkPos[i] += sparkVel[i];
      sparkPos[i] = constrain(sparkPos[i], 0, NUM_LEDS-1);
      sparkVel[i] += dying_gravity;
      sparkCol[i] *= .99;
      sparkCol[i] = constrain(sparkCol[i], 0, 255); // red cross dissolve
      if(sparkCol[i] > c1) { // fade white to yellow
        leds[int(sparkPos[i])] = CRGB(255, 255, (255 * (sparkCol[i] - c1)) / (255 - c1));
      }
      else if (sparkCol[i] < c2) { // fade from red to black
        leds[int(sparkPos[i])] = CRGB((255 * sparkCol[i]) / c2, 0, 0);
      }
      else { // fade from yellow to red
        leds[int(sparkPos[i])] = CRGB(255, (255 * (sparkCol[i] - c2)) / (c1 - c2), 0);
      }
    }
    dying_gravity *= .995; // as sparks burn out they fall slower
    FastLED.show();
  }
  FastLED.clear();
  FastLED.show();
}