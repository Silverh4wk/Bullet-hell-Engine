#pragma once

#include <linmath.h>
#include "../types.h"


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
} Quad;


Quad_Result QuadCreate(Quad *quad,real32 width, real32 height, real32 x, real32 y,
                      real32 c1, real32 c2, real32 c3, real32 a) ;

void QuadMove(Quad *quad, real32 posx, real32 posy);
void QuadChangeSize(Quad *quad, real32 posx, real32 posy);
void QuadChangeColor(Quad *quad, real32 c1, real32 c2, real32 c3, real32 c4) ;
