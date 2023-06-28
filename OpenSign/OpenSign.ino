#include <FastLED.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_SleepyDog.h>

#define NUM_LEDS_LIGHTNING 8
#define NUM_LEDS_TEXT 24
#define LIGHTNING_BOLT_PIN 18
#define OPEN_TEXT_PIN 17

// MAJOR CONFIG
bool debug = false;
bool debugSetup = false;
bool demo = false;
bool demoColors = false;
const char* ssid = "";
const char* password = "";

// MINOR CONFIG
const int LOOP_TIME = 1000 * 60 * 2; // update sign every 2 minutes
const int RESET_TIME = 1000 * 60 * 5; // reboot device if nonresponsive for 5 minutes

// GET OPEN STATUS, DATE, AND TIME REMAINING
const char* serverName = "http://crashspacela.com/sign/";
const String statusSearchString = "Crash Space is OPEN";
const String timeSearchString = "The space will close in ";
const String dateSearchString = "<td>update</td><td> ";

// LED ARRAYS
CRGB lightning_leds[NUM_LEDS_LIGHTNING];
CRGB text_leds[NUM_LEDS_TEXT];

//uint32_t heapSize = ESP.getFreeHeap(); // for debugging

String getWebsiteText() {
  WiFiClient client;
  HTTPClient http;
  http.begin(client, serverName);
  int httpResponseCode = http.GET();
  
  String payload = "error"; // this will make the sign appear off if the site is unreachable
  
  if (httpResponseCode == 200) {
    payload = http.getString();
  }
  else if (debug) {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  return payload;
}

bool isSpaceOpen(String websiteText) {
  if (websiteText.indexOf(statusSearchString)>0){
      return true;
  }
  return false;
}
String colorKeys[] = {"02-14", "03-17", "07-04", "12-24", "12-25"};
int colorValues[][3][3] = {
  {{200,90,90},{200,90,90},{200,90,90}},
  {{0,150,0},{0,150,0},{0,150,0}},
  {{180,0,0},{180,180,180},{0,0,180}},
  {{180,0,0},{0,180,0},{180,180,180}},
  {{180,0,0},{0,180,0},{180,180,180}}
};
int defaultColor[3][3] = {
  {200, 200, 200}, 
  {200, 200, 200}, 
  {100, 20, 0}
};
int currentColor[3][3];

void setCurrentColor(String websiteText) {
  int dateIndex = websiteText.indexOf(dateSearchString) + dateSearchString.length() + 5; // ignore year portion (2023-)
  String dateValue = websiteText.substring(dateIndex,dateIndex+5);
  memcpy(currentColor, defaultColor, sizeof(defaultColor[0][0])*9);
  for (int i=0; i<5; i++) {
    if (colorKeys[i] == dateValue) {
      memcpy(currentColor, colorValues[i], sizeof(defaultColor[0][0])*9);
    }
  }
}

void setOpenTextLights() {
  for (int i=0; i<NUM_LEDS_TEXT; i++) {
    if (i >= 12) {
      text_leds[i] = CRGB(currentColor[0][0], currentColor[0][1], currentColor[0][2]);
    } else {
      text_leds[i] = CRGB(currentColor[1][0], currentColor[1][1], currentColor[1][2]);
    }
  }
  FastLED.show();
}

void setLightningBoltLights(String websiteText) {
  int minutesRemainingIndex = websiteText.indexOf(timeSearchString) + timeSearchString.length();
  int minutesRemainingValue = websiteText.substring(minutesRemainingIndex, minutesRemainingIndex+2).toInt();
  int numberLit = ceil(8.0*(double(minutesRemainingValue)/60.0));
  
  for (int i=0; i<8; i++) {
    if (i < numberLit) {
      lightning_leds[7-i] = CRGB(currentColor[2][0], currentColor[2][1], currentColor[2][2]);
    } else {
      lightning_leds[7-i] = CRGB::Black;
    }
  }
  FastLED.show();
}

void setup()
{
  Watchdog.enable(RESET_TIME);
  
  if (debug) {
    Serial.begin(115200);
    if (debugSetup) {
      while (!Serial) {
        delay(10);
      }
    }
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
      Serial.print((wl_status_t) WiFi.status());
      Serial.println("/ connecting...");
    }
  }
}

void demoLoop() {
  fill_solid(lightning_leds, NUM_LEDS_LIGHTNING, CRGB(150, 30, 0));
  fill_solid(text_leds, NUM_LEDS_TEXT, CRGB(180, 180, 180));
  FastLED.show();
  delay(1000);

  if (demoColors) {
    for (int i=0; i<5;i++) {
      memcpy(currentColor, colorValues[i], sizeof(defaultColor[0][0])*9);
      setOpenTextLights();
      setLightningBoltLights("Space will close in 60 minutes");
      delay(500);
    }
  }
  
  fill_solid(lightning_leds, NUM_LEDS_LIGHTNING, CRGB(0, 0, 0));
  fill_solid(text_leds, NUM_LEDS_TEXT, CRGB(0, 0, 0));
  FastLED.show();
  delay(1000);
}

void loop()
{
  if (demo) {
    demoLoop();
    return;
  }
  String websiteText = getWebsiteText();
  if (debug) {
    Serial.println(isSpaceOpen(websiteText) ? "Crash is open" : "Crash is closed");
  }
  if (isSpaceOpen(websiteText)) {  
    setCurrentColor(websiteText);
    setOpenTextLights();
    setLightningBoltLights(websiteText);
  } else {
    fill_solid(lightning_leds, NUM_LEDS_LIGHTNING, CRGB(0, 0, 0));
    fill_solid(text_leds, NUM_LEDS_TEXT, CRGB(0, 0, 0));
    FastLED.show();
  }
  Watchdog.reset();
  delay(LOOP_TIME);
}
