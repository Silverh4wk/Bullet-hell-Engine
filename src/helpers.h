#ifndef HELPERS_H
#define HELPERS_H

#include <linmath.h>
#include <stdio.h>
#include "types.h"



struct Shape;

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
int32  pseudoRand(int32 n);
real32 lerp(real32 start_angle, real32 end_angle, real32 t);
real32 degreeToRadians(real32 angle);

void setVec2(vec2 *a, real32 x, real32 y);
void setVec3(vec3 *a, real32 x, real32 y, real32 z);
void setVec4(vec4 *a, real32 x, real32 y, real32 z, real32 l);
void setQuad(struct Shape *quad ,vec2 pos, vec2 size, vec4 *color);
void setCircle(struct Shape *circle, vec2 pos, real32 radius, vec4 *color);

#define global_variable static


#define Pi32 3.14159265359f
#define ArrayCount(Array) (sizeof(Array)/sizeof(Array[0]))
#define KiloBytes(Value) (Value*1024)
#define MegaBytes(Value) (KiloBytes(Value)*1024)
#define GigaBytes(Value) (MegaBytes(Value)*1024)
#define TeraBytes(Value) (GigaBytes(Value)*1024)




#define ERROR_EXIT(...) { fprintf(stderr, __VA_ARGS__); exit(1);}
#define ERROR_RETURN(R, ...) { fprintf(stderr, __VA_ARGS__); return R;}



#endif
