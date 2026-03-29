#ifndef PHYSICS_H
#define PHYSICS_H

#include "aabb.h"



// struct that hold the physical "body" or state of an object

struct Body
{
    AABB aabb;
    size_t hp;
    vec2 velocity;
    vec2 acceleration;
};

void
physicsInit(void);

void
physicsUpdate(void);

size_t
physicsBodyCreate(vec2 pos, vec2 size);

struct Body *
physicsBodyGet(size_t index);

size_t
physicsGetBodyCount(void);

struct Body*
physicsGetBody(size_t i);

int
testAABBAABB(AABB* a, AABB* b);

void
physicsToggleAllHitBoxes(void);

void
physicsBodyDestroyByIndex(size_t index);

void
physicsBodyDestroyByPtr(struct Body *body);

void
physicsClearBodies(void);

#endif
