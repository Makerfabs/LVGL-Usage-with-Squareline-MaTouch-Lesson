# Arduino project for Makerfabs Matouch MA1989BRK TFT Breakout board with ESP32 and 1.9" Display (with touch)


## Prerequisites

1. Arduino-IDE should be installed on your system. (The build and board was tested with v1.8.19 but should work with v2 versions too.)
2. In `Tools`/`Boards Manager` select and install `esp32` board-support package (contains the ESP toolchains for the board's Tensilica Xtensa ESP32 MCU, the board was tested with 3.0.0 and 2.0.17, lower versions might not be liked by the included display-driver.)
3. (The essence of `GFX_Library_for_Arduino` (`Arduino_GFX_Library`) with the display-driver is included in the local `libraries` folder of this board-template, but if you want to us Arduino's built-in one you can delete it and install to Arduino-IDE with `Tools`/`Manage Libraries`.)

## Board settings (retained between Arduino-IDE sessions)

1. In `Tools` select `ESP32 Arduino` / `ESP32S3 Dev Module` as board
2. Set the parameters for your board: Flash-mode to `QIO 80MHz`, Flash-size to `16MB`, PSRAM to `OPI PSRAM`, (Partition-scheme to `Huge App`)
3. If you'll want to upload to the device from Arduino-IDE, select the programmer tool and port in `Tools` menu: Arduino-IDE uses the `esptool` of ESP-IDF in the background.
4. (The `partitions.csv` file in `ui` subfolder of this sketch-folder beside `ui.ino` overrides the `Huge App`/`Custom`/etc. partitioning scheme setting. If you really want those you need to delete this file, or edit it for different partition sizes.)

## Compilation

1. Open the `.ino` file in Arduino
2. Go to File/Preferences and set Sketchbook location to the path of your UI project (where this README is located). The setting is retained between Arduino-IDE sessions.
3. Build the project

## Burning

If the built-in Arduino-IDE chip-programmer is working you can go with it.
If it's not working on your system, you can find the built merged .bin file in the folder (like `tmp`) where Arduino-IDE puts the builds.
The whole .bin (the merged 16MB one containing all partitions) can be burnt to the board by the ESP-IDF command: `esptool.py -b 460800 write_flash 0 ui.ino.merged.bin`
(If you want to make a backup before overwriting the flash in the device, you can use esptool's read command like: `esptool.py -b 460800 read_flash 0 ALL Backup.bin`.)


2024 SquareLine

