#pragma once
#include "linmath.h"
#include "../types.h"
//will be using basic aabb test
typedef struct AABB
{
    vec2 position;
    vec2 half_size;
    vec4 color;
}AABB;

typedef struct Body
{
    AABB aabb;
    vec2 velocity;
    vec2 acceleration;
    
} Body;

void physicsInit(void);
void physicsUpdate(void);
size_t physicsBodyCreate(vec2 pos, vec2 size, vec4 color);
Body *physicsBodyGet(size_t index);
