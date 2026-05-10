#include "../entity.h"
#include "../ecs_internal.h"
#include "../component.h"
#include "../physics.h"

struct Transform          g_transforms[MAX_ENTITIES];
struct Sprite             g_sprites[MAX_ENTITIES];
struct BulletSpawner      g_spawners[MAX_ENTITIES];
struct ShapeComponent     g_shapes[MAX_ENTITIES];
struct PatternEntityState g_pattern_states[MAX_ENTITIES] = {0};
uint64                    g_component_mask[MAX_ENTITIES]; // bitmask to see the components an entity got 
Entity                    g_next_free = 0;
int                       g_type[MAX_ENTITIES];
int32                     g_body_indices[MAX_ENTITIES] = {0};

// Initialize g_body_indices to -1 (no body attached by default)
static void ecsInitArrays(void) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        g_body_indices[i] = -1;
    }
}

Entity
entityInit( Type t )
{
    // Initialize on first call
    static int initialized = 0;
    if (!initialized) {
        ecsInitArrays();
        initialized = 1;
    }

    if ( g_next_free >= MAX_ENTITIES ) {
        return 0; //log this laters
    }
    // add the entity to the array
    Entity entity = ++g_next_free;
    g_type[entity] = t;
    //assigning vals to zeroes
    g_component_mask[entity] = 0; //no components attached
    g_body_indices[entity] = -1; // no body attached
    
    g_transforms[entity].position[0] = 0.0f;
    g_transforms[entity].position[1] = 0.0f;
    g_transforms[entity].rotation    = 0.0f;
    g_transforms[entity].size[0]     = 0.0f; 
    g_transforms[entity].size[1]     = 0.0f;
    memset(&g_pattern_states[entity], 0, sizeof(struct PatternEntityState));
    
    
    //bitwise OR assignment
    //left shift by COMPONENT_TRANSFORM position
    //turning the bit on
    //just using this to track what components does the entity got attached
    ADD_COMPONENT(entity, COMPONENT_TRANSFORM);    
    return entity;
}

void entityBuild( Entity entity, ShapeType shape_t )
{
    struct Transform* transform = entityGetTransform( entity );
    if ( !transform ) return;
    vec2 pos = { transform->position[0], transform->position[1] };
    vec2 size = { transform->size[0], transform->size[1] };
    
    if ( !HAS_COMPONENT( entity, COMPONENT_SHAPE ) )
    {
        vec4 color = {0,1,1,1}; //default color

        struct ShapeUnion shape_union = {0};
	
	if( shape_t == SHAPE_QUAD )
	{
	    shape_union = shapeQuadCreate(pos,size,&color,0,false); // physics remain off unless u decide to add a physical body
	}
	else if ( shape_t == SHAPE_CIRCLE )
	{
	    shape_union = shapeCircleCreate(pos,size[0],&color,0,false); // physics remain off unless u decide to add a physical body
	}
	struct ShapeComponent shape_component = { .shape = shape_union.shape };
	ComponentAttach(entity, COMPONENT_SHAPE, &shape_component);
    }    
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
    //a body is already attached
    //skip this step
    if (g_body_indices[entity] != -1) return;
    //no shape, return
   if ( ! HAS_COMPONENT( entity, COMPONENT_SHAPE ) ) return;

    struct Transform* transform = &g_transforms[entity];
    vec2 pos = { transform->position[0], transform->position[1] };
    vec2 size = { transform->size[0], transform->size[1] };  // if no size was found
   
    //create a physics body and store its index number
    size_t bodyIdx = physicsBodyCreate(g_shapes[entity].shape, pos, size, 0);
    g_body_indices[entity] = (int32)bodyIdx;
    
    //store a back-reference from the physics body to the entity
    //this one is funny cuz just like *shape, u can loop infinitely between members
    struct Body* body = physicsGetBody(bodyIdx);
    body->entity = entity;
}

