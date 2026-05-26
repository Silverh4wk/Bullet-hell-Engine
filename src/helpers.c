#include "helpers.h"
#include <SOIL/SOIL.h>
#include<SDL3/SDL_log.h>
#include "objects/shapes.h"


// just  quick functions to set vec2/3/4 variables
// not really necessary just something i did when i was bored and lazy (apparently, i never thought of using memcpy)
void
setVec2(vec2 *a, real32 x, real32 y) {
     (*a)[0] = x;
     (*a)[1] = y;
    };


void
setVec3(vec3 *a, real32 x, real32 y, real32 z) {
     (*a)[0] = x;
     (*a)[1] = y;
     (*a)[2] = z;
    };

void
setVec4(vec4*a, real32 x, real32 y, real32 z, real32 l) {
    (*a)[0] = x;
    (*a)[1] = y;
    (*a)[2] = z;
    (*a)[3] = l;
    };

//Set quad data 
void
setQuad(struct Shape *quad, vec2 pos, vec2 size, vec4 *color)
{
    setVec2(&quad->pos, pos[0], pos[1]);
    setVec2(&quad->data.quad.size, size[0], size[1]);
    setVec4(&quad->color,(*color)[0], (*color)[1], (*color)[2], (*color)[3]);
    quad->texture = 0;
};
    
//Set circle data 
void
setCircle(struct Shape *circle, vec2 pos, real32 radius, vec4 *color)
{
    setVec2(&circle->pos, pos[0], pos[1]);
    setVec4(&circle->color,(*color)[0], (*color)[1], (*color)[2], (*color)[3]);
    circle->data.circle.radius = radius;
    circle->texture = 0;
    };
