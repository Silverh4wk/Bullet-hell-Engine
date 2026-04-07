#ifndef RENDER_H
#define RENDER_H


#include <linmath.h>
#include "../helpers.h"
#include "../objects/shapes.h"
#include "../engine/physics.h"
#include "render/render_internal.h"
#include <SDL3/SDL.h>


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
renderSubmitQuad(struct Quad *quad);
//render a quad to the screen
void renderQuad(struct Quad* quad);

//Helper function to render the bounding box to the screen
void
renderAABB(struct AABB *aabb, vec4 color);

void
renderDrawInstances(GLuint texture,struct InstanceData* instances, size_t count);

//
void
renderDrawQuadsInstanced(struct Quad* quads, size_t count);
 
//render all bounding boxes using renderAABB()
void
drawAllAABB(void);

void
renderShutdown(void);
#endif
