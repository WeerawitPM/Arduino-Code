#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClient.h>

#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#include "RTClib.h"

#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 4     // Config DHT on GPIO 6

DHT dht(dht_dpin, DHTTYPE);
RTC_DS3231 rtc;

WiFiMulti wifiMulti;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String board_id = "001";

void setup(void)
{
  Serial.begin(9600);
#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  /*  Switch of sensors reading indicator Establishment */
  pinMode(5, OUTPUT);
  pinMode(2, OUTPUT);
  /* WiFi Connection Establishment */
  Serial.println();
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("MiwaMew", "1122334455");  // Replace SSID & PASSWORD by Your WiFi Network Name & Pass
  /* DHT Establishment */
  dht.begin();
  Serial.println("Humidity and temperature \n\n");
  Serial.println("===============================");
  delay(1000);

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

    if (http.begin(client, "http://backend-embedded.herokuapp.com/add?bi="+board_id+"&temp="+String(t)+"&tempf="+String(f)+"&hum="+String(h)+"&aa="+d1+"&bb="+String(d2)+"&cc="+String(m)+"&dd="+String(y)+"&ee="+String(th)+"&ff="+String(tm)+"&gg="+String(ts))) { // HTTP
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
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
  delay(1000);
}
