// Instantiate objects
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org");
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

// double reset detector objects
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);
