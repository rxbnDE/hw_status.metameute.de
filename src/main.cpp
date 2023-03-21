#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#ifdef ESP32
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
WiFiMulti wifiMulti;
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
ESP8266WiFiMulti wifiMulti;
#endif

#define LED_PIN 4
#define SWITCH_PIN 5
#define OPEN 1
#define CLOSED 0

Adafruit_NeoPixel led = Adafruit_NeoPixel(1, LED_PIN, NEO_RGB);

bool doorState = CLOSED;
bool lastSend = doorState;
bool interrupts = true;
bool freshWIFI = true;
int counter = 0;
volatile boolean interrupt_flag = false;
uint32_t color = led.Color(0,255,255);

String ssid = "";
String pswd = "";
const char* apiHOST = "https://status.metameute.de";
const char* apiPATH = "/setState?token=";
const char* apiTOKEN = "SECRET";

IRAM_ATTR void changeState() {
	if (interrupt_flag) {
		return;
	}

	interrupt_flag = true;

	
	doorState = digitalRead(SWITCH_PIN) == HIGH;


	interrupt_flag = false;
}

void flipDoorState() {
	HTTPClient http;
	WiFiClientSecure client;
	client.setInsecure();
	String path = apiHOST;
	path.concat(apiPATH);
	path.concat(apiTOKEN);
	path.concat("&value=");
	path.concat((doorState) ? "true": "false");
	
	if(http.begin(client, path)) {
		int httpCode = http.GET();
		counter++;
		Serial.println(counter);
		Serial.printf("httpCode: %d\n", httpCode);
		Serial.println(path);
		if(httpCode > 0) {
			if(httpCode == 200) {
				String payload = http.getString();
				Serial.println(payload);
			}
		} else {
			Serial.println("HTTP-Connection lost!");
		}
		http.end();
	} else {
		Serial.println("HTTP-Connection cannot be established.");
	}	
}

void setup()
{
	Serial.begin(9600);
	delay(10);
	Serial.println('\n');

	pinMode(SWITCH_PIN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), changeState, CHANGE);

	doorState = digitalRead(SWITCH_PIN) == HIGH;

	// setup led
	led.begin();
	led.setBrightness(10);
	led.setPixelColor(0, color);
	led.show();

	// CONNECT
	WiFi.mode(WIFI_STA);

	wifiMulti.addAP("luebeck.freifunk.net");
	wifiMulti.addAP("WLAN-XIE8N1", "YUNGYUNG789!");
}

void loop()
{
	// Maintain WiFi connection
	if (wifiMulti.run(5000) == WL_CONNECTED) {
		if(freshWIFI) {
				Serial.println(".");
				Serial.println("Connection established");
				Serial.println(WiFi.localIP());
				freshWIFI = false;
		}

		if (doorState) {
			color = led.Color(0, 255, 0);
		} else {
			color = led.Color(255, 0, 0);
		}
		led.setBrightness(50);
		led.setPixelColor(0, color);
		led.show();
		if(doorState != lastSend) {
			lastSend = doorState;
			flipDoorState();
			delay(300);
		}
		delay(50);
	} else {
		Serial.println("Not connected");
		freshWIFI = true;
		color = led.Color(0, 255, 255);
		led.setBrightness(20);
		led.setPixelColor(0, color);
		led.show();
	}
}
