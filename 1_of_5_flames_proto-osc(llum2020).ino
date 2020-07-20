//#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>                // for sending OSC messages
#include <OSCBundle.h>                 // for receiving OSC messages
#include <WS2812FX.h>
#include "FastLED.h"
#include <Ticker.h>

#define NUM_LEDS 60
CRGB leds[NUM_LEDS];
#define PIN 3

#define TIMER_MS 5000
#define FRAMES_PER_SECOND 60

CRGBPalette16 gPal;
uint8_t hue = 0;

bool gReverseDirection = false;

char ssid[] = "SSID_HEADLESS_RPI";                 // your network SSID (name)
char pass[] = "PASSWORD_HEADLESS_RPI";              // your network password

Ticker secondTick; //watchdog

volatile int watchdogCount = 0; //watchdog

const int boardLed = LED_BUILTIN;      // Builtin LED

WiFiUDP Udp;                           // A UDP instance to let us send and receive packets over UDP
const IPAddress destIp(192,168,42,10);   // remote IP of the target device
const unsigned int destPort = 9000;    // remote port of the target device where the NodeMCU sends OSC to
const unsigned int localPort = 8000;   // local port to listen for UDP packets at the NodeMCU (another device must send OSC messages to this port)
// const unsigned int localPort = 8001;   // local port to listen for UDP packets at the NodeMCU (another device must send OSC messages to this port)
// const unsigned int localPort = 8002;   // local port to listen for UDP packets at the NodeMCU (another device must send OSC messages to this port)
// const unsigned int localPort = 8003;   // local port to listen for UDP packets at the NodeMCU (another device must send OSC messages to this port)
// const unsigned int localPort = 8004;   // local port to listen for UDP packets at the NodeMCU (another device must send OSC messages to this port)

OSCErrorCode error;
unsigned int ledState = 1;             // LOW means led is *on*
unsigned int mode = 0;             // LOW means led is *on*

void ISRwatchdog(){
  watchdogCount++;
  if(watchdogCount == 20){
    Serial.println();
    Serial.println("watchdog bites!!!!");
    ESP.reset();
  }
}

void showStrip() {
 // #ifdef ADAFRUIT_NEOPIXEL_H
 //   // NeoPixel
 //   strip.show();
 // #endif
 // #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 // #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 // #ifdef ADAFRUIT_NEOPIXEL_H
 //   // NeoPixel
 //   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 // #endif
 // #ifndef ADAFRUIT_NEOPIXEL_H
 //   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 // #endif
}


void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void led1toggle(OSCMessage &msg, int addrOffset){
  Serial.println(msg.getFloat(0));
  int color = msg.getFloat(0);
//    uint32_t c = (uint32_t)strtoul(cmd + 2, NULL, 16);
    // ws2812fx.setColor(color);
  }

    int COOLING = 55;
    int SPARKING  = 120;

        void Flame1(OSCMessage &msg, int addrOffset) {

          int var = msg.getFloat(0);
          Serial.println(var);
          COOLING = var;
        }
        // SPARKING: What chance (out of 255) is there that a new spark will be lit?
        // Higher chance = more roaring fire.  Lower chance = more flickery fire.
        // Default 120, suggested range 50-200.
        void Flame2(OSCMessage &msg, int addrOffset) {

          int var = msg.getFloat(0);
          Serial.println(var);
          SPARKING = var;
        }

        const TProgmemRGBPalette16 YellowColors_p FL_PROGMEM =
        {
            0x000000,
            0xfffef3, 0xfffbbd, 0xfff54d, 0xeee32f, 0xe2d25d
        };

        void colorp(OSCMessage &msg, int addrOffset) {
          int var = msg.getFloat(0);
          Serial.println(var);
          switch (var) {
            case 0: gPal = HeatColors_p; break;
            case 1: gPal = CRGBPalette16( CRGB::Black, CRGB::DarkGreen, CRGB::DarkCyan, CRGB::ForestGreen); break;
            case 2: gPal = CRGBPalette16( CRGB::Black, CRGB::SeaGreen, CRGB::ForestGreen, CRGB::ForestGreen); break;
            case 3: gPal = CRGBPalette16( CRGB::Black, CRGB::Teal, CRGB::SeaGreen, CRGB::Black); break;
            case 4: gPal = CRGBPalette16( CRGB::Black, CRGB::DarkBlue, CRGB::Purple, CRGB::DarkBlue); break;
            case 5: gPal = CRGBPalette16( CRGB::Black, CRGB::DarkBlue, CRGB::Blue, CRGB::SkyBlue); break;
            case 6: gPal = CRGBPalette16( CRGB::Black, CRGB::Orange, CRGB::YellowGreen, CRGB::Yellow); break;
          }
        }

void receiveOSC(){
    OSCMessage msgIN;
    int size;
    if((size = Udp.parsePacket())>0){
        while(size--)
            msgIN.fill(Udp.read());
        if(!msgIN.hasError()){
            // msgIN.route("/2/1", led11);
            // msgIN.route("/2/2", led12);
            msgIN.route("/flame1", Flame1);
            msgIN.route("/flame2", Flame2);
            msgIN.route("/colorp", colorp);
        }
    }
}


void toggleOnOff(OSCMessage &msg, int addrOffset){
  ledState = (boolean) msg.getFloat(0);
  Serial.println(msg.getFloat(0));

  digitalWrite(boardLed, (ledState + 1) % 2);   // Onboard LED works the wrong direction (1 = 0 bzw. 0 = 1)

  if (ledState) {
    Serial.println("LED on");
  }
  else {
    Serial.println("LED off");
  }
  ledState = !ledState;                         // toggle the state from HIGH to LOW to HIGH to LOW ...
}

//animations -----------------------------


void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(100); } }
void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}

//------------------------------------------------
  void setup() {
      Serial.begin(115200);

      // Specify a static IP address for NodeMCU - only needeed for receiving messages)
      // If you erase this line, your ESP8266 will get a dynamic IP address
      WiFi.config(IPAddress(192,168,42,150),IPAddress(192,168,42,1), IPAddress(255,255,255,0));
      // WiFi.config(IPAddress(192,168,42,151),IPAddress(192,168,42,1), IPAddress(255,255,255,0));
      // WiFi.config(IPAddress(192,168,42,152),IPAddress(192,168,42,1), IPAddress(255,255,255,0));
      // WiFi.config(IPAddress(192,168,42,153),IPAddress(192,168,42,1), IPAddress(255,255,255,0));
      // WiFi.config(IPAddress(192,168,42,154),IPAddress(192,168,42,1), IPAddress(255,255,255,0));

      // Connect to WiFi network
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(ssid);
      WiFi.begin(ssid, pass);

      while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
      }

      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());

      Serial.println("Starting UDP");
      Udp.begin(localPort);
      Serial.print("Local port: ");
      Serial.println(Udp.localPort());

      pinMode(boardLed, OUTPUT);

      FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

      gPal = HeatColors_p;

      secondTick.attach(1,ISRwatchdog);


  }



  void loop() {
    watchdogCount = 0;
    Serial.printf("Watchdog Counter = %d\n", watchdogCount);
    receiveOSC();
    Fire2012();
    FastLED.delay(1000 / FRAMES_PER_SECOND);
  }
