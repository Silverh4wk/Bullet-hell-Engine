#pragma once
#include "linmath.h"
#include<stdlib.h>
#include "../types.h"
//will be using basic aabb test
typedef struct AABB
{
    vec2 position;
    vec2 center;
    vec2  radius;
    vec2 half_size;
    int toggle;
}AABB;

typedef struct Body
{
    AABB aabb;
    vec2 velocity;
    vec2 acceleration;
    vec4 color;
    
} Body;

void physicsInit(void);

void physicsUpdate(void);

size_t physicsBodyCreate(vec2 pos, vec2 size, vec4 color);

Body *physicsBodyGet(size_t index);
size_t physicsGetBodyCount(void);
Body* physicsGetBody(size_t i);

int testAABBAABB(AABB a, AABB b);

void physicsToggleAllHitBoxes(void);
