/*TODO

Add timezone 
Add resetSettings jumper code
Add display code


Pins HW I2C
D1 SCL
D2 SDA
*/

#include "Arduino.h"
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Timezone.h>
#include <U8g2lib.h>
#include <NTPClient.h>

// #define NTPDEBUG

#include "objects.h"

// Globals
unsigned long previousMillis = 0;
unsigned long previousNTPMillis = 0;
// Global Timer vars
const long interval = 1 * 1000;
const long ntpInterval = 30 * 1000; // interval for NTP checks

const char *monthName[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

// Late includes - leave here.  needs objects instantiated first
#include "displayTime.h"

void setup()
{

  OLED_1.begin();
  OLED_1.setI2CAddress(0x3C * 2);
  OLED_1.clearBuffer();
  OLED_1.setFont(u8g2_font_ncenB14_tr);
  OLED_1.drawStr(0, 20, "Booting!");
  OLED_1.sendBuffer();

  Serial.begin(115200);
  while (!Serial)
    ; // wait for serial
  delay(200);
  wifiManager.resetSettings();
  wifiManager.setTimeout(15);
  wifiManager.autoConnect("NTPClock");

  timeClient.begin();

  // grab time from RTC to get started
  t /*= NTP_t */ = RTC.get();

  // Shut off wifi if no connection - no reason to broadcast
  if (WiFi.status() == 3)
  {
    Serial.printf("Wifi Connected; IP = %s\n", WiFi.localIP().toString().c_str());
  }
  else
  {
    WiFi.mode(WIFI_OFF);
    Serial.println("Wifi Off\n");
  }
}

void loop()
{

  unsigned long currentMillis = millis();
  unsigned long currentNTPMillis = millis();
  if (currentNTPMillis - previousNTPMillis >= ntpInterval)
  {
    previousNTPMillis = currentNTPMillis;

    // Next if only updates NTP time and RTC if a True response is returned from NTP 
    if (timeClient.update())
    {
      Serial.print("-----------\nNTP updated\n-----------\n");
      RTC.set(timeClient.getEpochTime());
      NTP_t = (timeClient.getEpochTime());
    }
  }

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    {
      if ((t = RTC.get()))
      {
        drawOLED_time(myTZ.toLocal(t));
        Serial.printf("NTP time NTP_t = %.2d:%.2d:%.2d %.4d-%.2d-%.2d \t", hour(NTP_t), minute(NTP_t), second(NTP_t), year(NTP_t), month(NTP_t), day(NTP_t));
        Serial.printf("RTC time t = %.2d:%.2d:%.2d %.4d-%.2d-%.2d \t", hour(t), minute(t), second(t), year(t), month(t), day(t));
        Serial.printf("Local RTC time t = %.2d:%.2d:%.2d %.4d-%.2d-%.2d \n", hour(myTZ.toLocal(t)), minute(myTZ.toLocal(t)), second(myTZ.toLocal(t)), year(myTZ.toLocal(t)), month(myTZ.toLocal(t)), day(myTZ.toLocal(t)));
      }
      else
      {
        if (RTC.chipPresent())
        {
          Serial.println("The DS1307 is stopped.  Please run the SetTime");
          Serial.println("example to initialize the time and begin running.");
          Serial.println();
        }
        else
        {
          Serial.println("DS1307 read error!  Please check the circuitry.");
          Serial.println();
        }
        delay(5000);
      }
    }
  }
}
