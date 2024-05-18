#include "doomgeneric.h"
#include <M5Cardputer.h>
#include <M5Unified.h>
#include <M5GFX.h>

void DG_Init() {
  M5.begin();
  // Initialize M5Stack Cardputer
}

void DG_DrawFrame() {
  M5.Lcd.drawBitmap(0, 0, DOOMGENERIC_RESX, DOOMGENERIC_RESY, (uint16_t*)DG_ScreenBuffer);
}

void DG_SleepMs(uint32_t ms) {
  delay(ms);
}

uint32_t DG_GetTicksMs() {
  return millis(); 
}

int DG_GetKey(int* pressed, unsigned char* key) {
  *pressed = 0;
  *key = 0;

  if(M5.BtnA.wasPressed()) { 
    *pressed = 1;
    *key = KEY_ENTER;
    return 1;
  }
  else if(M5.BtnB.wasPressed()) {
    *pressed = 1;
    *key = KEY_FIRE;  
    return 1;
  }
  else if(M5.BtnC.wasPressed()) {
    *pressed = 1;
    *key = KEY_USE;
    return 1;
  }

  return 0;
}