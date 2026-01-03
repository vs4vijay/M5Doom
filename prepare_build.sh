#!/bin/bash
set -e

echo "Preparing M5Doom build..."

# Create src directory if it doesn't exist
mkdir -p src

# Copy main.cpp if it exists, otherwise create it
if [ ! -f "src/main.cpp" ]; then
    cat > src/main.cpp << 'EOF'
#include "M5Cardputer.h"

// Doom generic interface
extern "C" {
    void doomgeneric_Create(int argc, char **argv);
    void doomgeneric_Tick();
}

void setup() {
    // Initialize M5Cardputer
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    
    // Initialize display
    M5Cardputer.Display.setRotation(1); // Landscape mode
    M5Cardputer.Display.fillScreen(BLACK);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(WHITE);
    
    // Show loading message
    M5Cardputer.Display.setCursor(10, 60);
    M5Cardputer.Display.println("M5Doom Loading...");
    M5Cardputer.Display.setCursor(10, 75);
    M5Cardputer.Display.println("Press any key to start");
    
    // Wait for key press
    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isPressed()) {
            break;
        }
        delay(100);
    }
    
    // Initialize Doom
    char* argv[] = {(char*)"doom", (char*)"-iwad", (char*)"doom1.wad"};
    doomgeneric_Create(3, argv);
}

void loop() {
    // Run one Doom frame
    doomgeneric_Tick();
    
    // Update M5Cardputer state
    M5Cardputer.update();
}
EOF
fi

# Copy doomgeneric files to src if not already there
echo "Copying doomgeneric source files..."
rsync -a --exclude='Makefile*' --exclude='*.o' --exclude='build/' --exclude='doomgeneric' doomgeneric/ src/

echo "Build preparation complete!"
