#include "../physics.h"
#include "../global.h"
#include "../array_list.h"
#include "../../helpers.h"
#include "physics_internal.h"


static struct PhysicsStateInternal state;


void physicsInit(void)
{
    state.body_list = arrayListCreate(sizeof(struct Body), 0);
}

// Update all bodies in the list physics properties
// This to happen at the end of every frame
void physicsUpdate(void)
    {
	struct Body *body ;
	//for every body in the list of bodies, update its physics state and position
	for(uint32 i =0 ;i< state.body_list->len;++i)
	{
	    body = (struct Body*) arrayListGet(state.body_list, i);
	    body->velocity[0] += body->acceleration[0] * global.time.delta;
	    body->velocity[1] += body->acceleration[1] * global.time.delta;
	    body->aabb.coords[0] += body->velocity[0] * global.time.delta;
	    body->aabb.coords[1] += body->velocity[1] * global.time.delta;
	    
	}
}

//init a body with physics properties
size_t physicsBodyCreate(vec2 pos, vec2 size) {

    vec4 c;

// just any color for the hitbox #2563EB (pretty blue)
    setVec4(&c, 37, 99, 235, 1);
    
    
    struct Body body = {
	.aabb = {
	    .coords = {pos[0],pos[1]},
	    .dims ={ size[0]/2,size[0]/2},
	    
	},
	.velocity = {0,0},
	.color = {c[0],c[1],c[2],c[3]},
//if i ever think of adding gravity but who knows	
//.gravity  = 9.8f,
    };

    //attempt to append the newly created body to the list of bodies
    //else exit
    if(arrayListAppend(state.body_list,&body) == (size_t)-1)
	ERROR_EXIT("Could not append body to list of bodies\n")
    return state.body_list->len -1;	    
};


// grab a body from the list 
struct Body *physicsBodyGet(size_t index)
{
    return (struct Body*) arrayListGet(state.body_list,index);
}

int testAABBAABB(AABB* a, AABB* b){
    
// Exit with no intersection if separated along an axis
    if( abs((int)a->coords[0] - (int)b->coords[0]) > (a->dims[0] + b->dims[0])) return 0;
    if( abs((int)a->coords[1] - (int)b->coords[1]) > (a->dims[1] + b->dims[1])) return 0;
    // Overlapping on all axes means AABBs are intersecting
// basically if 1, they overlapping aka colliding
return 1;
}

size_t physicsGetBodyCount(void) { return state.body_list->len; }
struct Body* physicsGetBody(size_t i) { return (struct Body*) arrayListGet(state.body_list, i); }


void physicsToggleAllHitBoxes(void)
{
    for (size_t i = 0; i < state.body_list->len; i++)
    {
	struct Body* body = (struct Body*) arrayListGet(state.body_list, i);
	body->aabb.toggle = (body->aabb.toggle == 0) ? 1 : 0;
    }
}

void physicsBodyDestroy(size_t index)
{
    if (!state.body_list) return;
    if (index >= state.body_list->len) return;

    arrayListRemove(state.body_list, index);
}




void physicsBodyDestroyByPtr(struct Body* body)
{
    if (!state.body_list || !body) return;

    for (size_t i = 0; i < state.body_list->len; ++i)
    {
        struct Body* b = (struct Body*)arrayListGet(state.body_list, i);

        if (b == body)
        {
            arrayListRemove(state.body_list, i);
            return;
        }
    }
}


void physicsClearBodies(void)
{
    if (!state.body_list) return;
    state.body_list->len = 0;
}
