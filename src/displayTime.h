

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
  // OLED_1.setFont(u8g2_font_tenstamps_mf);
  OLED_1.setFont(u8g2_font_michaelmouse_tu);
  OLED_1.drawStr(95, 60, secondBuffer);
  OLED_1.sendBuffer(); // transfer internal memory to the display
}