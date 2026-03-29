#include "shapes.h"

#include <stdlib.h>
#include "../engine/physics.h"
#include "../helpers.h"


struct QuadUnion
QuadCreate (vec2 pos, vec2 size , vec4* color, bool t_physics)
 {
     // if color is NULL
     vec4 c;
     
     if(color == NULL)
     {
	 //#B561ED 
	 setVec4(&c, 181, 97, 237, 1.0f);
	 color =&c ;
     }

     struct QuadUnion r ;
     r.quad = NULL;

     //check if param are ok
     //alloc quad
     //set quad data
     //return result


    if (size[0] <= 0.0f || size[1] <= 0.0f) {
	r.result = QUAD_ERR_INVALID_SIZE; 
	return  r;
    }

    // just a double check if anything is out of the ordinary
    if (!isfinite(size[0]) || !isfinite(size[1]) ||
        !isfinite(pos[0]) || !isfinite(pos[1]) ||
        !isfinite(*color[0]) || !isfinite(*color[1]) || !isfinite(*color[2]) || !isfinite(*color[3])) {
	r.result = QUAD_ERR_INVALID_COLOR; 
	return r ;
    }

    struct Quad *quad = malloc(sizeof(struct Quad));
    
    // Fill hitbox stuff
    // Add to the body list
    // Return its index

    //if physics body is set to true
    if(t_physics == true)
    {
	size_t idx = physicsBodyCreate(pos, size);
	quad->body = physicsBodyGet(idx);
    }
    
    // Fill quad vars
    setQuad(quad, pos, size, color);
    
    if (!quad){
	r.result = QUAD_ERR_INTERNAL; 
	return r ;}

    r.quad = quad;
    return r;
 }




void QuadMove(struct Quad* quad, real32 posx, real32 posy)
{
    setVec2(&quad->pos, posx, posy);

    if (quad->physics_enabled == true)
	setVec2(&quad->body->aabb.coords, posx, posy);
    
}

void QuadSetSize(struct Quad* quad, real32 width, real32 height)
{
    setVec2(&quad->size,width,height);    
}

void QuadSetColor(struct Quad *quad, real32 c1, real32 c2, real32 c3, real32 c4) {
    setVec4(&quad->color,c1,c2,c3,c4);    
}

void QuadDelete(struct Quad* quad)
{
    
     if (!quad) {
	 return ;}

    if (quad->body)
    {
        physicsBodyDestroyByPtr(quad->body);
        quad->body = NULL;
    }

    return ;
}
