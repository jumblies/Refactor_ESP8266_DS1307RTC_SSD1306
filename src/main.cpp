

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


// #define NTPDEBUG ;

IPAddress timeServer(10, 10, 10, 102);

unsigned int localPort = 2390;      // local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48;     // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// Instantiate objects
WiFiUDP udp;
tmElements_t tm;
time_t NTP_t; //Time object for holding NTP time
time_t t;     // time object for holding RTC time
WiFiManager wifiManager;
U8G2_SH1106_128X64_NONAME_F_HW_I2C OLED_1(U8G2_R0, U8X8_PIN_NONE);


// Timezone Objects
TimeChangeRule myDST = {"EDT", Second, Sun, Mar, 2, -240}; //UTC - 4 hours
TimeChangeRule mySTD = {"EST", First, Sun, Nov, 2, -300};  //UTC - 5 hours
Timezone myTZ(myDST, mySTD);
TimeChangeRule *tcr;

// Globals
unsigned long previousMillis = 0;
unsigned long previousNTPMillis = 0;
// Global Timer vars
const long interval = 1 * 1000;
const long ntpInterval = 5 * 1000; // interval for NTP checks

const char *monthName[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

// Late includes - leave here.  needs objects instantiated first
#include "helpers.h"

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

  wifiManager.autoConnect("NTPClock");
  Serial.println(WiFi.localIP());
  Serial.println("DS1307RTC Read Test");
  Serial.println("-------------------");
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());

  // grab time from RTC to get started

  t = NTP_t = RTC.get();
}

void loop()
{

  unsigned long currentMillis = millis();
  unsigned long currentNTPMillis = millis();
  if (currentNTPMillis - previousNTPMillis >= ntpInterval)
  {
    previousNTPMillis = currentNTPMillis;
    NTP_t = getNTPtime();
  }

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    {
      if (t = RTC.get())
      {
        drawOLED_1(myTZ.toLocal(t));
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
