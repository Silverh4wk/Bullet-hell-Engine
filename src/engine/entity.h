#ifndef ENTITY_H
#define ENTITY_H

#include "../types.h"
#include "component.h"
#include "physics.h"
#include "../objects/shapes.h"

//creates a new game entity
Entity
EntityCreate(ShapeType shape,vec2* pos, vec2* size , vec4* color, real32* angle,bool32 p_enabled , Type t);


// destroy a game entity,
//1:  no entities to destroy
//-1: entered ID is outside of MAX_ENTITES range 
int
EntityDestroy( Entity entity );


// return the entity transform data
struct Transform*
EntityGet( Entity entity );

void
EntitySetTransform( Entity entity, vec2* pos, real32* angle );


#endif
