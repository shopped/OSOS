#include <FastLED.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define NUM_LEDS_LIGHTNING 8
#define NUM_LEDS_TEXT 24
#define LIGHTNING_BOLT_PIN 18
#define OPEN_TEXT_PIN 17
uint32_t heapSize = ESP.getFreeHeap();

const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* serverName = "URL";
String seachString = "Crash Space is OPEN";

bool CheckSign(){ //returns true if crashspace is open
  bool openStatus= false;
  WiFiClient client;
  HTTPClient http;
    
  http.begin(client, serverName);
  
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    payload = http.getString();           //payload should contain the source of the html of http://crashspacela.com/sign/
    if (payload.indexOf(seachString)>0){  // when the button has been pressed the page should contain "Crash Space is OPEN"
      openStatus = true;
    }
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  return openStatus;
}

bool debug = true;
bool demo = false;
int timer = -1;
int colors[][3][3] = {
  {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}}
};
String colorKeys[] = {"02-14", "03-17", "07-04", "12-24", "12-25"};
int colorValues[][3][3] = {
  {{200,90,90},{200,90,90},{200,90,90}},
  {{0,150,0},{0,150,0},{0,150,0}},
  {{180,0,0},{180,180,180},{0,0,180}},
  {{180,0,0},{0,180,0},{180,180,180}},
  {{180,0,0},{0,180,0},{180,180,180}}
};

CRGB lightning_leds[NUM_LEDS_LIGHTNING];
CRGB text_leds[NUM_LEDS_TEXT];

int counter = 0;
void setSpecialColor(String dateString) {
  if (demo) {
    for (int i=0; i<NUM_LEDS_TEXT; i++) {
      if (i >= 12) {
        text_leds[i] = CRGB(colorValues[counter][0][0], colorValues[counter][0][1], colorValues[counter][0][2]);
      } else {
        text_leds[i] = CRGB(colorValues[counter][1][0], colorValues[counter][1][1], colorValues[counter][1][2]);
      }
    }
    fill_solid(lightning_leds, NUM_LEDS_LIGHTNING, CRGB(colorValues[counter][2][0], colorValues[counter][2][1], colorValues[counter][2][2]));
  }
  if (++counter > 4) {
    counter = 0;
  }
  FastLED.show();
}

void setup()
{
  if (debug) {
    Serial.begin(115200);
  }
  FastLED.addLeds<NEOPIXEL, LIGHTNING_BOLT_PIN>(lightning_leds, NUM_LEDS_LIGHTNING);
  FastLED.addLeds<NEOPIXEL, OPEN_TEXT_PIN>(text_leds, NUM_LEDS_TEXT);
  fill_solid(lightning_leds, NUM_LEDS_LIGHTNING, CRGB(0, 0, 0));
  fill_solid(text_leds, NUM_LEDS_TEXT, CRGB(0, 0, 0));
  FastLED.show();

  WiFi.begin(ssid, password);
  if (debug) {
    Serial.println("Connecting");
  }
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (debug) {
      Serial.print(".");
    }
  }
  heapSize = ESP.getFreeHeap();
}

void loop()
{
  if (demo) {
    if (timer == -1) {
      timer = 1;
//      setSpecialColor("testing");
      fill_solid(lightning_leds, NUM_LEDS_LIGHTNING, CRGB(150, 30, 0));
      fill_solid(text_leds, NUM_LEDS_TEXT, CRGB(180, 180, 180));
    } else {
      timer = -1;
      fill_solid(lightning_leds, NUM_LEDS_LIGHTNING, CRGB(0, 0, 0));
      fill_solid(text_leds, NUM_LEDS_TEXT, CRGB(0, 0, 0));
    }
    FastLED.show();
    delay(1000);
    return;
  } else {
    if (CheckSign()) {
      fill_solid(lightning_leds, NUM_LEDS_LIGHTNING, CRGB(150, 30, 0));
      fill_solid(text_leds, NUM_LEDS_TEXT, CRGB(180, 180, 180));
    } else {
      fill_solid(lightning_leds, NUM_LEDS_LIGHTNING, CRGB(0, 0, 0));
      fill_solid(text_leds, NUM_LEDS_TEXT, CRGB(0, 0, 0));
    }
    if (debug) {
      Serial.println(CheckSign());
    }
    FastLED.show();
    delay(1000 * 60 * 2);
    // wait two minutes for now. In the future lets make this longer
  }
}
