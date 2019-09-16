#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "chip8.h"
#include "sdl.h"

SDL_Event gEvent;


int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    printf("Usgae: %s <CHIP-8 ROM>\n", *argv);
    return 0;
  }
  CHIP8_Init();
  CHIP8_Load(argv[1]);
  
  if (!GFX_Init())
  {
    GFX_Close();
    return -1;
  }

  setbuf(stdout, NULL);
  
  for(;;)
  {
    CHIP8_Emulate();
    while(SDL_PollEvent(&gEvent) != 0)
    {
        if(gEvent.type == SDL_QUIT)
        {
          GFX_Close();
          return 0; 
        }
        else if (gEvent.type == SDL_KEYDOWN)
        {
          switch(gEvent.key.keysym.sym) 
          {
            case SDLK_0:
            case SDLK_KP_0:
              chip8.key = 0x0;
              break;
            case SDLK_1:
            case SDLK_KP_1:
              chip8.key = 0x1;
              break;
            case SDLK_DOWN:
            case SDLK_2:
            case SDLK_KP_2:
              chip8.key = 0x2;
              break;
            case SDLK_3:
            case SDLK_KP_3:
              chip8.key = 0x3;
              break;
            case SDLK_4:
            case SDLK_LEFT:
            case SDLK_KP_4:
              chip8.key = 0x4;
              break;
            case SDLK_5:
            case SDLK_KP_5:
            case SDLK_SPACE:
              chip8.key = 0x5;
              break;
            case SDLK_6:
            case SDLK_KP_6:
            case SDLK_RIGHT:
              chip8.key = 0x6;
              break;
            case SDLK_7:
            case SDLK_KP_7:
              chip8.key = 0x7;
              break;
            case SDLK_UP:
            case SDLK_8:
            case SDLK_KP_8:
              chip8.key = 0x8;
              break;
            case SDLK_9:
            case SDLK_KP_9:
              chip8.key = 0x9;
              break;
            case SDLK_a:
              chip8.key = 0xA;
              break;
            case SDLK_b:
              chip8.key = 0xB;
              break;
            case SDLK_c:
              chip8.key = 0xC;
              break;
            case SDLK_d:
              chip8.key = 0xD;
              break;
            case SDLK_e:
              chip8.key = 0xE;
              break;
            case SDLK_f:
              chip8.key = 0xF;
              break;
            default: break;
          }
        }
        else if(gEvent.type == SDL_KEYUP)
        {
          chip8.key = 0xFF;
        }
    }
    if(isGFXDraw) 
    {
      GFX_Draw(chip8.GFX);
      isGFXDraw = false;
    }
  }
  GFX_Close();
  return 0;
}
