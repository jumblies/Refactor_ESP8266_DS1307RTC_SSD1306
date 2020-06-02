/*TODO

STARTED Add adjustable timezones
DONE Add resetSettings jumper code
STARTED Add display code
TODO  move ntp update loop to seperate function and just call it from here and loop below to shorten code    


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
#include <EEPROM.h>
// #define NTPDEBUG

uint8_t tzIndex;
uint8_t ee_tzIndex; //copy of tzIndex persisted in EEPROM
const char *dstNames[] = {"EDT", "CDT", "MDT", "PDT"};
const char *stdNames[] = {"EST", "CST", "MST", "PST"};
const int dstOffsets[] = {-240, -300, -360, -420};
const int stdOffsets[] = {-300, -360, -420, -480};

#include "objects.h"

// Globals
unsigned long previousMillis = 0;
unsigned long previousNTPMillis = 0;
// Global Timer vars
const uint16_t interval = 1 * 1000; // 1 second loop interval
const uint8_t ntpSeconds = 300; //NTP update interval in seconds
const uint16_t ntpInterval = ntpSeconds * 1000; // interval for NTP checks

// Late includes - leave here.  needs objects instantiated first
#include "displayTime.h"

void setup()
{
  drawOLED_boot();

  Serial.begin(115200);
  while (!Serial)
  {
    delay(200);
  }

  //  wifiManager.resetSettings();

  uint8_t myEEPROM_data = 255;
  uint8_t myEEPROM_address = 0;

  EEPROM.begin(512);

  if (EEPROM.read(0) != myEEPROM_data)
  {
    EEPROM.write(0, myEEPROM_data);
    if (EEPROM.commit())
    {
      Serial.println("EEPROM successfully committed");
    }
    else
    {
      Serial.println("ERROR! EEPROM commit failed");
    }
  }

  Serial.printf("\n Your EEPROM value at %.3d is %.3d\n", myEEPROM_address, EEPROM.read(myEEPROM_address));

  wifiManager.setTimeout(15);
  wifiManager.autoConnect("NTPClock");

  timeClient.begin();

  // grab time from RTC to get started
  t /*= NTP_t */ = RTC.get();

  // Shut off wifi if no connection - no reason to broadcast
  if (WiFi.status() == 3)
  {
    Serial.printf("Wifi Connected; IP = %s\n", WiFi.localIP().toString().c_str());

    // TODO move this to seperate function and just call it from here and loop below to shorten code
    if (timeClient.update())
    {
      Serial.print("-----------\nNTP updated\n-----------\n");
      RTC.set(timeClient.getEpochTime());
      NTP_t = (timeClient.getEpochTime());
    }
  }
  else
  {
    WiFi.forceSleepBegin();
    delay(1);
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
        Serial.printf("Last NTP_t = %.2d:%.2d:%.2d %.4d-%.2d-%.2d \t", hour(NTP_t), minute(NTP_t), second(NTP_t), year(NTP_t), month(NTP_t), day(NTP_t));
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
