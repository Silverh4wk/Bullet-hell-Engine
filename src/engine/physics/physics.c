#include "../physics.h"
#include "../../objects/shapes.h"
#include "../global.h"
#include "../pool_allocator.h"
#include "physics_internal.h"
#include "spatial_hashing.h"
#include "../entity.h"

static struct PhysicsStateInternal state;

void physicsInit(void)
{
    state.body_list = arrayListCreate(sizeof(struct Body), 0);
}

// funny physics system:
// Two separate (bodies) --- the physical body and the rendered body
// the physics body handle all the physical aspects, like moving collision etc
// the rendered body is just the visual represenation of what happens to that body


// grab all bodies in the game from the global body list and update them accordingly
static inline void updateBodiesPosition(void)
{
    struct Body *body ;
    for(uint32 i =0 ;i< state.body_list->len;++i)
    {
        body = (struct Body*) arrayListGet(state.body_list, i);
        body->velocity[0] += body->acceleration[0] * global.time.delta;
        body->velocity[1] += body->acceleration[1] * global.time.delta;

	 body->aabb.coords[0] += body->velocity[0] * global.time.delta;
	 body->aabb.coords[1] += body->velocity[1] * global.time.delta;
	 //update the visual shape pos accordingly
        if (body->sptr) {
            body->sptr->pos[0] = body->aabb.coords[0];
            body->sptr->pos[1] = body->aabb.coords[1];
        }
	
        if (body->entity != 0) {
            struct Transform* t = entityGetTransform(body->entity);
            if (t) {
                t->position[0] = body->aabb.coords[0];
                t->position[1] = body->aabb.coords[1];
            }
        }
    }
}

// should handle all the physics update functions
// currently only got the position function
void physicsUpdate(void)
    {
	updateBodiesPosition();
    }

//init a body with physics properties
size_t physicsBodyCreate(struct Shape* sptr,vec2 pos, vec2 size,Type t) {    
    struct Body body = {
	.aabb = {
	    .coords = {pos[0],pos[1]},
	    .dims ={ size[0]/2,size[1]/2}, //physical body for a circle is still the same as a quad
	},
	.velocity = {0,0},
	.onCollision = NULL,
	.type = t,
	.sptr = sptr,
    .entity = 0,
// if i ever think of adding gravity but who knows (this thought was stupid of me, im a changed man now)	
//.gravity  = 9.80665f,
    };
    
    // attempt to append the newly created body to the global list of bodies to store its reference
    // else exit
    if(arrayListAppend(state.body_list,&body) == (size_t)-1)
	ERROR_EXIT("Could not append body to list of bodies\n")
    return state.body_list->len -1;	    
};


// get the total count of physical bodies created
size_t physicsGetBodyCount(void) { return state.body_list->len; }

// get a body from the global list via its index
struct Body* physicsGetBody(size_t i) { return (struct Body*) arrayListGet(state.body_list, i); }


void physicsBodyDestroy(size_t index)
{
    if (!state.body_list) return;

    // check if within range
    if (index >= state.body_list->len) return;
    // then destroy it
    arrayListRemove(state.body_list, index);
}


// does what it says
// if you need to remove that specific body
void physicsBodyDestroyByPtr(struct Body* body)
{
    if (!state.body_list || !body) return;

    for (size_t i = 0; i < state.body_list->len; ++i)
    {
        struct Body* b = physicsGetBody(i);

        if (b == body)
        {
            arrayListRemove(state.body_list, i);
            return;
        }
    }
}

void physicsToggleAllHitBoxes(void)
{
    for (size_t i = 0; i < state.body_list->len; i++)
    {
	struct Body* body = physicsGetBody(i);
	// only in debug mode, toggle the aabb rendering flag to see the hitboxes
	body->aabb.toggle = (body->aabb.toggle == 0) ? 1 : 0;
    }
}

// when an body dies, and is no longer needed
// gets called at the end of physics update 
// they get removed from the game (note to self, i am not sure if i want this as
// "dead" objects are meant to stay outside in case they need to get recalled)

void physicsRemoveInactiveBodies(void) {
    for (size_t i = 0; i < state.body_list->len; ) {
        struct Body* b = physicsGetBody(i);
        if (!b->active) {
            arrayListRemove(state.body_list, i); 
        } else {
            i++;
        }
    }
}

void broadPhaseResolve(void) {
    if (state.body_list->len == 0) return;

    // build spatial hash for all active bodies
    struct SpatialHash* sh = spatialHashCreate();
    for (size_t i = 0; i < state.body_list->len; i++) {
        struct Body* body = physicsGetBody(i);
        if (body->active) {
            spatialHashInsert(sh, body);
        }
    }

    
    struct Array_List* candidates = arrayListCreate(sizeof(struct Body*), 0);

    //  query nearby bodies 
    for (size_t i = 0; i < state.body_list->len; i++) {
        struct Body* a = physicsGetBody(i);
        if (!a->active) continue; //ignore if the body isnt active
	                          //will get deleted after

        // Query radius = half the diagonal of AABB
        float radius = sqrtf(a->aabb.dims[0] * a->aabb.dims[0] +
                             a->aabb.dims[1] * a->aabb.dims[1]);

        arrayListClear(candidates);
        spatialHashQuery(sh, a->aabb.coords, radius, candidates);

        // Narrow phase... test all candidates against 'a'
        narrowPhaseResolve(a, candidates);
    }

    
    spatialHashDestroy(sh);
    arrayListDestroy(candidates);
}



void narrowPhaseResolve(struct Body* a, struct Array_List* candidates) {
    for (size_t j = 0; j < candidates->len; j++) {
        struct Body* b = *(struct Body**)arrayListGet(candidates, j);
        if (a == b) continue;
        if (!b->active) continue;
    }
}


void physicsClearBodies(void)
{
    if (!state.body_list) return;
    state.body_list->len = 0;
}
