
#ifndef RENDER_H
#define RENDER_H


#include <linmath.h>
#include "../helpers.h"
#include "../objects/shapes.h"
#include "../engine/physics.h"
#include <SDL3/SDL.h>


struct Render_State
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    real32 height;
    real32 width;
    SDL_DisplayID displayID;
} ;

//Initialize the rendering device 
void
renderInit(void);

//rendering block beginning
void
renderBegin(void);
//rendering block ending
void
renderEnd(void);

//render a quad to the screen
void renderQuad(struct Quad* quad);

//Helper function to render the bounding box to the screen
void
renderAABB(struct AABB *aabb, vec4 color);

//render all bounding boxes using renderAABB()
void
drawAllAABB(void);

#endif
