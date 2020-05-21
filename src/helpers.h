// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress &address)
{
#ifdef NTPDEBUG
    {
        Serial.println("sending NTP packet...");
    };
#endif
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;          // Stratum, or type of clock
    packetBuffer[2] = 6;          // Polling Interval
    packetBuffer[3] = 0xEC;       // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(address, 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}

time_t getNTPtime(void)
{

    sendNTPpacket(timeServer); // send an NTP packet to a time server
    delay(100);
    if (udp.parsePacket())
    {
#ifdef NTPDEBUG
        Serial.println("packet received");
#endif
        // We've received a packet, read the data from it
        udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

        //the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, esxtract the two words:

        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        //Calc below updates the local NTP_t to unixtime
        NTP_t = (highWord << 16 | lowWord) - 2208988800UL;

#ifdef NTPDEBUG
        Serial.print("Seconds since Jan 1 1900 = ");
        Serial.println(secsSince1900);
#endif
        // now convert NTP time into everyday time:
#ifdef NTPDEBUG
        Serial.print("Unix time = ");
#endif
        // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
        const unsigned long seventyYears = 2208988800UL;
        // subtract seventy years to get from NTPtime (1900) to unixtime:
        unsigned long unixTime = secsSince1900 - seventyYears;

        // print Unix time:
#ifdef NTPDEBUG
        Serial.println(unixTime);
#endif


        (RTC.set(unixTime));
        return NTP_t;
    }
}

void drawOLED_1(time_t locoMoco)
{
  char hourBuffer[10];
  sprintf(hourBuffer, "%02u", hour(locoMoco));
  // itoa(tm.Hour), hourBuffer, 10);

  char minuteBuffer[10];
  sprintf(minuteBuffer, "%02u", minute(locoMoco));
  // itoa(tm.Minute, minuteBuffer, 10);

  OLED_1.clearBuffer(); // clear the internal memory
  OLED_1.setFont(u8g2_font_logisoso42_tn);
  // char buffer[7];
  OLED_1.drawStr(0, 42, hourBuffer);

  OLED_1.drawStr(60, 62, minuteBuffer);
  OLED_1.sendBuffer(); // transfer internal memory to the display
}