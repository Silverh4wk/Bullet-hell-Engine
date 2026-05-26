#ifndef RENDER_H
#define RENDER_H


#include <linmath.h>
#include "../helpers.h"
#include "../objects/shapes.h"
#include "../engine/physics.h"
#include "render/render_internal.h"
#include <SDL3/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

struct Render_State
{
    SDL_Window *
    window;
    SDL_Renderer *renderer;
    int height;
    int width;
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

void
renderSubmitShape(struct Shape *shape);
//render a shape to the screen

void
renderShape(struct Shape* shape);

//Helper function to render the bounding box to the screen
void
renderAABB(struct AABB *aabb, vec4 color, int toggle);


// render the ecs
void
renderECS(void);

//render all bounding boxes using renderAABB()
void
drawAllAABB(int toggle);

void
renderShutdown(void);

void
renderInitText(struct RenderStateInternal *state);

void
renderText(char *text, real32 x, real32 y, vec4* color);

#endif