void
entitySetTransform(Entity entity, vec2* pos, vec2* size,real32* angle) {
    // a temp pointer to get the entity transform data
    struct Transform* transform = entityGetTransform(entity);
    if (!transform) return;

    //override that data with 
    if( pos != NULL )
    {
	transform->position[0] = ( *pos )[0];
	transform->position[1] = ( *pos )[1];
    }

     if( size != NULL )
    {
	transform->size[0] = ( *size )[0];
	transform->size[1] = ( *size )[1];
    }
     
    if ( angle!= NULL )
	transform->rotation = *angle;
}

void
entityAddPhysics( Entity entity )
{
    // a temp pointer to get the entity transform data
    struct Transform* transform = entityGetTransform( entity );
    if ( !transform ) return;
    
    //make sure it got a physical body attached to it
    ensurePhysicsBody( entity );

    int32 bodyIdx = g_body_indices[ entity ];
    struct Body* body = NULL;
    //get the body then copy the transform attribs
    if ( bodyIdx != -1 )
    {
	body = physicsGetBody( bodyIdx );
	body->aabb.coords[ 0 ] = transform->position[ 0 ];
	body->aabb.coords[ 1 ] = transform->position[ 1 ];
	body->aabb.dims  [ 0 ] = transform->size    [ 0 ];
	body->aabb.dims  [ 1 ] = transform->size    [ 1 ];
	body->active = true; //enable physics by default
    }
    
};

void
entitySetColor(Entity entity, vec4 color)
{
    if ( entity == 0 || entity >= MAX_ENTITIES ) return;
    
    g_shapes[entity].shape->color[0] = color[0];
    g_shapes[entity].shape->color[1] = color[1];
    g_shapes[entity].shape->color[2] = color[2];
    g_shapes[entity].shape->color[3] = color[3];
}

void
entitySetTexture(Entity entity, GLuint tex) {
    if (entity == 0 || entity >= MAX_ENTITIES) return;
    if (!HAS_COMPONENT(entity, COMPONENT_SHAPE)) return;
    g_shapes[entity].shape->texture = tex;
}

void
collisionSetHitboxBox( Entity entity, int width, int height ) {
    if ( entity == 0 || entity >= MAX_ENTITIES ) return;
    ensurePhysicsBody( entity );
    int32 bodyIdx = g_body_indices[entity];
    if ( bodyIdx == -1 ) return;
    
    struct Body* body = physicsGetBody(bodyIdx);
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

    struct Body* body = physicsGetBody(bodyIdx);
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

    // check if component is already  attached, except for spawner (TODO)  
    // shift the number 1 by the type position,
    // then AND to check that type of component is attached or not
    if (HAS_COMPONENT( entity, type ) ) return -2;

    switch (type) {
        case COMPONENT_SPRITE:
            g_sprites[entity] = *(struct Sprite*)data;
            break;
        case COMPONENT_BULLET_SPAWNER:
            g_spawners[entity] = *(struct BulletSpawner*)data;
	    memset(&g_pattern_states[entity], 0, sizeof(struct PatternEntityState));
	    g_pattern_states[entity].pattern_started = false;  
	    break;
    case COMPONENT_SHAPE:
	    g_shapes[entity] = *(struct ShapeComponent*)data;
	    break;
        default:
            return -3;
    }
    //turn bit for that component on
    ADD_COMPONENT( entity, type );
    return 0;
}

int
ComponentDetach( Entity entity, ComponentType type ) {
    if ( entity >= MAX_ENTITIES ) return -1;
    if ( entity == 0 ) return 1;
    uint32_t flag = (1 << type);
    //AND to see if the component is attached or not
   if ( !HAS_COMPONENT( entity, type ) ) {
        return -2; 
    }
    
    REMOVE_COMPONENT(entity, type); // remove the component thats attached 
    return 0;
}

void*
ComponentGetData(Entity entity, ComponentType type) {
    if ( entity == 0 || entity >= MAX_ENTITIES ) return NULL;
    if ( ! ( HAS_COMPONENT( entity, type ) ) ) return NULL;
    
    switch (type) {
    case COMPONENT_TRANSFORM: return &g_transforms[entity];
    case COMPONENT_SPRITE:    return &g_sprites[entity];
    case COMPONENT_BULLET_SPAWNER: return &g_spawners[entity];
    case COMPONENT_SHAPE: return &g_shapes[entity];
    default: return NULL;
    }
}
