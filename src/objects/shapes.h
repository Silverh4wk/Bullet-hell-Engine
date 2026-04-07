#ifndef SHAPES_H
#define SHAPES_H

#include <linmath.h>
#include "../types.h"
#include <stdbool.h>
#include <Glad/glad.h>
#include "../engine/physics.h"

#define PI 3.14159
 
#define RotationMatrix(Angle) () 
struct Quad {
    vec2 pos;
    vec2 size;
    vec4 color;
    real32 rotation_angle;
    struct Body* body;
    GLuint texture;
    bool physics_enabled;
};


enum QuadResults{
    QUAD_OK,
    QUAD_ERR_INVALID_SIZE,
    QUAD_ERR_INVALID_COLOR,
    QUAD_ERR_INTERNAL
}  ;

void
initQuadPool(void);//initiate the object pool for quad objects

struct QuadUnion {
    enum  QuadResults result;
    //return a potiner to the allocated object if no issues
    struct Quad* quad;    
};

//allocoate memorycreates the quad with the given param
//position and size are needed but color can be NULL

struct QuadUnion
QuadCreate(vec2 pos, vec2 size , vec4* color, type t, bool t_physics);


//change the position of the quad  
void
QuadMove(struct Quad *quad, real32 posx, real32 posy);

//set the size of the quad
void
QuadSetSize(struct Quad *quad, real32 width, real32 height);

//set the hitbox size of the quad
void
QuadSetHitBoxSize(struct Quad *quad, real32 width, real32 height);

//set the color of the quad
void
QuadSetColor(struct Quad *quad, real32 c1, real32 c2, real32 c3, real32 c4) ;

//delete the quad and free the allocated memory
void
QuadDelete(struct Quad *quad);



struct Circle {
    vec2 pos;
    vec2 diam;
    vec4 radius;
    struct Body* body;
    GLuint texture;
    bool physics_enabled;
};


/* struct CircleUnion */
/* CircleCreate(pos,radius,color,physics){}     */
#endif
