#ifndef ENTITY_H
#define ENTITY_H

#include "../types.h"
#include "component.h"
#include "physics.h"
#include "../objects/shapes.h"

//creates a new game entity
Entity
entityCreate(Type t);


// destroy a game entity,
//1:  no entities to destroy
//-1: entered ID is outside of MAX_ENTITES range 
int
entityDestroy( Entity entity );

// return the entity transform data
struct Transform*
entityGetTransform( Entity entity );

void
entitySetTransform( Entity entity, vec2* pos, real32* angle );

void
collisionSetHitboxBox( Entity entity, int width, int height );

void
collisionSetHitboxCircle( Entity entity, int radius  );

void
collisionGroupAddToGrp( Entity entity,  char* grp );

#endif
