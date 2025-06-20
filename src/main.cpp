// #include "doomgeneric.h"
#include "../doomgeneric/doomgeneric.h"
#include "../doomgeneric/doomkeys.h"
#include "../doomgeneric/doomtype.h"
#include <M5Cardputer.h>
#include <M5Unified.h>
#include <M5GFX.h>
#include <cstring>
#include <cstdlib>

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

#ifdef __cplusplus
extern "C"
{
#endif

  void doomgeneric_Create(int argc, char **argv);
  void doomgeneric_Tick();

#ifdef __cplusplus
}
#endif

// Function to add keys to the queue
static void addKeyToQueue(int pressed, unsigned char key)
{
  unsigned short keyData = (pressed << 8) | key;
  s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
  s_KeyQueueWriteIndex++;
  s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

// Function to convert M5Cardputer keys to Doom keys
static unsigned char convertToDoomKey(char key)
{
  switch (key)
  {
    case 'w': case 'W': return KEY_UPARROW;    // Move forward
    case 's': case 'S': return KEY_DOWNARROW;  // Move backward  
    case 'a': case 'A': return KEY_LEFTARROW;  // Turn left
    case 'd': case 'D': return KEY_RIGHTARROW; // Turn right
    case ' ': return KEY_USE;                  // Use/Open
    case '\n': case '\r': return KEY_FIRE;    // Fire/Attack
    case 27: return KEY_ESCAPE;                // Escape/Menu
    case '\t': return KEY_TAB;                 // Map
    case 'q': case 'Q': return KEY_STRAFE_L;   // Strafe left
    case 'e': case 'E': return KEY_STRAFE_R;   // Strafe right
    default: return key;                       // Pass through other keys
  }
}

// Function to poll M5Cardputer keyboard
static void pollKeyboard()
{
  M5Cardputer.update();
  
  // Handle keyboard state changes
  if (M5Cardputer.Keyboard.isChange())
  {
    Keyboard_Class::KeysState keysState = M5Cardputer.Keyboard.keysState();
    
    // For simplicity, treat any key change as a press event
    // More sophisticated handling would track individual key states
    if (M5Cardputer.Keyboard.isPressed())
    {
      // Handle individual key presses from word
      if (keysState.word.length() > 0) 
      {
        // Get the last character typed
        char key = keysState.word[keysState.word.length() - 1];
        unsigned char doomKey = convertToDoomKey(key);
        addKeyToQueue(1, doomKey);
      }
      
      // Handle special keys
      if (keysState.enter) addKeyToQueue(1, KEY_ENTER);
      if (keysState.del) addKeyToQueue(1, KEY_BACKSPACE);
      if (keysState.space) addKeyToQueue(1, KEY_USE);
      if (keysState.fn) addKeyToQueue(1, KEY_FIRE);  // Use Fn as fire button
    }
  }
}

void DG_Init()
{
  // M5Cardputer should already be initialized from setup()
  // Initialize display for DOOM
  M5Cardputer.Display.setRotation(1); // Landscape orientation
  M5Cardputer.Display.fillScreen(BLACK);
  M5Cardputer.Display.setTextColor(WHITE);
  M5Cardputer.Display.println("DOOM Initializing...");
}

// External color palette from i_video.c
#ifdef CMAP256
extern struct color {
  byte r;
  byte g; 
  byte b;
  byte a;
} colors[256];
#endif

void DG_DrawFrame()
{
  // For M5Cardputer, we need to scale and convert the screen buffer
  // DOOM uses 256-color palette, but M5Cardputer display expects RGB565
  
  #ifdef CMAP256
  // Convert 8-bit palette to RGB565 for M5Cardputer display
  uint16_t* rgb565_buffer = (uint16_t*)malloc(DOOMGENERIC_RESX * DOOMGENERIC_RESY * 2);
  if (rgb565_buffer)
  {
    uint8_t* src = (uint8_t*)DG_ScreenBuffer;
    for (int i = 0; i < DOOMGENERIC_RESX * DOOMGENERIC_RESY; i++)
    {
      // Convert using the actual DOOM color palette
      uint8_t palIndex = src[i];
      struct color c = colors[palIndex];
      
      // Convert RGB888 to RGB565
      uint16_t r = (c.r >> 3) & 0x1F;
      uint16_t g = (c.g >> 2) & 0x3F; 
      uint16_t b = (c.b >> 3) & 0x1F;
      rgb565_buffer[i] = (r << 11) | (g << 5) | b;
    }
    M5Cardputer.Display.pushImage(0, 0, DOOMGENERIC_RESX, DOOMGENERIC_RESY, rgb565_buffer);
    free(rgb565_buffer);
  }
  #else
  M5Cardputer.Display.pushImage(0, 0, DOOMGENERIC_RESX, DOOMGENERIC_RESY, (uint16_t *)DG_ScreenBuffer);
  #endif
}

void DG_SleepMs(uint32_t ms)
{
  delay(ms);
}

uint32_t DG_GetTicksMs()
{
  return millis();
}

// int DG_GetKey(int* pressed, unsigned char* key) {
//   *pressed = 0;
//   *key = 0;

//   if(M5.BtnA.wasPressed()) {
//     *pressed = 1;
//     *key = KEY_ENTER;
//     return 1;
//   }
//   else if(M5.BtnB.wasPressed()) {
//     *pressed = 1;
//     *key = KEY_FIRE;
//     return 1;
//   }
//   else if(M5.BtnC.wasPressed()) {
//     *pressed = 1;
//     *key = KEY_USE;
//     return 1;
//   }

//   return 0;
// }

int DG_GetKey(int *pressed, unsigned char *doomKey)
{
  if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex)
  {
    // key queue is empty
    return 0;
  }
  else
  {
    unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
    s_KeyQueueReadIndex++;
    s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

    *pressed = keyData >> 8;
    *doomKey = keyData & 0xFF;

    return 1;
  }

  return 0;
}

void setup()
{
  // Initialize M5Cardputer with SD card support
  M5Cardputer.begin(true, true, true, true);  // Enable: Display, Power, Speaker, SD
  
  // Initialize key queue
  memset(s_KeyQueue, 0, KEYQUEUE_SIZE * sizeof(unsigned short));
  s_KeyQueueWriteIndex = 0;
  s_KeyQueueReadIndex = 0;
  
  // Create arguments to specify the WAD file
  // Try multiple possible locations for the WAD file
  static char* argv[] = {
    (char*)"doomgeneric",
    (char*)"-iwad",
    (char*)"miniwad.wad"  // Look for WAD file in current directory/SD card
  };
  
  doomgeneric_Create(3, argv);
}

void loop()
{
  pollKeyboard(); // Poll M5Cardputer keyboard for input
  doomgeneric_Tick();
}

void DG_SetWindowTitle(const char * title)
{
  // M5Cardputer doesn't have window titles, so this is a no-op
}

// int main(int argc, char **argv)
// {
//   doomgeneric_Create(argc, argv);

//   while (1)
//   {
//     doomgeneric_Tick();
//   }

//   return 0;
// }