#include <stdlib.h>

#include "shapes.h"
#include "../helpers.h"
#include "../engine/pool_allocator.h"

static struct PoolAllocator shape_pool;


void
initShapesPool(void)
    {
	initPool(&shape_pool, sizeof(struct Shape), 256);
    }

struct ShapeUnion
shapeQuadCreate(vec2 pos, vec2 size , vec4* color, Type t,bool t_physics)
 {
     // if color is NULL
     vec4 c;
     
     if(color == NULL)
     {
	 //#B561ED 
	 setVec4(&c, 181, 97, 237, 1.0f);
	 color =&c ;
     }

     struct ShapeUnion r ;
     r.shape = NULL;

     //check if param are ok
     //alloc quad
     //set quad data
     //return result


    if (size[0] <= 0.0f || size[1] <= 0.0f) {
	r.result = SHAPE_ERR_INVALID_SIZE; 
	ERROR_RETURN(r, "Failed to create shape, SHAPE_ERROR: SHAPE_ERR_INVALID_SIZE %d \n", r.result)
    }

    // just a double check if anything is out of the ordinary
    if (!isfinite(size[0]) || !isfinite(size[1]) ||
        !isfinite(pos[0]) || !isfinite(pos[1]) ||
         !isfinite(*color[0]) || !isfinite(*color[1]) || !isfinite(*color[2]) || !isfinite(*color[3])) {
	r.result = SHAPE_ERR_INVALID_COLOR; 
	ERROR_RETURN(r, "Failed to create shape, SHAPE_ERROR: SHAPE_ERR_INVALID_COLOR %d \n", r.result)
    }

    struct Shape *quad = allocatePool(&shape_pool);
    quad->shape_type = SHAPE_QUAD;
    quad->physics_enabled = t_physics;
    quad->type = t;
    // Fill hitbox stuff
    // Add to the body list
    // Return its index

    //if physics body is set to true
    //create physics body and attach it to the quad
    if(t_physics == true)
    {
	shapeAddPhysics(quad);
    }
    
    // Fill quad vars
    setQuad(quad, pos, size, color);
    quad->data.quad.rotation_angle = 0.0f;
    
    if (!quad){
	r.result = SHAPE_ERR_INTERNAL; 
	ERROR_RETURN(r, "Failed to create shape, SHAPE_ERROR: SHAPE_ERR_INTERNAL %d \n", r.result)}

    r.result = SHAPE_OK;
    r.shape = quad;
    return r;
 }


struct ShapeUnion
shapeCircleCreate(vec2 pos, real32 radius, vec4* color, Type t,bool t_physics)
 {
     // if color is NULL
     vec4 c;
     
     if(color == NULL)
     {
	 //#B561ED 
	 setVec4(&c, 181, 97, 237, 1.0f);
	 color =&c ;
     }

     struct ShapeUnion r ;
     r.shape = NULL;

     //check if param are ok
     //alloc quad
     //set quad data
     //return result


    if (radius <= 0.0f) {
	r.result = SHAPE_ERR_INVALID_SIZE; 
	ERROR_RETURN(r, "Failed to create shape, SHAPE_ERROR: SHAPE_ERR_INVALID_SIZE %d \n", r.result)
    }

    // just a double check if anything is out of the ordinary
    if (!isfinite(radius) ||
        !isfinite(pos[0]) || !isfinite(pos[1]) ||
         !isfinite(*color[0]) || !isfinite(*color[1]) || !isfinite(*color[2]) || !isfinite(*color[3])) {
	r.result = SHAPE_ERR_INVALID_COLOR; 
	ERROR_RETURN(r, "Failed to create shape, SHAPE_ERROR: SHAPE_ERR_INVALID_COLOR %d \n", r.result)
    }

    struct Shape *circle = allocatePool(&shape_pool);
    circle->shape_type = SHAPE_CIRCLE;
    circle->physics_enabled = t_physics;
    circle->type = t;
    
    // Fill hitbox stuff
    // Add to the body list
    // Return its index

    //if physics body is set to true
    //create physics body and attach it to the quad
    if(t_physics == true)
    {
	shapeAddPhysics(circle);
    }
    
    // Fill quad vars
    setCircle(circle,pos, radius, color);
    if (!circle){
	r.result = SHAPE_ERR_INTERNAL; 
	ERROR_RETURN(r, "Failed to create shape, SHAPE_ERROR: SHAPE_ERR_INTERNAL %d \n", r.result)}

    r.result = SHAPE_OK;
    r.shape = circle;
    return r;
 }

void
shapeAddPhysics(struct Shape *shape)
{
    if(!shape)
	ERROR_RETURN(,"Failed to enable physics, shape returned NULL\n SHAPE_ERROR:SHAPE_ERR_INTERNAL\n");

    if(shape->shape_type == SHAPE_QUAD)
    {
	size_t idx = physicsBodyCreate(shape,shape->pos,shape->data.quad.size,shape->type);
	shape->body = physicsBodyGet(idx);
    }
    else if (shape->shape_type == SHAPE_CIRCLE)
    {
	vec2 radius = {shape->data.circle.radius,shape->data.circle.radius};
	size_t idx = physicsBodyCreate(shape,shape->pos,radius,shape->type);
	shape->body = physicsBodyGet(idx);
    }
}


void shapeMove(struct Shape* quad, real32 posx, real32 posy)
{
    setVec2(&quad->pos, posx, posy);

    if (quad->physics_enabled == true)
	setVec2(&quad->body->aabb.coords, posx, posy);
    
}

void QuadSetSize(struct Shape* quad, real32 width, real32 height)
{
    setVec2(&quad->data.quad.size,width,height);    
}

void QuadSetColor(struct Shape *quad, real32 c1, real32 c2, real32 c3, real32 c4) {
    setVec4(&quad->color,c1,c2,c3,c4);    
}

void QuadDelete(struct Shape* quad)
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
