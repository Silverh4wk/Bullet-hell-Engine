#include "../physics.h"
#include "../../objects/shapes.h"
#include "../global.h"
#include "../pool_allocator.h"
#include "physics_internal.h"
#include "spatial_hashing.h"
#include "../entity.h"
#include "../AABB.h"

static struct PhysicsStateInternal state;

void
physicsInit(void)
{
    state.body_list = arrayListCreate(sizeof(struct Body), 0);
}

// funny physics system:
// Two separate (bodies) --- the physical body and the rendered body
// the physics body handle all the physical aspects, like moving collision etc
// the rendered body is just the visual represenation of what happens to that body


// grab all bodies in the game from the global body list and update them accordingly
static inline
void updateBodiesPosition(void)
{
    struct Body *body ;
    
    for(uint32 i =0 ;i < state.body_list->len;++i)
    {
	body = (struct Body*) arrayListGet(state.body_list, i);
	
        if (body->entity != 0)
	{

	    struct Transform* t = entityGetTransform(body->entity);
	    struct Shape* shape = g_shapes[body->entity].shape;

	    if (shape)
	    {
		shape->pos[0] = body->aabb.coords[0];
		shape->pos[1] = body->aabb.coords[1];
	    }

	    if (t)
	    {
                t->position[0] = body->aabb.coords[0];
                t->position[1] = body->aabb.coords[1];
            }
        }
	
        if (body->active == false) continue;
	body->velocity[0] += body->acceleration[0] * global.time.delta;
        body->velocity[1] += body->acceleration[1] * global.time.delta;
	
	 body->aabb.coords[0] += body->velocity[0] * global.time.delta;
	 body->aabb.coords[1] += body->velocity[1] * global.time.delta;
	 
    }
}

// should handle all the physics update functions
// currently only got the position function
void
physicsUpdate(void)
    {
	updateBodiesPosition();
	broadPhaseResolve(); 
	//physicsRemoveInactiveBodies();
    }

//init a body with physics properties
size_t
physicsBodyCreate(struct Shape* sptr,vec2 pos, vec2 size,Type t) {    
    struct Body body = {
	.aabb = {
	    .coords = {pos[0],pos[1]},
	    .dims ={ size[0]/2,size[1]/2}, //physical body for a circle is still the same as a quad
	},
	.velocity = {0,0},
	.onCollision = NULL,
	.type = t,
	.entity = 0,
	.active = true,
	.lifetime = 0,
	.hp = 1,
// if i ever think of adding gravity but who knows (this thought was stupid of me, im a changed man now)	
//.gravity  = 9.80665f,
    };

    // attempt to append the newly created body to the global list of bodies to store its reference
    // else exit
    if(arrayListAppend(state.body_list,&body) == (size_t)-1)
	ERROR_EXIT("Could not append body to list of bodies\n");
    size_t index = state.body_list->len -1; 
    
    return index;	    
};


// get the total count of physical bodies created
size_t
physicsGetBodyCount(void) { return state.body_list->len; }

// get a body from the global list via its index
struct Body*
physicsGetBody(size_t i) { return (struct Body*) arrayListGet(state.body_list, i); }


void
physicsBodyDestroy(size_t index)
{
    if (!state.body_list) return;

    // check if within range
    if (index >= state.body_list->len) return;
    // then destroy it
    arrayListRemove(state.body_list, index);
}


// does what it says
// if you need to remove that specific body
void
physicsBodyDestroyByPtr(struct Body* body)
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

void
physicsToggleAllHitBoxes(void)
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

void
physicsRemoveInactiveBodies(void) {
    for (size_t i = 0; i < state.body_list->len; ) {
        struct Body* b = physicsGetBody(i);
        if (!b->active) {
            arrayListRemove(state.body_list, i); 
        } else {
            i++;
        }
    }
}

//https://math.stackexchange.com/questions/2229130/dividing-a-rectangle-into-a-grid-of-rectangles-squares
/* static inline */
/* int computeGridSide(real32 W, real32 H, int k) */
/* { */
/*     real32 WxH = W* H; */
/*     real32 no_of_grids = k; */

/*     int side = floorf(sqrtf(WxH/no_of_grids)); */

/*     return side ; */
/*     } */

void
broadPhaseResolve(void) {
    
    if (state.body_list->len == 0) return;
    
// build spatial hash for all active bodies
    int spacing = SPATIAL_HASH_SPACING;
    int number_of_objects  = (int)state.body_list->len;
    if((number_of_objects) == 0) return;

    struct SpatialHash* sh = spatialHashCreate(spacing, number_of_objects);

    spatialHashBuildAABB(sh);

    struct Array_List* candidates = arrayListCreate(sizeof(struct Body*), 0);
    
    for (int i = 0; i < state.body_list->len; i++) {
        struct Body* body = physicsGetBody(i);
        if (body->active)
	{//ignore if the body isnt active
	    //will get deleted after

	    //query nearby bodies
	    spatialHashQuery(sh,body,i,body->aabb.dims[0]*2);
	    arrayListClear(candidates);
	    //add all bodies that are within this body cell
	    //(TODO:) also reminder to add a check later to see if they are within the same collisoin grp
	    for (int q = 0; q < sh->query_size; q++)
	    {
		
		int body_idx = *(int*)arrayListGet(sh->query_Ids,q);
		if (body_idx == (int)i) continue;       // skip self
	    
		struct Body* b = physicsGetBody(body_idx);
		if (b && b->active)
		{
		    arrayListAppend(candidates, &b);
		}
	    }
	    narrowPhaseResolve(body, candidates);
	}
	
    }
    arrayListDestroy(candidates);
    spatialHashDestroy(sh);
    
}


void narrowPhaseResolve(struct Body* a, struct Array_List* candidates) {
    for (size_t j = 0; j < candidates->len; j++) {
        struct Body* b = *(struct Body**)arrayListGet(candidates, j);
        if (a == b) continue;
        if (!b->active) continue;
	if (a->type ==BODY_PLAYER && b->type == BODY_ENEMY)
	    if (AABB_intersects(&a->aabb, &b->aabb))
		printf("A and B are colliding at %f : %f", a->aabb.coords[0],a->aabb.coords[1]);
    }
}

void physicsClearBodies(void)
{
    if (!state.body_list) return;
    state.body_list->len = 0;
}
