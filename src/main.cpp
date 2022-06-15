#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
// #include <DNSServer.h>
#ifdef ESP32
// #include <WiFi.h>
// #include <AsyncTCP.h>
#elif defined(ESP8266)
// #include <ESP8266WiFi.h>
// #include <ESPAsyncTCP.h>
#endif
// #include "ESPAsyncWebServer.h"

#define LED_PIN 4
#define SWITCH_PIN 5
#define OPEN true
#define CLOSED false

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, LED_PIN, NEO_RGB);

// DNSServer dnsServer;
// AsyncWebServer server(80);

bool doorState = CLOSED;
bool interrupts = true;
volatile boolean interrupt_flag = false;

uint32_t Wheel(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

ICACHE_RAM_ATTR void changeState() {
    if (interrupt_flag) {
        return;
    }

    interrupt_flag = true;

    if (digitalRead(SWITCH_PIN) == HIGH) {
        strip.setPixelColor(0, strip.Color(0, 255, 0));
        for (byte i = 255; i > 30; i--) {
            strip.setBrightness(i);
            strip.show();
            // delay(1);
        }
    } else {
        strip.setPixelColor(0, strip.Color(255, 0, 0));
        for (byte i = 255; i > 30; i--) {
            strip.setBrightness(i);
            strip.show();
            delay(1);
        }
    }
    
    interrupt_flag = false;
}


// class CaptiveRequestHandler : public AsyncWebHandler
// {
// public:
//     CaptiveRequestHandler() {}
//     virtual ~CaptiveRequestHandler() {}

//     bool canHandle(AsyncWebServerRequest *request)
//     {
//         // request->addInterestingHeader("ANY");
//         return true;
//     }

//     void handleRequest(AsyncWebServerRequest *request)
//     {
//         AsyncResponseStream *response = request->beginResponseStream("text/html");
//         response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
//         response->print("<p>This is out captive portal front page.</p>");
//         response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
//         response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
//         response->print("</body></html>");
//         request->send(response);
//     }
// };

void setup()
{
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), changeState, CHANGE);

    // setup led
    strip.begin();
    strip.setBrightness(10);
    strip.show(); // Initialize all pixels to 'off'

    // // your other setup stuff...
    // WiFi.softAP("esp-captive");
    // dnsServer.start(53, "*", WiFi.softAPIP());
    // server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); // only when requested from AP
    // // more handlers...
    // server.begin();
}

void loop()
{
    // dnsServer.processNextRequest();
    delay(500);
    // strip.setBrightness(100);
    // for (byte i = 0; i < 255; i++) {
    //     strip.setPixelColor(0, Wheel(i));
    //     strip.show();
    //     delay(10);
    // }
}
