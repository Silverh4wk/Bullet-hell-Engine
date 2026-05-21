#ifndef PHYSICS_H
#define PHYSICS_H

#include "linmath.h"
#include "array_list.h"
#include "AABB.h"
#include "../types.h"
struct Body;

// collision callback type
typedef void (*CollisionCallback)(struct Body* self, struct Body* other);


typedef enum {
  BODY_PLAYER,
  BODY_ENEMY,
  BODY_SPAWNER,
  BODY_ITEM,
  BODY_BULLET,
} Type;


// struct that hold the physical "body" or state of an object
struct Body
{
    AABB aabb;
    size_t hp;
    vec2 velocity;
    vec2 acceleration;
    CollisionCallback onCollision; // a function pointer
    int  active;
    real32 lifetime;
    Type type;
    struct Shape* sptr; //points back to the shape that its holdoing i guess (temp fix)
    Entity entity;
    char* group ;
    
};

void
physicsInit(void);

void
physicsUpdate(void);

size_t
physicsBodyCreate(struct Shape* sptr,vec2 pos, vec2 size,Type t);

size_t
physicsGetBodyCount(void);

struct Body*
physicsGetBody(size_t i);

void
physicsToggleAllHitBoxes(void);

void
physicsBodyDestroy(size_t index);

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
