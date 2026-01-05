#include "../physics.h"
#include "../global.h"
#include "../array_list.h"
#include "../../helpers.h"
#include "physics_internal.h"

static PhysicsStateInternal state;

void physicsInit(void)
{
    state.body_list = arrayListCreate(sizeof(Body), 0);
}

void physicsUpdate(void)
    {
	Body *body ;
	//for every body in the list of bodies, update its physics state and position
	for(uint32 i =0 ;i< state.body_list->len;++i)
	{
	    body = (Body*) arrayListGet(state.body_list, i);
	    body->velocity[0] += body->acceleration[0] * global.time.delta;
	    body->velocity[1] += body->acceleration[1] * global.time.delta;
	    body->aabb.position[0] += body->velocity[0] * global.time.delta;
	    body->aabb.position[1] += body->velocity[1] * global.time.delta;
	    
	}
}
//init a body with physics properties
size_t physicsBodyCreate(vec2 pos, vec2 size, vec4 color) {
    
    Body body = {
	.aabb = {
	    .position = {pos[0],pos[1]},
	    .half_size ={ size[0]/2,size[0]/2},
	    
	},
	.velocity = {0,0},
	.color = {color[0],color[1],color[2],color[3]},
//if i ever think of adding gravity but who knows	
//.gravity  = 9.8f,
    };

    //attempt to append the newly created body to the list of bodies
    //else exit
    if(arrayListAppend(state.body_list,&body) == (size_t)-1)
	ERROR_EXIT("Could not append body to list\n")
    return state.body_list->len -1;	    
};


Body *physicsBodyGet(size_t index)
{
    return (Body*) arrayListGet(state.body_list,index);
}

int testAABBAABB(AABB* a, AABB* b){
    
// Exit with no intersection if separated along an axis
    if( abs((int)a->position[0] - (int)b->position[0]) > (a->half_size[0] + b->half_size[0])) return 0;
    if( abs((int)a->position[1] - (int)b->position[1]) > (a->half_size[1] + b->half_size[1])) return 0;
    // Overlapping on all axes means AABBs are intersecting
// basically if 1, they overlapping aka colliding
return 1;
}

size_t physicsGetBodyCount(void) { return state.body_list->len; }
Body* physicsGetBody(size_t i) { return (Body*) arrayListGet(state.body_list, i); }


void physicsToggleAllHitBoxes(void)
{
    for (size_t i = 0; i < state.body_list->len; i++)
    {
	Body* body = (Body*) arrayListGet(state.body_list, i);
	body->aabb.toggle = (body->aabb.toggle == 0) ? 1 : 0;
    }
}
