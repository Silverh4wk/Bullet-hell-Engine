
#include "shapes.h"
#include "../helpers.h"

//(todo:Hazim) create basic functions for the created quad
// stuff like move, change size etc etc (do more research on this also)
// wait i see that is already happening inside of setvec2 and so functions, i
// just need to seperate those and rename them properly.


Quad_Result QuadCreate(Quad *quad ,real32 width, real32 height, real32 x, real32 y,
                      real32 c1, real32 c2, real32 c3, real32 a) {

    if (width <= 0.0f || height <= 0.0f) {
        return QUAD_ERR_INVALID_SIZE;
    }

    if (!isfinite(x) || !isfinite(y) ||
        !isfinite(width) || !isfinite(height) ||
        !isfinite(c1) || !isfinite(c2) || !isfinite(c3) || !isfinite(a)) {
        return QUAD_ERR_INVALID_COLOR;
    }

    if (!quad) return QUAD_ERR_INVALID_SIZE;
    
    vec2 pos, size;
    vec4 color;
    
    setVec2(&pos, width, height) ;
    setVec2(&size, x, y); 
    setVec4(&color, c1, c2, c3, a);
    setQuad(quad, pos, size, color);
    return QUAD_OK;
}


void QuadMove(Quad* quad, real32 posx, real32 posy)
    {
	    setVec2(&quad->pos, posx, posy) ;

    }
void QuadChangeSize(Quad* quad, real32 width, real32 height)
{
    setVec2(&quad->size,width,height);    
}

void QuadChangeColor(Quad *quad, real32 c1, real32 c2, real32 c3, real32 c4) {
    setVec4(&quad->color,c1,c2,c3,c4);    
}   
