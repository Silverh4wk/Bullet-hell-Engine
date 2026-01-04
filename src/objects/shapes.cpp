
#include "shapes.h"
#include "../helpers.h"



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

    size_t idx = physicsBodyCreate(pos, size, color);
    quad->body = physicsBodyGet(idx);

    setVec2(&pos, width, height) ;
    setVec2(&size, x, y); 
    setVec4(&color, c1, c2, c3, a);
    setQuad(quad, pos, size, color);
    return QUAD_OK;
}

Quad_Result QuadCreate(Quad *quad ,vec2 size, vec2 pos,
                      vec4 colors) {

    if (size[0] <= 0.0f || size[1] <= 0.0f) {
        return QUAD_ERR_INVALID_SIZE;
    }

    if (!isfinite(pos[0]) || !isfinite(pos[1]) ||
        !isfinite(size[0]) || !isfinite(pos[1]) ||
        !isfinite(colors[0]) || !isfinite(pos[1]) || !isfinite(pos[0]) || !isfinite(pos[3])) {
        return QUAD_ERR_INVALID_COLOR;
    }
    Body* body;
    size_t idx = physicsBodyCreate(pos, size, colors);
    body = physicsBodyGet(idx);
    quad->body = body;
    setQuad(quad, pos, size, colors);

    if (!quad) return QUAD_ERR_INVALID_SIZE;
    
    
    
    return QUAD_OK;
}


//custom hitbox for this
Quad_Result QuadCreate(Quad *quad ,real32 width, real32 height, real32 x, real32 y,
		       real32 c1, real32 c2, real32 c3, real32 a, size_t bodyIndex) {

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

    size_t idx = physicsBodyCreate(pos, size, color);
    quad->body = physicsBodyGet(idx);

    setVec2(&pos, width, height) ;
    setVec2(&size, x, y); 
    setVec4(&color, c1, c2, c3, a);
    setQuad(quad, pos, size, color);
    return QUAD_OK;
}

void QuadMove(Quad* quad, real32 posx, real32 posy)
    {
	    setVec2(&quad->pos, posx, posy);
	    setVec2(&quad->body->aabb.position, posx, posy);

    }

void QuadSetSize(Quad* quad, real32 width, real32 height)
{
    setVec2(&quad->size,width,height);    
}

void QuadSetColor(Quad *quad, real32 c1, real32 c2, real32 c3, real32 c4) {
    setVec4(&quad->color,c1,c2,c3,c4);    
}


