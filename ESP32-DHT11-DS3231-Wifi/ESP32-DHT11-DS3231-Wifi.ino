#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClient.h>

#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor

#define DHTTYPE DHT11
#define dht_dpin 4
DHT dht(dht_dpin, DHTTYPE);

#include "RTClib.h"
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String board_id = "001";

WiFiMulti wifiMulti;

void setup(void)
{
  Serial.begin(115200);
  pinMode(2, OUTPUT);

  Serial.println();
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("MiwaMew", "1122334455");  // Replace SSID & PASSWORD by Your WiFi Network Name & Pass
  Serial.println("Connecting Wifi...");
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  dht.begin();
}
void loop() {
  /* Reading Temp & Humadity Values of DHT */
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(t);
  Serial.print("C  ");
  Serial.print("temperatureF = ");
  Serial.print(f);
  Serial.println("F  ");

  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  int y = now.year();
  Serial.print('/');
  Serial.print(now.month(), DEC);
  int m = now.month();
  Serial.print('/');
  Serial.print(now.day(), DEC);
  int d2 = now.day();
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  String d1 = daysOfTheWeek[now.dayOfTheWeek()];
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  int th = now.hour();
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  int tm = now.minute();
  Serial.print(':');
  Serial.print(now.second(), DEC);
  int ts = now.second();

  /* Passing Data To Your Running DataBase */
  if ((wifiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    /* Make sure to replace the http by your API & replace the localhost by your IP address */

    if (http.begin(client, "http://backend-embedded.herokuapp.com/add?bi=" + board_id + "&temp=" + String(t) + "&tempf=" + String(f) + "&hum=" + String(h) + "&aa=" + d1 + "&bb=" + String(d2) + "&cc=" + String(m) + "&dd=" + String(y) + "&ee=" + String(th) + "&ff=" + String(tm) + "&gg=" + String(ts))) { // HTTP
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
          digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
          delay(500);                       // wait for a second
          digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
          delay(500);
        }
      }
      else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    }
    else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
  else {
    Serial.println("WiFi not connected!");
    delay(1000);
  }

  delay(1000);
}
