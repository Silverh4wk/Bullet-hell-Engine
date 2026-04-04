#ifndef PHYSICS_H
#define PHYSICS_H

#include "aabb.h"
#include "array_list.h"


struct Body;

// collision callback type
typedef void (*CollisionCallback)(struct Body* self, struct Body* other);

// struct that hold the physical "body" or state of an object

typedef enum {
  BODY_PLAYER,
  BODY_BULLET,
} type;

struct Body
{
    AABB aabb;
    size_t hp;
    vec2 velocity;
    vec2 acceleration;
    CollisionCallback onCollision; // a function pointer
    int  active;
    type type;
};

void
physicsInit(void);

void
physicsUpdate(void);

size_t
physicsBodyCreate(vec2 pos, vec2 size,type t);

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
narrowPhaseResolve(struct Body *a, struct Array_List *candidates);

void
broadPhaseResolve(void);


void
physicsRemoveInactiveBodies(void);

void
physicsClearBodies(void);

#endif
