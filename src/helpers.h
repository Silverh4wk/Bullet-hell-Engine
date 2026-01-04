#pragma once

#include <stdio.h>
#include <linmath.h>
#include "types.h"


typedef struct Quad Quad;

int32 pseudoRand(int32 n);
void setVec2(vec2 *a, real32 x, real32 y);
void setVec3(vec3 *a, real32 x, real32 y, real32 z);
void setVec4(vec4 *a, real32 x, real32 y, real32 z, real32 l);
void setQuad(Quad *quad ,vec2 pos, vec2 size, vec4 color);

#define global_variable static


#define Pi32 3.14159265359f
#define ArrayCount(Array) (sizeof(Array)/sizeof(Array[0]))
#define KiloBytes(Value) (Value*1024)
#define MegaBytes(Value) (KiloBytes(Value)*1024)
#define GigaBytes(Value) (MegaBytes(Value)*1024)
#define TeraBytes(Value) (GigaBytes(Value)*1024)





#define ERROR_EXIT(...) { fprintf(stderr, __VA_ARGS__); exit(1);}
#define ERROR_RETURN(R, ...) { fprintf(stderr, __VA_ARGS__); return R;}



