// M5Cardputer port for doomgeneric
#include "M5Cardputer.h"
#include "doomkeys.h"
#include "doomgeneric.h"

#include <stdlib.h>
#include <stdio.h>

// Display configuration
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 135

// Key queue for input handling
#define KEYQUEUE_SIZE 16
static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

// Frame buffer for 8-bit palette mode
static uint16_t* frameBuffer = NULL;

// Doom palette (default VGA palette)
static uint16_t doomPalette[256];

// Convert Doom palette color to RGB565
static uint16_t toRGB565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Initialize default VGA palette
void initPalette() {
    // This is a simplified palette initialization
    // Doom will override this with actual palette data
    for (int i = 0; i < 256; i++) {
        uint8_t r = (i & 0xE0);
        uint8_t g = (i & 0x1C) << 3;
        uint8_t b = (i & 0x03) << 6;
        doomPalette[i] = toRGB565(r, g, b);
    }
}

// Convert M5Cardputer key to Doom key
static unsigned char convertToDoomKey(char key) {
    // Special key codes for M5Cardputer arrows
    #define M5_KEY_LEFT  0xB4
    #define M5_KEY_UP    0xB5
    #define M5_KEY_DOWN  0xB6
    #define M5_KEY_RIGHT 0xB7
    
    // Handle special keys
    switch (key) {
        case 0x08: // Backspace
            return KEY_ESCAPE;
        case 0x0D: // Enter
        case 0x0A: // LF
            return KEY_ENTER;
        case 0x09: // Tab
            return KEY_TAB;
        case M5_KEY_LEFT:
            return KEY_LEFTARROW;
        case M5_KEY_RIGHT:
            return KEY_RIGHTARROW;
        case M5_KEY_UP:
            return KEY_UPARROW;
        case M5_KEY_DOWN:
            return KEY_DOWNARROW;
        case ' ':
            return KEY_USE;
        case 'w':
        case 'W':
            return KEY_UPARROW;
        case 's':
        case 'S':
            return KEY_DOWNARROW;
        case 'a':
        case 'A':
            return KEY_LEFTARROW;
        case 'd':
        case 'D':
            return KEY_RIGHTARROW;
        case 0x1B: // ESC
            return KEY_ESCAPE;
        default:
            // Convert to lowercase for standard keys
            if (key >= 'A' && key <= 'Z') {
                return key + 32; // Convert to lowercase
            }
            // Handle Control key as fire
            if (key == '`' || key == '~') {
                return KEY_FIRE;
            }
            return key;
    }
}

// Add key to input queue
static void addKeyToQueue(int pressed, unsigned char keyCode) {
    unsigned char doomKey = convertToDoomKey(keyCode);
    unsigned short keyData = (pressed << 8) | doomKey;
    
    s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
    s_KeyQueueWriteIndex++;
    s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

// Process keyboard input
static void handleKeyInput() {
    M5Cardputer.update();
    
    // Check for key press or release
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            
            // Handle Fn key combinations first (for arrow keys)
            if (status.fn) {
                // Fn + WASD for arrows when Fn is held
                for (auto i : status.word) {
                    if (i == 'w' || i == 'W') {
                        addKeyToQueue(1, M5_KEY_UP);
                        continue; // Skip normal processing
                    }
                    else if (i == 's' || i == 'S') {
                        addKeyToQueue(1, M5_KEY_DOWN);
                        continue;
                    }
                    else if (i == 'a' || i == 'A') {
                        addKeyToQueue(1, M5_KEY_LEFT);
                        continue;
                    }
                    else if (i == 'd' || i == 'D') {
                        addKeyToQueue(1, M5_KEY_RIGHT);
                        continue;
                    }
                    // For other keys with Fn, process normally
                    addKeyToQueue(1, i);
                }
            } else {
                // Handle normal key presses
                for (auto i : status.word) {
                    addKeyToQueue(1, i);
                }
            }
            
            // Handle special keys
            for (auto i : status.del) {
                addKeyToQueue(1, 0x08); // Backspace
            }
            for (auto i : status.enter) {
                addKeyToQueue(1, 0x0D); // Enter
            }
            for (auto i : status.tab) {
                addKeyToQueue(1, 0x09); // Tab
            }
        } else {
            // Key release - send release events for all keys
            // This helps prevent stuck keys
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            for (auto i : status.word) {
                addKeyToQueue(0, i);
            }
        }
    }
}

// DG_Init - Initialize display and input
void DG_Init() {
    // Allocate frame buffer using default memory (optimal for display performance)
    // Using DMA-capable memory for better display transfer speed
    frameBuffer = (uint16_t*)heap_caps_malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t), MALLOC_CAP_DMA);
    
    if (frameBuffer == NULL) {
        // Fallback to default memory if DMA allocation fails
        frameBuffer = (uint16_t*)heap_caps_malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t), MALLOC_CAP_DEFAULT);
    }
    
    if (frameBuffer == NULL) {
        Serial.println("FATAL: Failed to allocate frame buffer!");
        // On embedded systems, halting execution is appropriate for critical errors
        // as there's no recovery mechanism without reboot
        while(1) delay(1000);
    }
    
    // Initialize palette
    initPalette();
    
    // Clear display
    M5Cardputer.Display.fillScreen(BLACK);
    
    Serial.println("DG_Init complete");
}

// DG_DrawFrame - Render the frame buffer to display
void DG_DrawFrame() {
    // Convert 8-bit indexed color to RGB565
    pixel_t* src = DG_ScreenBuffer;
    uint16_t* dst = frameBuffer;
    
    // Scale from 240x135 (native) - Doom runs at this resolution
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            uint8_t paletteIndex = src[y * DOOMGENERIC_RESX + x];
            dst[y * DISPLAY_WIDTH + x] = doomPalette[paletteIndex];
        }
    }
    
    // Push to display
    M5Cardputer.Display.startWrite();
    M5Cardputer.Display.setAddrWindow(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    M5Cardputer.Display.pushPixels(frameBuffer, DISPLAY_WIDTH * DISPLAY_HEIGHT);
    M5Cardputer.Display.endWrite();
    
    // Handle keyboard input
    handleKeyInput();
}

// DG_SleepMs - Sleep for specified milliseconds
void DG_SleepMs(uint32_t ms) {
    delay(ms);
}

// DG_GetTicksMs - Get milliseconds since boot
uint32_t DG_GetTicksMs() {
    return millis();
}

// DG_GetKey - Get next key from input queue
int DG_GetKey(int* pressed, unsigned char* doomKey) {
    if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex) {
        // Key queue is empty
        return 0;
    } else {
        unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
        s_KeyQueueReadIndex++;
        s_KeyQueueReadIndex %= KEYQUEUE_SIZE;
        
        *pressed = keyData >> 8;
        *doomKey = keyData & 0xFF;
        
        return 1;
    }
}

// DG_SetWindowTitle - Not applicable for embedded device
void DG_SetWindowTitle(const char* title) {
    // No-op for M5Cardputer
    Serial.print("Title: ");
    Serial.println(title);
}
