#!/bin/bash

# Script to prepare SD card with WAD file for M5Doom

echo "M5Doom SD Card Setup Script"
echo "=========================="

# Check if SD card mount point is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <SD_CARD_MOUNT_POINT>"
    echo "Example: $0 /media/sdcard"
    exit 1
fi

SD_MOUNT="$1"

# Check if mount point exists
if [ ! -d "$SD_MOUNT" ]; then
    echo "Error: SD card mount point '$SD_MOUNT' does not exist"
    exit 1
fi

# Check if we're in the right directory
if [ ! -f "doomgeneric/miniwad.wad" ]; then
    echo "Error: Please run this script from the M5Doom project root directory"
    exit 1
fi

echo "Copying WAD files to SD card at: $SD_MOUNT"

# Copy the minimal WAD file
cp doomgeneric/miniwad.wad "$SD_MOUNT/"
echo "✓ Copied miniwad.wad"

# Optionally copy other WAD files if they exist
if [ -f "doomgeneric/doom1.wad" ]; then
    cp doomgeneric/doom1.wad "$SD_MOUNT/"
    echo "✓ Copied doom1.wad (shareware)"
fi

# Create a simple readme on the SD card
cat > "$SD_MOUNT/README.txt" << EOF
M5Doom WAD Files
================

This SD card contains DOOM WAD files for use with M5Doom on M5Cardputer.

Files:
- miniwad.wad: Minimal DOOM game data (recommended)
- doom1.wad: DOOM shareware version (if available)

To play:
1. Insert this SD card into your M5Cardputer
2. Power on the M5Cardputer with M5Doom firmware
3. The game should start automatically

Controls:
- WASD: Movement
- Space: Use/Open
- Enter/Fn: Fire
- ESC: Menu
EOF

echo "✓ Created README.txt on SD card"
echo ""
echo "SD card setup complete!"
echo "You can now safely eject the SD card and insert it into your M5Cardputer."