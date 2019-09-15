#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT 32       

bool isGFXDraw;

typedef uint8_t byte;

typedef struct CHIP8 
{
    byte memory[0x1000];
    byte V[16];

    uint16_t opcode;
    uint16_t I;
    uint16_t PC;

    byte delay_timer;
    byte sound_timer;

    uint16_t stack[16];
    uint16_t SP;

    byte key;
    byte GFX[SCREEN_WIDTH*SCREEN_HEIGHT];
    // uint32_t screen[(SCREEN_WIDTH*SCREEN_HEIGHT)>>5];
} CHIP8;


void CHIP8_Init();
void CHIP8_Load(const char * ROM);
void CHIP8_Emulate();

CHIP8 chip8;

#endif