#include "../entity.h"
#include "../ecs_internal.h"
#include "../component.h"

struct Transform      g_transforms[MAX_ENTITIES];
struct Sprite         g_sprites[MAX_ENTITIES];
struct BulletSpawner  g_spawners[MAX_ENTITIES];
uint32                g_component_mask[MAX_ENTITIES];
Entity                g_next_free = 1;
struct ShapeComponent g_shapes[MAX_ENTITIES];

Entity
EntityCreate(ShapeType shape,vec2* pos, vec2* size,vec4* color, real32* angle,bool32 p_enabled , Type t)
{
    if (g_next_free >= MAX_ENTITIES) {
        return 0; //log this laters
    }
    Entity entity = g_next_free++;
    g_component_mask[entity] = 0;

    
    if (pos != NULL) {
        g_transforms[entity].position[0] = (*pos)[0];
        g_transforms[entity].position[1] = (*pos)[1];
    } else {
        g_transforms[entity].position[0] = 0.0f;
        g_transforms[entity].position[1] = 0.0f;
    }
    if (angle != NULL) {
        g_transforms[entity].rotation = *angle;
    } else {
        g_transforms[entity].rotation = 0.0f;
    }

    g_transforms[entity].scale[0] = 1.0f;
    g_transforms[entity].scale[1] = 1.0f;

    g_component_mask[entity] |= (1 << COMPONENT_TRANSFORM);
    
    // create shape based on type, so quad or circle
    vec4 defaultColor = {1,1,1,1};
    vec4* col = color ? color : &defaultColor;
    vec2 defaultPos = {0,0};
    vec2* p = pos ? pos : &defaultPos;

    struct Shape* sh = NULL;
    struct ShapeUnion result;
    
    if (shape == SHAPE_QUAD) {
        vec2 s = {32, 32};      // default size
        if (size != NULL) {
            result = shapeQuadCreate(*p, *size, col, t, p_enabled);
        } else {
            result = shapeQuadCreate(*p, s, col, t, p_enabled);
        }
        if (result.result != SHAPE_OK) return 0;
        sh = result.shape;
    } else if (shape == SHAPE_CIRCLE) {
        real32 r = 16.0f;
        if (size != NULL) {
            /* use first component of provided size as radius */
            result = shapeCircleCreate(*p, (*size)[0], col, t, p_enabled);
        } else {
            result = shapeCircleCreate(*p, r, col, t, p_enabled);
        }
        if (result.result != SHAPE_OK) return 0;
        sh = result.shape;
    }

    if (!sh)
	return 0;

    struct ShapeComponent sc = { .shape = sh };
    ComponentAttach(entity, COMPONENT_SHAPE, &sc);
    
    return entity;
}

int EntityDestroy(Entity entity) {
    if (entity == 0 )
	return 1;
    if( entity >= MAX_ENTITIES )
	return -1;
    g_component_mask[entity] = 0;
    return 0;
}

struct Transform*
EntityGet(Entity entity) {
    if (entity == 0 || entity >= MAX_ENTITIES) return NULL;
    return &g_transforms[entity];
}

void
EntitySetTransform(Entity entity, vec2* pos, real32* angle) {
    // a temp pointer to get the entity data
    struct Transform* transform = EntityGet(entity);
    //override that data with 
    if (transform) {
	if(pos != NULL)
	{
	    transform->position[0] = (*pos)[0];
	    transform->position[1] = (*pos)[1];
	}
	if (angle!= NULL)
	    transform->rotation = *angle;
    }
}


int
ComponentAttach(Entity entity, ComponentType type, void* data) {
    if ( entity >= MAX_ENTITIES ) return -1;
    if ( entity == 0 )            return 1;
    if ( type >= COMPONENT_COUNT )  return 2;

    // check if component is already already attached, except for spawner  
    // shift the number 1 by the type position,
    // then AND to check that type of component is attached or not
    if (g_component_mask[entity] & (1 << type)) return -2;

    switch (type) {
        case COMPONENT_SPRITE:
            g_sprites[entity] = *(struct Sprite*)data;
            break;
        case COMPONENT_BULLET_SPAWNER:
            g_spawners[entity] = *(struct BulletSpawner*)data;
            break;
        case COMPONENT_SHAPE:
	    g_shapes[entity] = *(struct ShapeComponent*)data;
	    break;
        default:
            return -3;
    }
    //a quicker way to answer questions later
    // each bit represent a component type
    g_component_mask[entity] |= (1 << type);
    return 0;
}

int
ComponentDetach( Entity entity, ComponentType type ) {
    if ( entity >= MAX_ENTITIES ) return -1;
    if ( entity == 0 ) return 1;
    uint32_t flag = (1 << type);
    //AND to see if the component is attached or not
    if (!(g_component_mask[entity] & flag)) {
        return -2; 
    }
    
    g_component_mask[entity] &= ~(1 << type); // remove the component thats attached 
    return 0;
}

void*
ComponentGetData(Entity entity, ComponentType type) {
    if ( entity == 0 || entity >= MAX_ENTITIES ) return NULL;
    if ( ! ( g_component_mask[entity] & ( 1 << type ) ) ) return NULL;
    
    switch (type) {
    case COMPONENT_TRANSFORM: return &g_transforms[entity];
    case COMPONENT_SPRITE:    return &g_sprites[entity];
    case COMPONENT_BULLET_SPAWNER: return &g_spawners[entity];
    case COMPONENT_SHAPE: return &g_shapes[entity];
    default: return NULL;
    }
}
