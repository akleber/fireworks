
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER

#include <FastLED.h>

#define NUM_LEDS 66

#define LED_PIN1 4
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB  
// #define LED_PIN1 10
// #define LED_TYPE NEOPIXEL
// #define COLOR_ORDER GRB  

CRGB leds[NUM_LEDS];

#define NUM_SPARKS 25 // max number (could be NUM_LEDS / 2);
float sparkPos[NUM_SPARKS];
float sparkVel[NUM_SPARKS];
float sparkCol[NUM_SPARKS];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN1, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

  Serial.begin(115200); // Any baud rate should work
  Serial.println("Setup\n");
}

void loop() {
  flare();
  explode02();

  FastLED.clear();
  FastLED.show();
}

void flare() {

  float gravity = -.0049; // m/s/s
  float flarePos = 0;
  float flareVel = 0.70;
  float brightness = 1;

  // launch
  FastLED.clear();
  while (flareVel >= 0.0) {
    leds[int(flarePos)] = CHSV(0, 0, int(brightness * 255));
    FastLED.show();
    FastLED.clear();
    flarePos += flareVel;
    flareVel += gravity;
    brightness *= .997;
    //Serial.print(int(flarePos)); Serial.print("\n");

    delay(10);
  }

  //Serial.print("flare end\n");
}

void explode02() {
  //int nSparks = flarePos / 2; // works out to look about right
  float flarePos = 51.0;
  int nSparks = 7;

  float sparkTime = 255.0;

  // initialize sparks
  for (int i = 0; i < nSparks; i++) {
    sparkPos[i] = flarePos;
    sparkVel[i] = (float(random16(0, 10000)) / 40000.0);// 0 to 1
    sparkCol[i] = constrain(sparkVel[i] * 500, 0, 255);
  }

  while (sparkTime > 1.0) { // as long as our known spark is lit, work with all the sparks
    FastLED.clear();
    sparkTime *= .95;
    //Serial.print(sparkTime); Serial.print("\n");

    for (int i = 0; i < nSparks; i++) {
      sparkPos[i] += sparkVel[i];
      sparkPos[i] = constrain(sparkPos[i], 51, NUM_LEDS);
      sparkCol[i] *= .97;
      sparkCol[i] = constrain(sparkCol[i], 0, 255); // red cross dissolve
      leds[int(sparkPos[i])] = CRGB(255, 255, 255);
    }
    FastLED.show();
    FastLED.delay(10);
  }
}

void explode01() {
  //int nSparks = flarePos / 2; // works out to look about right
  float flarePos = 51.0;
  int nSparks = 7;

  // initialize sparks
  for (int i = 0; i < nSparks; i++) {
    sparkPos[i] = flarePos;
    sparkVel[i] = (float(random16(0, 10000)) / 40000.0);// 0 to 1
    sparkCol[i] = constrain(sparkVel[i] * 500, 0, 255);
  }

  sparkCol[0] = 255; // this will be our known spark
  float c1 = 120;
  float c2 = 50;

  while (sparkCol[0] > c2 / 128) { // as long as our known spark is lit, work with all the sparks
    FastLED.clear();
    for (int i = 0; i < nSparks; i++) {
      sparkPos[i] += sparkVel[i];
      sparkPos[i] = constrain(sparkPos[i], 51, NUM_LEDS);
      sparkCol[i] *= .99;
      sparkCol[i] = constrain(sparkCol[i], 0, 255); // red cross dissolve
      if (sparkCol[i] > c1) { // fade white to yellow
        leds[int(sparkPos[i])] = CRGB(255, 255, (255 * (sparkCol[i] - c1)) / (255 - c1));
      }
      else if (sparkCol[i] < c2) { // fade from red to black
        leds[int(sparkPos[i])] = CRGB((255 * sparkCol[i]) / c2, 0, 0);
      }
      else { // fade from yellow to red
        leds[int(sparkPos[i])] = CRGB(255, (255 * (sparkCol[i] - c2)) / (c1 - c2), 0);
      }
    }
    FastLED.show();

    FastLED.delay(10);
  }
}
