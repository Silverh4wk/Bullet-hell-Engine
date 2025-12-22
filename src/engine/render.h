#pragma once

#include <SDL3/SDL.h>
#include <linmath.h>
#include "../helpers.h"

#include "../objects/shapes.h"
typedef struct Render_State
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    real32 height;
    real32 width;
    SDL_DisplayID displayID;
}Render_State;

void renderInit(void);
void renderBegin(void);
void renderEnd(void);
void renderQuad(Quad quad);
void renderQuad(vec2 pos, vec2 size, vec4 color);

