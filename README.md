# CHIP-8 Emulator

A modern implementation of [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8).

<p align="center">
<img src="https://raw.githubusercontent.com/nemoload/CHIP8-Emulator/master/invaders.gif">
</p>

## Requirements
* GNU/Linux or Microsoft Windows (MSVC) environment
* SDL2
* cmake (building)

## Build
### Linux
```
$ git clone https://github.com/nemoload/CHIP8-Emulator
$ cd CHIP8-Emulator
$ mkdir build
$ cd build
$ cmake .. -DDEFINE_DEBUG=OFF
$ make
```
### Windows
1. Clone the repository or download it.
2. Import it in Microsoft Visual Studio.
3. Download SDL2 development headers from [here](https://www.libsdl.org/download-2.0.php) and extract it anywhere on your computer.
4. Open CMakeLists.txt and from CMakeCache settings change `SDL2_DIR` variable to the location you extracted SDL2 headers.
5. Build it.
6. In order to make it run, copy SDL2.dll to your build directory.

## Usage
The `ROMS` directory contains public domain CHIP-8 programs which can be run using the emulator.

After building it, the executable file can be used like:

`$ ./chip8emu ../ROMS/PONG #CHIP-8 program location`