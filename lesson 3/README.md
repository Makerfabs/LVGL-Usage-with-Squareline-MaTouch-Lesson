# Arduino project for Makerfabs Matouch E32S3RGB40 Display with ESP32 and 4" Display (with touch-screen)


## Prerequisites

1. Arduino-IDE should be installed on your system. (The build and board was tested with v1.8.19 but should work with v2 versions too.)
2. In `Tools`/`Boards Manager` select and install version 2.0.17 of `esp32` board-support package. (Contains the ESP toolchains for the board's Tensilica Xtensa ESP32 MCU, versions above 3.0.0 don't support the used Arduino_ESP32RGBpanel() function. )
3. (The essence of `GFX_Library_for_Arduino` (`Arduino_GFX_Library`) v1.3.1 (newer versions won't like the init-code) with the ST7701 display-driver is included in the local `libraries` folder of this board-template, but if you want to us Arduino's built-in one you can delete it and install v1.3.1 to Arduino-IDE with `Tools`/`Manage Libraries`.)

## Board settings (retained between Arduino-IDE sessions)

1. In `Tools` select `ESP32 Arduino` / `ESP32S3 Dev Module` as board
2. Set the parameters for your board: Flash-mode to `QIO 80MHz`, Flash-size to `16MB`, PSRAM to `OPI PSRAM`, (Partition-scheme to `Huge App`)
3. If you'll want to upload to the device from Arduino-IDE, select the programmer tool and port in `Tools` menu: Arduino-IDE uses the `esptool` of ESP-IDF in the background.
4. (The `partitions.csv` file in `ui` subfolder of this sketch-folder beside `ui.ino` should override the `Huge App`/etc. partitioning scheme setting, for example to give bigger app-partition. But with v2 of esp32 package this seems not effective.)

## Compilation

1. Open the `.ino` file in Arduino
2. Go to File/Preferences and set Sketchbook location to the path of your UI project (where this README is located). The setting is retained between Arduino-IDE sessions.
3. Build the project

## Burning

If the built-in Arduino-IDE chip-programmer is working you can go with it.
If it's not working on your system, you can find the built ELF's flashable version in a `ui.ino.bin` file in the folder (like `tmp`) where Arduino-IDE puts the builds.
This `ui.ino.bin` file can be burnt to the board's app0 partition at offset 0x10000 by the ESP-IDF command: `esptool.py  write_flash  0x10000 ui.ino.bin`. (Offset might differ with other patition scheme, get app0 offset by `esptool.py read_flash 0x8000 0xc00 ptable.img` and `gen_esp32part.py ptable.img`.)
(If you want to make a backup before overwriting the flash in the device, you can use esptool's read command like: `esptool.py -b 460800 read_flash 0 ALL Backup.bin`.)


2024 SquareLine

