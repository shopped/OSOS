#include <FastLED.h>
#define NUM_LEDS 8
#define DATA_PIN 18
#define THRESHOLD 23000


int currentTouchValue = 0;
bool debug = true;
bool demo = true;
int timer = -1;

CRGB leds[NUM_LEDS];

void setup()
{
  if (debug) {
    Serial.begin(115200);
  }
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop()
{
  currentTouchValue = touchRead(T9);
   if (debug) {
    Serial.println(currentTouchValue);
    Serial.println(timer);
  }
  if (demo) {
    if (currentTouchValue > THRESHOLD) {
      timer = 9;
      fill_solid(leds, NUM_LEDS, CRGB(100, 20, 0));
      FastLED.show();
    } else if (--timer >= 0) {
      for (int i=0; i<8; i++) {
        if (i < timer) {
          leds[7-i] = CRGB(100, 100, 100);
        } else {
          leds[7-i] = CRGB::Black;
        }
      }
      FastLED.show();
    }
    delay(1000);
    return;
  }
  if (currentTouchValue > THRESHOLD) {
    timer = 60 * 60 + 1;
    fill_solid(leds, NUM_LEDS, CRGB(100, 20, 0));
    FastLED.show();
  } else if (timer > 0) {
    if (--timer % 450 == 0) {
      for (int i=0; i<8; i++) {
        if (i < timer/450) {
          leds[7-i] = CRGB(100, 100, 100);
        } else {
          leds[7-i] = CRGB::Black;
        }
      }
      FastLED.show();
    }
  }
  delay(1000);
}
