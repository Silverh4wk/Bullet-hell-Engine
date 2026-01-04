#pragma once

#include <linmath.h>
#include "../types.h"
#include "../engine/physics.h"

typedef enum Quad_Result{
    QUAD_OK = 0,
    QUAD_ERR_INVALID_SIZE,
    QUAD_ERR_INVALID_COLOR,
    QUAD_ERR_INTERNAL
} Quad_Result;


typedef struct Quad {
    vec2 pos;
    vec2 size;
    vec4 color;
    Body* body;
} Quad;


Quad_Result QuadCreate(Quad *quad, real32 width, real32 height, real32 x,
                       real32 y, real32 c1, real32 c2, real32 c3, real32 a);

// if want modified hitbox
Quad_Result QuadCreate(Quad *quad,real32 width, real32 height, real32 x, real32 y,
		       real32 c1, real32 c2, real32 c3, real32 a,size_t bodyIndex) ;

Quad_Result QuadCreate(Quad *quad ,vec2 size, vec2 pos,vec4 colors) ;
    
void QuadMove(Quad *quad, real32 posx, real32 posy);

void QuadSetSize(Quad *quad, real32 posx, real32 posy);

void QuadSetHitBoxSize(Quad *quad, real32 posx, real32 posy);

void QuadSetColor(Quad *quad, real32 c1, real32 c2, real32 c3, real32 c4) ;

