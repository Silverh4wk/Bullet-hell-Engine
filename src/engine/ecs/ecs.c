#include "../entity.h"
#include "../ecs_internal.h"
#include "../component.h"
#include "../physics.h"

struct Transform      g_transforms[MAX_ENTITIES];
struct Sprite         g_sprites[MAX_ENTITIES];
struct BulletSpawner  g_spawners[MAX_ENTITIES];
uint32                g_component_mask[MAX_ENTITIES];
Entity                g_next_free = 0;
struct ShapeComponent g_shapes[MAX_ENTITIES];
int32                 g_body_indices[MAX_ENTITIES];

Entity
entityCreate( Type t )
{
    if ( g_next_free >= MAX_ENTITIES ) {
        return 0; //log this laters
    }
    Entity entity = g_next_free++;
    g_component_mask[entity] = 0;

    //assigning vals 
    g_component_mask[entity] = 0;
    g_body_indices[entity] = -1; // no body attached yet
    g_transforms[entity].position[0] = 0.0f;
    g_transforms[entity].position[1] = 0.0f;
    g_transforms[entity].rotation = 0.0f;
    
    g_transforms[entity].scale[0] = 1.0f;
    g_transforms[entity].scale[1] = 1.0f;

    g_component_mask[entity] |= ( 1 << COMPONENT_TRANSFORM );

    return entity;
}

int entityDestroy( Entity entity ) {
    if (entity == 0 )
	return 1;
    if( entity >= MAX_ENTITIES )
	return -1;
    if (g_body_indices[entity] != -1)
    {
	physicsBodyDestroy(g_body_indices[entity]);
        g_body_indices[entity] = -1;
    }

    g_component_mask[entity] = 0;
    return 0;
}

struct Transform*
entityGetTransform( Entity entity ) {
    if (entity == 0 || entity >= MAX_ENTITIES) return NULL;
    return &g_transforms[entity];
}


static void
ensurePhysicsBody( Entity entity ) {
    if (g_body_indices[entity] != -1) return;  

    struct Transform* transform = &g_transforms[entity];
    vec2 pos = { transform->position[0], transform->position[1] };
    vec2 size = { 32.0f, 32.0f };  // if no size was found

    // if the entity has a ShapeComponent, use its dimensions.
    if (g_component_mask[entity] & (1 << COMPONENT_SHAPE)) {
        struct Shape* shape = g_shapes[entity].shape;
        if (shape) {
            if (shape->type == SHAPE_QUAD) {
                size[0] = shape->data.quad.size[0];
                size[1] = shape->data.quad.size[1];
            } else if (shape->type == SHAPE_CIRCLE) {
                float r = shape->data.circle.radius;
                size[0] = size[1] = r * 2.0f;
            }
        }
    }

    size_t bodyIdx = physicsBodyCreate(NULL, pos, size, 0);
    g_body_indices[entity] = (int32)bodyIdx;

    // Store a back-reference from the physics body to the entity
    struct Body* body = physicsBodyGet(bodyIdx);
    body->entity = entity;
}

void
entitySetTransform(Entity entity, vec2* pos, real32* angle) {
    // a temp pointer to get the entity data
    struct Transform* transform = entityGetTransform(entity);
    if (!transform) return;
   //override that data with 
    
    if( pos != NULL )
    {
	transform->position[0] = ( *pos )[0];
	transform->position[1] = ( *pos )[1];
    }

    if ( angle!= NULL )
	transform->rotation = *angle;
    
    ensurePhysicsBody(entity);
    
    int32 bodyIdx = g_body_indices[entity];
    struct Body* body = NULL;
    if (bodyIdx != -1) {
	body = physicsBodyGet(bodyIdx);
        body->aabb.coords[0] = transform->position[0];
        body->aabb.coords[1] = transform->position[1];
    }

    if (!(g_component_mask[entity] & (1 << COMPONENT_SHAPE))) {
        vec4 teal = {0,1,1,1};
        struct ShapeUnion su = shapeQuadCreate(
            (vec2){transform->position[0], transform->position[1]},
            (vec2){32,32},
            &teal,
            0,
            false
        );
        su.shape->body = body;
        struct ShapeComponent sc = { .shape = su.shape };
        ComponentAttach(entity, COMPONENT_SHAPE, &sc);
    }
}

void
collisionSetHitboxBox( Entity entity, int width, int height ) {
    if ( entity == 0 || entity >= MAX_ENTITIES ) return;
    ensurePhysicsBody( entity );
    int32 bodyIdx = g_body_indices[entity];
    if ( bodyIdx == -1 ) return;
    
    struct Body* body = physicsBodyGet(bodyIdx);
    body->aabb.dims[0] = width / 2.0f;
    body->aabb.dims[1] = height / 2.0f;
}

//reminder to add a circle hitbox
void
collisionSetHitboxCircle( Entity entity, int radius ) {
    if ( entity == 0 || entity >= MAX_ENTITIES ) return;
    ensurePhysicsBody(entity);
    int32 bodyIdx = g_body_indices[entity];
    if ( bodyIdx == -1 ) return;

    struct Body* body = physicsBodyGet(bodyIdx);
    body->aabb.dims[0] = radius;
    body->aabb.dims[1] = radius;
}

void
collisionGroupAddToGrp( Entity entity, char *grp )
{
    if ( entity == 0 || entity >= MAX_ENTITIES ) return;
    int32 bodyIdx = g_body_indices[entity];
    if ( bodyIdx == -1 ) return;

    struct Body * body = physicsGetBody( bodyIdx );
    body->group = grp;
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
