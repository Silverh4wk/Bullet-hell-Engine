#ifndef COMPONENT_H
#define COMPONENT_H

#include "../types.h"
#include <linmath.h>

typedef enum {
    COMPONENT_TRANSFORM,
    COMPONENT_SPRITE,
    COMPONENT_BULLET_SPAWNER,
    COMPONENT_SHAPE, 
    /* COMPONENT_PLAYER_CONTROLLER, */
    /* COMPONENT_ENEMY_AI, */
    COMPONENT_COUNT
} ComponentTypeEnum;



struct Transform {
    vec2 position;
    vec2 size;
    real32 rotation;  // is in rads
} ;

struct Sprite {
    vec4 color;
    int layer;           // this can be optional for the draw order since we didnt enable the depth buffer
    uint32 texture_id;   
} ;

struct BulletSpawner {
    real32 fire_Rate;      
    real32 cooldown;      
    uint32 pattern_id;    
} ;

struct ShapeComponent {
    struct Shape* shape; 
};


// attachs a component type to the entity
// components can be :
// sprite
// bullet spawner
// collider, atleast for now
// return -1 if out of MAX_ENTITIES range
// return 1 if entities are empty
// return 2 if not in the component enum, aka this component doesnt exist 
// return -2 if component already attached
// return -3 if the passed component param is not handled, default case 
// return 0 if successful
int ComponentAttach(Entity entity, ComponentType type, void *data);


// remove a component from the entity
// return -1 if out of MAX_ENTITIES range
// return -2 if component wasnt attached
// return 1 if entitiesa are empty
int
ComponentDetach( Entity entity, ComponentType type );

// get the data of the component that you want
void*
ComponentGetData( Entity entity, ComponentType type );



#endif

