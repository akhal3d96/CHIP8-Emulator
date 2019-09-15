#ifndef _SDL_H
#define _SDL_H

#define GFX_WIDTH  640
#define GFX_HEIGHT 320

bool GFX_Init();
void GFX_Close();
void GFX_Draw(uint8_t gfx[]);

#endif