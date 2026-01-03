# M5Doom - DOOM for M5Stack Cardputer

A port of id Software's DOOM to the M5Stack Cardputer using doomgeneric.

![M5Cardputer](https://docs.m5stack.com/en/core/Cardputer)

## Features

- ✅ Full DOOM gameplay on 240x135 display
- ✅ 56-key QWERTY keyboard input
- ✅ Optimized for ESP32-S3
- ✅ Built with PlatformIO and Arduino framework
- ✅ CI/CD pipeline for automated builds

## Hardware Requirements

- **M5Stack Cardputer** with:
  - ESP32-S3 processor
  - 240x135 IPS display (ST7789V2)
  - 56-key QWERTY keyboard
  - 8MB Flash / 8MB PSRAM

## Software Requirements

- [PlatformIO](https://platformio.org/) or Arduino IDE
- M5Cardputer library
- M5GFX library
- M5Unified library

## Building

### Using PlatformIO (Recommended)

```bash
# Install PlatformIO
pip install platformio

# Build the firmware
pio run -e m5stack-cardputer

# Upload to device
pio run -e m5stack-cardputer -t upload

# Monitor serial output
pio device monitor
```

### Using Arduino IDE

1. Install the following libraries via Library Manager:
   - M5Cardputer
   - M5GFX
   - M5Unified

2. Select board: **ESP32S3 Dev Module**

3. Configure board settings:
   - Flash Mode: QIO
   - Flash Size: 16MB
   - Partition Scheme: Huge APP
   - PSRAM: OPI PSRAM

4. Open `M5Doom.ino` and compile/upload

## Controls

### Default Key Mapping

- **W/A/S/D** or **Arrow Keys**: Move and turn
- **Space**: Use/Open doors
- **Tab** (or `` ` ``): Fire weapon
- **Enter**: Menu select
- **Backspace**: Menu back/Exit
- **1-7**: Select weapon
- **M**: Show map
- **ESC**: Menu

### Additional Controls

- **Fn + W/A/S/D**: Alternative arrow key control

## Game Data

You need a DOOM WAD file to play. The shareware version (`doom1.wad`) is freely available.

1. Download `doom1.wad` from [here](https://distro.ibiblio.org/slitaz/sources/packages/d/doom1.wad)
2. Place it on the root directory of the SD card or in the firmware
3. Insert SD card into M5Cardputer

## Performance

- Native resolution: 240x135 pixels (scaled from Doom's native rendering)
- Frame rate: ~20-30 FPS (depends on scene complexity)
- 8-bit indexed color mode for memory efficiency

## Development

### Project Structure

```
M5Doom/
├── src/
│   ├── main.cpp                             # Arduino main application
│   └── doomgeneric_m5cardputer.cpp         # M5Cardputer port implementation
├── doomgeneric/                             # Original doom source (unchanged)
│   ├── doomgeneric.c
│   ├── doomgeneric.h
│   └── ...                                  # All original doom files
├── platformio.ini                           # PlatformIO configuration
├── build_doomgeneric.py                     # Build script for doomgeneric sources
└── .github/workflows/                       # CI/CD workflows
```

### Porting Details

The M5Cardputer port follows the doomgeneric porting approach by implementing only the platform-specific functions in `src/doomgeneric_m5cardputer.cpp`, while keeping the original `doomgeneric/` directory unchanged:

- `DG_Init()`: Initialize display and keyboard
- `DG_DrawFrame()`: Render frame buffer to display (with 8-bit palette conversion)
- `DG_SleepMs()`: Delay function
- `DG_GetTicksMs()`: Millisecond timer
- `DG_GetKey()`: Keyboard input handling
- `DG_SetWindowTitle()`: No-op for embedded device

This approach allows easy upstream updates to the doomgeneric code without conflicts.

### Display Rendering

The port uses:
- 8-bit indexed color mode (CMAP256) to save memory
- RGB565 color conversion for M5GFX
- Direct pixel pushing to ST7789 controller
- Native 240x135 resolution (Doom configured to render at this size)

## Troubleshooting

### Build Issues

**Error: Insufficient memory**
- Make sure to use partition scheme: Huge APP
- Enable PSRAM in board configuration

**Library not found**
- Install required libraries: `pio lib install`
- Or use Arduino Library Manager

### Runtime Issues

**Black screen**
- Check if WAD file is present
- Monitor serial output for error messages

**No keyboard response**
- M5Cardputer.update() is called every frame
- Check serial output for input debugging

**Slow performance**
- Reduce screen resolution (edit DOOMGENERIC_RESX/RESY in platformio.ini)
- Disable detailed graphics (lower detail level in game settings)

## Credits

- Original DOOM by id Software
- doomgeneric by ozkl
- M5Stack Cardputer by M5Stack
- Port implementation for M5Cardputer

## License

DOOM source code is licensed under the GNU General Public License v2.0.
See LICENSE file for details.

## Links

- [M5Stack Cardputer](https://docs.m5stack.com/en/core/Cardputer)
- [doomgeneric](https://github.com/ozkl/doomgeneric)
- [Original DOOM](https://github.com/id-Software/DOOM)
