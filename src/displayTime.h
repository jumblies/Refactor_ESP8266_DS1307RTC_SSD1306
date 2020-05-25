void drawOLED_wifiReset(void)
{
  OLED_1.clearBuffer();
  OLED_1.setFont(u8g2_font_ncenB14_tr);
  OLED_1.drawStr(0, 20, "wifi reset!");
  OLED_1.sendBuffer();
}

void drawOLED_boot(void)
{
  OLED_1.begin();
  OLED_1.setI2CAddress(0x3C * 2);
  OLED_1.clearBuffer();
  // OLED_1.setFont(u8g2_font_ncenB14_tr);
  // OLED_1.drawStr(0, 20, "Booting!");

  OLED_1.setFont(u8g2_font_open_iconic_www_8x_t);
  OLED_1.drawGlyph(30, 64, 81);

  OLED_1.sendBuffer();
}

void drawOLED_time(time_t locoMoco)
//locomoco is the cutesy name for the local time var
{
  char hourBuffer[10];
  sprintf(hourBuffer, "%02u:%02u", hour(locoMoco), minute(locoMoco));
  OLED_1.clearBuffer(); // clear the internal memory
  OLED_1.setFont(u8g2_font_logisoso42_tn);
  char secondBuffer[3];
  sprintf(secondBuffer, "%02u", second(locoMoco));

  OLED_1.drawStr(0, 42, hourBuffer);
  OLED_1.setFont(u8g2_font_tenstamps_mf);
  // OLED_1.setFont(u8g2_font_michaelmouse_tu); //cartoon-y and hard to read
  // OLED_1.setFont(u8g2_font_pieceofcake_mel_tr); //blocky and hard to read
  // OLED_1.setFont(u8g2_font_sticker_mel_tr); //blocky and hard to read
  OLED_1.drawStr(95, 60, secondBuffer);

  if (NTP_t){
    OLED_1.setFont(u8g2_font_unifont_t_symbols);
    OLED_1.drawGlyph(36, 60, 9200); //alarmclock

  } 

  // // Fun with glyphs
  // if (second(locoMoco) % 2 /*== 0*/)
  // {
  //   // OLED_1.setFont(u8g2_font_unifont_t_0_76);
  //   OLED_1.setFont(u8g2_font_unifont_t_0_76);
  //   OLED_1.drawGlyph(20, 60, 9762); //nuclear
    
  // }
  // else
  // {
  //   OLED_1.setFont(u8g2_font_unifont_t_0_76);
  //   OLED_1.drawGlyph(20, 60, 9763);  //Biohazard
  //   // OLED_1.drawGlyph(20, 60, 9783);
  // }
  // wifi glyph
  if (WiFi.status() == 3)
  {
    OLED_1.setFont(u8g2_font_siji_t_6x10);
    OLED_1.drawGlyph(8, 60, 57584);
  }
  OLED_1.sendBuffer(); // transfer internal memory to the display
}