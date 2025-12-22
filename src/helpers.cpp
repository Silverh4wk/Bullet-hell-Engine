#include <random>
#include "helpers.h"
#include "objects/shapes.h"

//Returns a random value in the range of [0 .. n-1]
int32 pseudoRand(int32 n)
    {
       int32 result= 0 ;
       std::random_device rd;
       std::mt19937 gen(rd());
       std::uniform_int_distribution<> distrib(0, n-1);
       result = distrib(rd);  
       return result;
    };


void setVec2(vec2 *a, real32 x, real32 y) {
     (*a)[0] = x;
     (*a)[1] = y;
    };


void setVec3(vec3 *a, real32 x, real32 y, real32 z) {
     (*a)[0] = x;
     (*a)[1] = y;
     (*a)[2] = z;
    };

void setVec4(vec4*a, real32 x, real32 y, real32 z, real32 l) {
    (*a)[0] = x;
    (*a)[1] = y;
    (*a)[2] = z;
    (*a)[3] = l;
    };
        
void setQuad(Quad *quad, vec2 pos, vec2 size, vec4 color)
{
    setVec2(&quad->pos, pos[0], pos[1]);
    setVec2(&quad->size, size[0], size[1]);
    setVec4(&quad->color, color[0],color[1],color[2],color[3]);
};
    
