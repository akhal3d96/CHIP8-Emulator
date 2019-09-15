#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "sdl.h"


SDL_Window   * gWindow = NULL;
SDL_Surface  * gScreenSurface = NULL;

bool GFX_Init()
{
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr,"SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
    } 
    else 
    {
        gWindow = SDL_CreateWindow("CHIP-8 Emulator",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, GFX_WIDTH, GFX_HEIGHT,SDL_WINDOW_SHOWN);
        if(gWindow == NULL) 
        {
            fprintf(stderr,"Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        } 
        else 
        {
            gScreenSurface = SDL_GetWindowSurface(gWindow);
            SDL_FillRect(gScreenSurface, NULL,SDL_MapRGB(gScreenSurface->format, 0, 0, 0));

        }
    }

    return success;
}

void GFX_Close()
{
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    SDL_Quit();
}

void GFX_Draw(uint8_t gfx[])
{
    SDL_LockSurface(gScreenSurface);
    for (int i = 0; i < GFX_HEIGHT; i++)
        for (int j = 0; j < GFX_WIDTH; j++){
            ((Uint32 *)gScreenSurface->pixels)[j+i*gScreenSurface->w] = gfx[(j/10)+(i/10)*64] ? UINT32_MAX : 0;
    }
    SDL_UnlockSurface(gScreenSurface);
    SDL_UpdateWindowSurface(gWindow);
    SDL_Delay((1.0/60)*1000);
}