// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
  Serial.begin(9600);

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
    //หากต้องการตั้งค่าเวลาใหม่ให้ตรงกับเครื่องให้เปิดคอมเม้น rtc.adjust
//    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
//     rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  //หากต้องการตั้งค่าเวลาใหม่ให้ตรงกับเครื่องให้เปิดคอมเม้น rtc.adjust
//   rtc.adjust(DateTime(2022, 9, 12, 17, 18, 0));
}

void loop () {
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
    delay(1000);
}
