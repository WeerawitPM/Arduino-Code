#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClient.h>

#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor

#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 4     // Config DHT on GPIO 6

DHT dht(dht_dpin, DHTTYPE);
WiFiMulti wifiMulti;

void setup(void)
{
  Serial.begin(9600);
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
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(t);
  Serial.println("C  ");
  /* Passing Data To Your Running DataBase */
  if ((wifiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    /* Make sure to replace the http by your API & replace the localhost by your IP address */
    if (http.begin(client, "http://localhost:5000/add?temp="+String(t)+"&hum="+String(h))) { // HTTP
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
