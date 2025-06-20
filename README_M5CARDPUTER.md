# M5Doom for M5Cardputer

This is a port of DOOM for the M5 Cardputer using the doomgeneric framework.

## Hardware Requirements

- M5 Cardputer device
- MicroSD card (recommended for storing WAD files)

## Setup Instructions

### 1. WAD File Setup

You need a DOOM WAD file to run the game. The easiest option is to use the included `miniwad.wad`:

1. Copy `miniwad.wad` from the `doomgeneric/` directory to your M5Cardputer's SD card
2. Insert the SD card into your M5Cardputer

Alternative WAD files you can use:
- `doom1.wad` (Shareware version)
- `doom.wad` (Full version, if you own it)

### 2. Building and Flashing

1. Install PlatformIO
2. Clone this repository
3. Build and upload:
   ```bash
   platformio run --target upload
   ```

## Controls

### M5Cardputer Keyboard Controls

- **W/S**: Move forward/backward
- **A/D**: Turn left/right  
- **Q/E**: Strafe left/right
- **Space**: Use/Open doors
- **Enter**: Fire/Attack
- **Fn**: Fire/Attack (alternative)
- **ESC**: Menu
- **Tab**: Map

## Display

The game is rendered at 240x135 resolution to match the M5Cardputer display. The original DOOM color palette is converted to RGB565 format for the display.

## Technical Details

### Key Components

- **Input Handling**: M5Cardputer keyboard input is mapped to DOOM controls
- **Display**: 8-bit color palette converted to RGB565 for the display
- **File System**: WAD files loaded from SD card
- **Memory**: Optimized for ESP32 constraints

### Modifications Made

1. **Display Integration**: 
   - Custom `DG_DrawFrame()` with palette conversion
   - Resolution scaled to 240x135

2. **Input System**:
   - M5Cardputer keyboard polling
   - Key mapping to DOOM controls
   - Key queue system for input buffering

3. **Platform Integration**:
   - M5Cardputer initialization
   - SD card support for WAD files
   - ESP32-compatible build configuration

## Troubleshooting

### Common Issues

1. **Game won't start**: Ensure `miniwad.wad` is on the SD card
2. **Display issues**: Check that M5Cardputer display is properly initialized
3. **No input response**: Verify keyboard functionality with M5Cardputer examples

### Performance Notes

- The ESP32 has limited memory, so some levels may not load
- Frame rate may vary depending on scene complexity
- Consider using the smallest WAD file (miniwad.wad) for best performance

## License

This port maintains the same license as the original DOOM source code and doomgeneric framework.