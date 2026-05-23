#include "pattern.h"
#include "../engine/ecs_internal.h"
#include "../engine/entity.h"
#include "../engine/pool_allocator.h"
#include "../engine/array_list.h"
#include "../engine/global.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
//( TODO ) wrap all the logging stuff inside one function

//  constants & pools
#define MAX_PATTERNS              256
#define MAX_BULLETS               1024
#define MAX_BULLET_TYPES          64
#define MAX_COMMANDS_PER_PATTERN  32   

static struct {

    struct BulletType *bullet_types[MAX_BULLET_TYPES];// we keep an array of pointers for quick ID lookup
    struct Pattern *patterns[MAX_PATTERNS]; // we keep an array of pointers for quick ID lookup
    struct PoolAllocator pattern_pool;
    struct PoolAllocator bullet_type_pool;
    
    uint32 next_bullet_type_id;
    uint32 next_pattern_id;

    Entity entity;
} GlobalPatternSystem;

// to track active bullets 
static Entity bulletFreeList[MAX_BULLETS];
static int    bulletFreeCount = 0;
static int    bulletActiveCount = 0;

// forward declarations
static void
executePattern( Entity e, struct Pattern *pat, real32 dt,
		struct PatternEntityState *state, vec2 spawnPos );

static void spawnbullet( const struct PatternCommand *cmd, int bullet_idx,
			 Entity spawner, uint32 bullet_type_id, vec2 spawnPos,
			 struct PatternEntityState *state );
static real32
getTimingFactor(  uint32 func, real32 t  );

static bool advanceFireCommand( uint32 count, real32 base_delay,
				uint32 timing_func,
				struct PatternEntityState *state,
				const struct PatternCommand *cmd,
				Entity spawner_entity, vec2 spawnPos )
{
    real32 total_duration = ( real32 )count * base_delay;
    real32 elapsed   = state->time_in_cmd;
    if ( total_duration <= 0.0f || count ==0 ) {
	state->bullets_fired_in_cmd = 0;
	return true; // return if zero
    } 
    real32 progress  = fminf( elapsed / total_duration, 1.0f ); // make sure it stays between 0--1
    real32 eased     = getTimingFactor( timing_func, progress );

    //expected bullet count 
    int target = ( int )floorf( eased * ( real32 )( count - 1 ) + 0.5f ) + 1;
    if ( target > ( int )count ) target = count;

    while ( state->bullets_fired_in_cmd < target ) {
        spawnbullet( cmd, state->bullets_fired_in_cmd, spawner_entity,
		     state->current_bullet_type_id, spawnPos, state );
        state->bullets_fired_in_cmd++;
    }

    if ( state->bullets_fired_in_cmd == ( int )count ) {
        state->time_in_cmd -= total_duration;
        state->bullets_fired_in_cmd = 0;
        return true;
    }
    return false;   // still waiting
    
}



void
initPatternSystemPools (  void  )
{
    initPool(  &GlobalPatternSystem.pattern_pool, sizeof(  struct Pattern  ), MAX_PATTERNS  );
    initPool(  &GlobalPatternSystem.bullet_type_pool, sizeof(  struct BulletType  ), MAX_BULLET_TYPES  );
}

void
patternSystemInit(  void  ) {
    initPatternSystemPools(  );
    memset(  GlobalPatternSystem.patterns, 0, sizeof(  GlobalPatternSystem.patterns  )  );
    memset(  GlobalPatternSystem.bullet_types, 0, sizeof(  GlobalPatternSystem.bullet_types  )  );
    GlobalPatternSystem.next_pattern_id = 1;
    GlobalPatternSystem.next_bullet_type_id = 1;
    GlobalPatternSystem.entity = 0;
}

void patternSystemShutdown( void ) {
    for ( int i = 0; i < MAX_PATTERNS; i++ ) {
        if ( GlobalPatternSystem.patterns[i] ) {
            if ( GlobalPatternSystem.patterns[i]->commands )
                arrayListDestroy( GlobalPatternSystem.patterns[i]->commands );
            deallocatePool( &GlobalPatternSystem.pattern_pool, GlobalPatternSystem.patterns[i] );
        }
    }
    for ( int i = 0; i < MAX_BULLET_TYPES; i++ ) {
        if ( GlobalPatternSystem.bullet_types[i] )
            deallocatePool( &GlobalPatternSystem.bullet_type_pool, GlobalPatternSystem.bullet_types[i] );
    }
    destroyPool( &GlobalPatternSystem.pattern_pool );
    destroyPool( &GlobalPatternSystem.bullet_type_pool );
}




uint32
bulletTypeCreate(  vec4 color, vec2 size, real32 speed, real32 lifetime, GLuint texture  ) {
    if (  GlobalPatternSystem.next_bullet_type_id >= MAX_BULLET_TYPES  ) return 0; //add proper logging here
    struct BulletType *bullet_type = allocatePool(  &GlobalPatternSystem.bullet_type_pool  ); //alloc a pool for this bullet type
    if (  !bullet_type  ) return 0;
    // assigning,  can be added using other override functions
    // while i do wish to set everything in one command, this way makes it more readable and easier to understand
    memcpy(  bullet_type->color, color, sizeof(  vec4  )  ); 
    memcpy(  bullet_type->size, size, sizeof(  vec2  )  );
    bullet_type->speed = speed;
    bullet_type->acceleration[0] = bullet_type->acceleration[1] = 0.0f;
    bullet_type->lifetime = lifetime;
    bullet_type->texture = texture;
    uint32 id = GlobalPatternSystem.next_bullet_type_id++; // prepare its id 
    GlobalPatternSystem.bullet_types[id] = bullet_type; // point to this bullet using its given id 
    return id;
}

void
bulletTypeDestroy(  uint32 id  ) {
    if (  id >= MAX_BULLET_TYPES || !GlobalPatternSystem.bullet_types[id]  ) return; // add proper looging here
    deallocatePool(  &GlobalPatternSystem.bullet_type_pool, GlobalPatternSystem.bullet_types[id]  );
    GlobalPatternSystem.bullet_types[id] = NULL;
}

void
bulletTypeSetAcceleration(  uint32 id, real32 ax, real32 ay  ) {
    struct BulletType *bullet_type = bulletTypeGet(  id  );
    if (  !bullet_type  ) return; 
    bullet_type->acceleration[0] = ax;
    bullet_type->acceleration[1] = ay;
}


struct BulletType*
bulletTypeGet(  uint32 id  ) {
    if (  id >= MAX_BULLET_TYPES  ) return NULL;
    return GlobalPatternSystem.bullet_types[id];
}


void bulletFreeListPush( Entity e ) {
    if(e==0) return;
    if ( bulletFreeCount < MAX_BULLETS ) {
        bulletFreeList[bulletFreeCount++] = e;
        if ( bulletActiveCount > 0 ) --bulletActiveCount;
    }
}

Entity bulletFreeListPop( void ) {
    if ( bulletFreeCount > 0 ) {
	--bulletFreeCount;
        Entity e = bulletFreeList[bulletFreeCount];
        ++bulletActiveCount;
        return e;
    }
    return 0;
}

void bulletSystemUpdate( real32 dt ) {
    for ( Entity e = 1; e <= g_next_free; e++ )
    {
        if ( e >= MAX_ENTITIES ) break;  // safety bounds check
        if ( !HAS_COMPONENT( e, COMPONENT_SHAPE ) ) continue;
        int32 idx = g_body_indices[e];
        if ( idx == -1 || idx < 0 ) continue;  // ensure valid body index
        struct Body *body = physicsGetBody( idx );
        if ( !body || !body->active ) continue;
	if ( body->lifetime > 0 )
	{
            body->lifetime -= dt;
	}
	if ( body->lifetime <= 0 )
	{
	    body->active = false;
	    // Hide its shape
	    if ( body->sptr ) {
		body->sptr->pos[0] = body->sptr->pos[1] = 50;//-10000; // move pos to offscreen
	    }
	    bulletFreeListPush( e );
	}
    }
}



// Pattern Creation / Destruction
uint32
PatternCreate(  void  ) {
    if (  GlobalPatternSystem.next_pattern_id >= MAX_PATTERNS  ) return 0; // add proper logging here
    struct Pattern *pat = allocatePool(  &GlobalPatternSystem.pattern_pool  );
    if (  !pat  ) return 0;
    pat->id = GlobalPatternSystem.next_pattern_id;
    pat->commands = arrayListCreate(  sizeof(  struct PatternCommand  ), 16  );
    pat->default_bullet_type_id = 0;
    pat->global_repeat = 1;
    pat->duration = 0.0f;
    pat->timing_function = TIMING_LINEAR; //default timing function
    uint32 id = GlobalPatternSystem.next_pattern_id++;
    GlobalPatternSystem.patterns[id] = pat;
    return id;
}

void
PatternDestroy(  uint32 pattern_id  ) {
    if (  pattern_id >= MAX_PATTERNS || !GlobalPatternSystem.patterns[pattern_id]  ) return; // add proper logging her
    struct Pattern *pat = GlobalPatternSystem.patterns[pattern_id];
    if (  pat->commands  )
        arrayListDestroy(  pat->commands  );
    deallocatePool(  &GlobalPatternSystem.pattern_pool, pat  );
    GlobalPatternSystem.patterns[pattern_id] = NULL;
}


// append a cmd to the list of cmds for that pattern
static void addCommand(  uint32 pattern_id, struct PatternCommand cmd  ) {
    if (  pattern_id >= MAX_PATTERNS || !GlobalPatternSystem.patterns[pattern_id]  ) return;
    struct Pattern *pat = GlobalPatternSystem.patterns[pattern_id];
    arrayListAppend(  pat->commands, &cmd  );
}

////////////////////////////////////////////
//Basic Setup                             //
////////////////////////////////////////////

// a wrapper that repeasts the whole list from start to end
void
PatternSetRepeats( uint32 pattern_id, uint32 repeats ) {
    if ( pattern_id >= MAX_PATTERNS || !GlobalPatternSystem.patterns[pattern_id] ) return; // add proper error logging
    struct Pattern *pattern = GlobalPatternSystem.patterns[pattern_id];
    pattern->global_repeat = repeats; //set the number of repeats inside that pattern
}

void PatternSetDuration( uint32 pattern_id, real32 seconds ) {
    if ( pattern_id >= MAX_PATTERNS || !GlobalPatternSystem.patterns[pattern_id] ) return; // add proper logging here
    struct Pattern* pattern = GlobalPatternSystem.patterns[pattern_id];
    pattern->duration = seconds; //set the duration of inside that pattern
}

////////////////////////////////////////////
// Angle & Spread Definitions             //
////////////////////////////////////////////

void
PatternAddCircle(  uint32 pattern_id, uint32 count, real32 speed, real32 delay  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_FIRE_CIRCLE;
    cmd.param[0] = (  real32  ) count;
    cmd.param[1] = speed;
    cmd.param[2] = delay; 
    addCommand(  pattern_id, cmd  );
}

void
PatternAddArc(  uint32 pattern_id, uint32 count, real32 start_angle, real32 end_angle, real32 speed, real32 delay  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_FIRE_ARC;
    cmd.param[0] = (  real32  )count;
    cmd.param[1] = start_angle;
    cmd.param[2] = end_angle;
    cmd.param[3] = speed;
    cmd.param[4] = delay;
    addCommand(  pattern_id, cmd  );
}

void
PatternAddRing(  uint32 pattern_id, uint32 rings_count, uint32 bullets_per_ring, real32 speed, real32 ring_spacing, real32 delay  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_FIRE_RING;
    cmd.param[0] = (  real32  )rings_count;
    cmd.param[1] = (  real32  )bullets_per_ring;
    cmd.param[2] = speed;
    cmd.param[3] = ring_spacing;
    cmd.param[4] = delay;
    addCommand(  pattern_id, cmd  );
}

void
PatternAddSpiral(  uint32 pattern_id, uint32 count, real32 start_angle, real32 angle_increment, real32 speed, real32 acceleration, real32 delay  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_FIRE_SPIRAL;
    cmd.param[0] = (  real32  )count;
    cmd.param[1] = start_angle;
    cmd.param[2] = angle_increment;
    cmd.param[3] = speed;
    cmd.param[4] = acceleration;
    cmd.param[5] = delay;
    addCommand(  pattern_id, cmd  );
}

void
PatternAddWave(  uint32 pattern_id, uint32 count, real32 amplitude, real32 frequency,real32 phase_shift, real32 vertical_shift, real32 base_speed, real32 delay  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_FIRE_WAVE;
    cmd.param[0] = (  real32  )count;
    cmd.param[1] = amplitude;
    cmd.param[2] = frequency;
    cmd.param[3] = phase_shift;
    cmd.param[4] = vertical_shift;
    cmd.param[5] = base_speed;
    cmd.param[6] = delay;
    addCommand(  pattern_id, cmd  );
}

void
PatternAddRandomSpread(  uint32 pattern_id, uint32 count, real32 min_angle, real32 max_angle, real32 min_speed, real32 max_speed, real32 delay  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_FIRE_RANDOM_SPREAD;
    cmd.param[0] = (  real32  )count;
    cmd.param[1] = min_angle;
    cmd.param[2] = max_angle;
    cmd.param[3] = min_speed;
    cmd.param[4] = max_speed;
    cmd.param[5] = delay;
    addCommand(  pattern_id, cmd  );
}

void PatternAddAimedAtPlayer(  uint32 pattern_id, uint32 count, real32 spread, real32 speed, real32 delay  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_FIRE_AIMED_AT_PLAYER;
    cmd.param[0] = (  real32  )count;
    cmd.param[1] = spread;
    cmd.param[2] = speed;
    cmd.param[3] = delay;
    addCommand(  pattern_id, cmd  );
}

////////////////////////////////////////////
//  Timing & Chaining                     //
////////////////////////////////////////////

void
PatternAddDelay(  uint32 pattern_id, real32 delay  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_DELAY;
    cmd.param[0] = delay;
    addCommand(  pattern_id, cmd  );
}

// used to chain multiple patterns if wanted 
void
PatternChainPattern(  uint32 parent_id, uint32 child_id, real32 delay  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode      = PATCMD_CHAIN_PATTERN;
    cmd.intParam[0] = child_id;
    cmd.param[0]    = delay;
    addCommand(  parent_id, cmd  );
}

void
PatternSetTimingFunction(  uint32 pattern_id, int function_type  ) {
    if (  pattern_id >= MAX_PATTERNS || !GlobalPatternSystem.patterns[pattern_id]  ) return; // add proper logging here
    struct Pattern* pattern = GlobalPatternSystem.patterns[pattern_id];
    pattern->timing_function = (  uint32  )function_type;

    
}

// timing functions
// https://easings.net/
// https://blog.febucci.com/2018/08/easing-functions/

static real32
flip(real32 t) {
    return 1 - t;
}

static real32
timingEaseLinear(  real32 t  ) {
    return t;   
}


//starts slow, then accelerates.
static real32 timingEaseIn( real32 t ) {
    return t * t;
}


static real32
timingEaseInSine( real32 t ) {
    return flip(cosf( (t* Pi32 )  / 2 ) );
}

// fast start, slow end
static real32
timingEaseOut(  real32 t  ) {
    return flip( ( flip( t ) * flip( t ) ) );   //flip(Square(flip(t)));
}


static real32
timingEaseOutSine(  real32 t  ) {
    return sinf((t * Pi32 ) / 2);
}

//start accelerating (EaseIn) and stop decelerating (EaseOut),
static real32
timingEaseInOut(  real32 t  ) {
    return lerp(timingEaseIn( t ), timingEaseOut( t ), t); 
}

static real32
timingEaseInOutBack( real32 t ) {
    real32 c1 = 1.170158;
    real32 c2 = c1 * 1.525;

    return t < 0.5f ? ( powf( 2 * t, 2 )  * ( ( c2 + 1 ) * 2 * t - c2)) / 2: 
	              ( powf( 2 * t - 2, 2 ) * ( ( c2 + 1 ) * (t * 2 - 2) + c2) + 2) / 2;
}

//"mirrored" easing
//we use easein until we get to 50% and then flip and do the same
static real32
timingSpike( real32 t ) {
    if ( t >= 0.5f)
	return timingEaseIn( t / 0.5f );

    return timingEaseIn(flip( t ) / 0.5f );
}


static real32
getTimingFactor(  uint32 func, real32 t  ) {
    switch ( func ) {

    case TIMING_LINEAR:             return timingEaseLinear( t );
    case TIMING_EASE_IN:            return timingEaseIn( t );
    case TIMING_EASE_IN_SINE:       return timingEaseInSine( t );
    case TIMING_EASE_OUT:           return timingEaseOut( t );
    case TIMING_EASE_OUT_SINE:      return timingEaseOutSine( t );
    case TIMING_EASE_IN_OUT:        return timingEaseInOut( t );
    case TIMING_EASE_IN_OUT_BACK:   return timingEaseInOutBack( t );
    case TIMING_SPIKE:              return timingSpike( t );

	// just return back the progress by default
    default: return t;
    }
}

////////////////////////////////////////////
// Overrides                              //
////////////////////////////////////////////

void
PatternSetBulletType(  uint32 pattern_id, uint32 bullet_type_id  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode      = PATCMD_SET_BULLET_TYPE;
    cmd.intParam[0] = bullet_type_id;
    addCommand(  pattern_id, cmd  );
    if (  pattern_id >= MAX_PATTERNS || !GlobalPatternSystem.patterns[pattern_id]  ) return; // add proper logging here
    
    struct Pattern* pattern =  GlobalPatternSystem.patterns[pattern_id];
    pattern->default_bullet_type_id = bullet_type_id;
	
}

void
PatternSetSpeed(  uint32 pattern_id, real32 speed  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_SET_SPEED;
    cmd.param[0] = speed;
    addCommand(  pattern_id, cmd  );
}

void
PatternSetAcceleration(  uint32 pattern_id, real32 accel_x, real32 accel_y  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_SET_ACCELERATION;
    cmd.param[0] = accel_x;
    cmd.param[1] = accel_y;
    addCommand(  pattern_id, cmd  );
}

void
PatternSetColor(  uint32 pattern_id, vec4 color  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode = PATCMD_SET_COLOR;
    memcpy(  cmd.param, color, sizeof(  vec4  )  );
    addCommand(  pattern_id, cmd  );
}

void
PatternSetSize(  uint32 pattern_id, real32 width, real32 height  ) {
    struct  PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_SET_SIZE;
    cmd.param[0] = width;
    cmd.param[1] = height;
    addCommand(  pattern_id, cmd  );
}

void
PatternSetLifetime(  uint32 pattern_id, real32 lifetime  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_SET_LIFETIME;
    cmd.param[0] = lifetime;
    addCommand(  pattern_id, cmd  );
}

////////////////////////////////////////////
// 4.12.3.5 Transform & Animation Effects //
////////////////////////////////////////////

void
PatternSetRotation(  uint32 pattern_id, real32 degrees_per_second  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode = PATCMD_SET_ROTATION;
    cmd.param[0] = degrees_per_second;
    addCommand(  pattern_id, cmd  );
}

void
PatternSetOscillation(  uint32 pattern_id, real32 amplitude, real32 frequency  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_SET_OSCILLATION;
    cmd.param[0] = amplitude;
    cmd.param[1] = frequency;
    addCommand(  pattern_id, cmd  );
}

void
PatternAddWiggle(  uint32 pattern_id, real32 intensity, real32 frequency  ) {
    struct PatternCommand cmd = {0};
    cmd.opcode   = PATCMD_SET_WIGGLE;
    cmd.param[0] = intensity;
    cmd.param[1] = frequency;
    addCommand(  pattern_id, cmd  );
}


////////////////////////////////////////////
//  Presets                               //
////////////////////////////////////////////

uint32
PatternCreateStreamingStyle(  uint32 num_streams, real32 density, real32 speed  ) {
    uint32 pid = PatternCreate(  );
    for (  uint32 i = 0; i < num_streams; i++  ) {
        real32 offset = (  real32  )i * density;
        PatternAddDelay(  pid, 0.1f  );
        PatternAddCircle(  pid, 6, speed + offset, 0.05f  );
    }
    return pid;
}

uint32
PatternCreateCurtainFire(  uint32 columns, uint32 rows, real32 spacing, real32 speed  ) {
    uint32 pid = PatternCreate(  );
    for (  uint32 r = 0; r < rows; r++  ) {
        PatternAddDelay(  pid, 0.2f  );
        PatternAddArc(  pid, columns, -20.0f, 20.0f, speed, 0.1f  );
    }
    return pid;
}

uint32
PatternCreateRandomBarrage(  uint32 count, real32 min_speed, real32 max_speed, real32 duration  ) {
    uint32 pid = PatternCreate(  );
    PatternSetDuration(  pid, duration  );
    PatternAddRandomSpread(  pid, count, 0, 360, min_speed, max_speed, 0.0f  );
    
    return pid;
}
uint32
PatternCreateShotgun(  uint32 pellets, real32 spread, real32 speed  ) {
    uint32 pid = PatternCreate(    );
    PatternAddArc(  pid, pellets, -spread/2.0f, spread/2.0f, speed, 0.0f  );
    return pid;
}

uint32 PatternCreateLaser(  real32 width, real32 length, real32 sweep_angle, real32 duration  ) {
    // stub
    // prolly gonna have to create a long quad for this one so it shouldnt be hard, just a reminder for later
    return 0;
}

////////////////////////////////////////////
// Runtime execution per spawner          //
////////////////////////////////////////////

void
patternSetPlayerEntity(  Entity player  )
{
    GlobalPatternSystem.entity = player;
}

void
patternSystemUpdate( float delta_time ) {
    
    for (  Entity e = 1; e <= g_next_free; e++  ) {
        if (  !HAS_COMPONENT( e, COMPONENT_BULLET_SPAWNER  ) ) continue;

        struct BulletSpawner *spawner = &g_spawners[e];
        uint32 pid = spawner->pattern_id;
        if (  pid == 0 || pid >= MAX_PATTERNS  ) continue; //logging here

        struct Pattern *pat = GlobalPatternSystem.patterns[pid];
        if ( !pat ) continue;

        struct PatternEntityState *state = &g_pattern_states[e];

        // get spawner world position from its transform
        struct Transform *t = entityGetTransform( e );
        if ( !t ) continue;
        vec2 spawnPos = { t->position[0], t->position[1] };

        // first time init -
        if ( !state->pattern_started ) {
            state->current_cmd_idx = 0;
            state->time_in_cmd = 0.0f;
            state->current_bullet_type_id = pat->default_bullet_type_id;
            state->override_flags = 0;
            state->repeats_remaining = pat->global_repeat; 
            state->total_time = 0.0f;
            state->bullets_fired_in_cmd = 0;
            state->pattern_started = true;
        }

        // check duration
        state->total_time += delta_time;
        if ( pat->duration > 0.0f && state->total_time >= pat->duration ) {  // stop spawning once the pattern time is done
            state->pattern_started = false;
            continue;  
        }

        // execute the cmmnd list 
        executePattern( e, pat, delta_time, state, spawnPos );
	
	// did we execute all?
        if ( state->current_cmd_idx >= pat->commands->len && state->repeats_remaining <= 1 ) {
            // repeats done = mark finished
            state->pattern_started = false;
            // spawner->pattern_id = 0;
        }
    }
}


#define OVERRIDE_SPEED       (  1<<0  )
#define OVERRIDE_ACCEL       (  1<<1  )
#define OVERRIDE_COLOR       (  1<<2  )
#define OVERRIDE_SIZE        (  1<<3  )
#define OVERRIDE_LIFETIME    (  1<<4 )
#define OVERRIDE_FIRE_SPEED  (  1<<5  )
#define OVERRIDE_FIRE_ACCEL  (  1<<6  )

static void
executePattern( Entity e, struct Pattern *pat, real32 dt,
		struct PatternEntityState *state, vec2 spawnPos ) {
    struct Array_List *cmdlist = pat->commands;
    if ( !cmdlist || cmdlist->len == 0 ) return;

    state->time_in_cmd += dt;

    while ( state->current_cmd_idx < cmdlist->len ) {
        struct PatternCommand *cmd = ( struct PatternCommand * )arrayListGet( cmdlist, state->current_cmd_idx );
        bool advance = false;

        switch ( cmd->opcode ) {

            //  delays 
	case PATCMD_DELAY:
	    if ( state->time_in_cmd >= cmd->param[0] ) {
		state->time_in_cmd = 0; //set it back to zero
		advance = true;
	    } else {
		return;  // still waiting
	    }
	    break;

            // overides
	case PATCMD_SET_BULLET_TYPE:
	    state->current_bullet_type_id = cmd->intParam[0];
	    advance = true; break;
	case PATCMD_SET_SPEED:
	    state->speed_override = cmd->param[0];
	    state->override_flags |= OVERRIDE_SPEED;
	    advance = true; break;
	case PATCMD_SET_ACCELERATION:
	    state->accel_x_override = cmd->param[0];
	    state->accel_y_override = cmd->param[1];
	    state->override_flags |= OVERRIDE_ACCEL;
	    advance = true; break;
	case PATCMD_SET_COLOR:
	    memcpy( state->color_override, cmd->param, sizeof( vec4 ) );
	    state->override_flags |= OVERRIDE_COLOR;
	    advance = true; break;
	case PATCMD_SET_SIZE:
	    state->size_override[0] = cmd->param[0];
	    state->size_override[1] = cmd->param[1];
	    state->override_flags |= OVERRIDE_SIZE;
	    advance = true; break;
	case PATCMD_SET_LIFETIME:
	    state->lifetime_override = cmd->param[0];
	    state->override_flags |= OVERRIDE_LIFETIME;
	    advance = true; break;

            // fire commands 
	case PATCMD_FIRE_CIRCLE: {
	    uint32 count = ( uint32 )cmd->param[0];
	    real32 base_delay = cmd->param[2];
	    if ( advanceFireCommand( count, base_delay, pat->timing_function, state, cmd, e, spawnPos ) )
		advance = true;
	    else
		return;
	    break;
	}
	case PATCMD_FIRE_ARC: {
	    uint32 count = ( uint32 )cmd->param[0];
	    real32 base_delay = cmd->param[4];
	    if ( advanceFireCommand( count, base_delay, pat->timing_function, state, cmd, e, spawnPos ) )
		advance = true;
	    else
		return;
	    break;
	}
	case PATCMD_FIRE_RING: {
	    uint32 rings = ( uint32 )cmd->param[0];
	    uint32 per_ring = ( uint32 )cmd->param[1];
	    uint32 count = rings * per_ring;
	    real32 base_delay = cmd->param[4];
	    if ( advanceFireCommand( count, base_delay, pat->timing_function, state, cmd, e, spawnPos ) )
		advance = true;
	    else
		return;
	    break;
	}
	case PATCMD_FIRE_SPIRAL: {
	    uint32 count = ( uint32 )cmd->param[0];
	    real32 base_delay = cmd->param[5];
	    if ( advanceFireCommand( count, base_delay, pat->timing_function, state, cmd, e, spawnPos ) )
		advance = true;
	    else
		return;
	    break;
	}
	case PATCMD_FIRE_WAVE: {
	    uint32 count = ( uint32 )cmd->param[0];
	    real32 base_delay = cmd->param[4];
	    if ( advanceFireCommand( count, base_delay, pat->timing_function, state, cmd, e, spawnPos ) )
		advance = true;
	    else
		return;
	    break;
	}
	case PATCMD_FIRE_RANDOM_SPREAD: {
	    uint32 count = ( uint32 )cmd->param[0];
	    real32 base_delay = cmd->param[5];
	    if ( advanceFireCommand( count, base_delay, pat->timing_function, state, cmd, e, spawnPos ) )
		advance = true;
	    else
		return;
	    break;
	}
	case PATCMD_FIRE_AIMED_AT_PLAYER: {
	    uint32 count = ( uint32 )cmd->param[0];
	    real32 base_delay = cmd->param[3];
	    if ( advanceFireCommand( count, base_delay, pat->timing_function, state, cmd, e, spawnPos ) )
		advance = true;
	    else
		return;
	    break;
	}

            //  chain pattern (NOTE:review this)
	case PATCMD_CHAIN_PATTERN: {
	    uint32 child_id = cmd->intParam[0];
	    struct BulletSpawner *spawner = &g_spawners[e];
	    spawner->pattern_id = child_id;
	    memset( state, 0, sizeof( struct PatternEntityState ) );
	    return;
	}

           
	default:
	    advance = true;
	    break;
        }

        if ( advance ) {
            state->current_cmd_idx++;
        }
    }

    // finished the command list?
    if ( state->repeats_remaining > 1 ) {
        state->repeats_remaining--;
        state->current_cmd_idx = 0;
        state->time_in_cmd = 0.0f;
        state->bullets_fired_in_cmd = 0;
        state->current_bullet_type_id = pat->default_bullet_type_id;
        state->override_flags = 0;
    } else {
        state->pattern_started = false;
    }
}



static void spawnbullet( const struct PatternCommand *cmd, int bullet_idx, Entity spawner,
			 uint32 bullet_type_id, vec2 spawnPos, struct PatternEntityState *state ) {

    
    if ( bulletActiveCount >= MAX_BULLETS ) return;

    // try to reuse the already made bullets from the pool
    Entity bullet = bulletFreeListPop(  );
    if ( bullet == 0 ) {
        bullet = entityInit( BODY_BULLET );
        if ( bullet == 0 ) return; // truly full
        
        ++bulletActiveCount;
    }
    
    //  get base bullet type
    struct BulletType *bt = bulletTypeGet( bullet_type_id );
    if ( !bt ) return;   // return if no bullettype is assigned

    // prepare final properties
    vec2 size; memcpy( size, bt->size, sizeof( vec2 ) );
    vec4 color; memcpy( color, bt->color, sizeof( vec4 ) );
    real32 speed = bt->speed;
    real32 ax = bt->acceleration[0], ay = bt->acceleration[1];
    real32 lifetime = bt->lifetime;

    if ( state->override_flags & OVERRIDE_SPEED )    speed   = state->speed_override;
    if ( state->override_flags & OVERRIDE_ACCEL )    { ax = state->accel_x_override; ay = state->accel_y_override; }
    if ( state->override_flags & OVERRIDE_COLOR )    memcpy( color, state->color_override, sizeof( vec4 ) );
    if ( state->override_flags & OVERRIDE_SIZE )     memcpy( size, state->size_override, sizeof( vec2 ) );
    if ( state->override_flags & OVERRIDE_LIFETIME ) lifetime = state->lifetime_override;

    // compute the spawn position and angle based on command type and bullet_idx
    vec2 pos = { spawnPos[0], spawnPos[1] };
    real32 angle = 0; // IN RADS cuz all math libs use it and honestly rad is just more reliable
    uint32 count = 0;
    
    switch ( cmd->opcode ) {
    case PATCMD_FIRE_CIRCLE: {
	count = ( uint32 )cmd->param[0];
	// evenly spaced in full circle
	angle = ( 2*Pi32 / count ) * bullet_idx;
	break;
    }
	
    case PATCMD_FIRE_ARC: {
	count = ( uint32 )cmd->param[0];
	real32 start_angle = degreeToRadians(cmd->param[1]);
	real32 end_angle   = degreeToRadians(cmd->param[2]);
	real32 t = ( real32 ) bullet_idx / ( real32 )( count - 1 ); //interpolation value - value used to interpolate between start and end
	angle = lerp( start_angle,end_angle,t );
	break;
    }
	
    case PATCMD_FIRE_RING: {
	
	uint32 rings_count = ( uint32 )cmd->param[0];
	uint32 bullet_per_ring = ( uint32 )cmd->param[1];
	real32 ring_spacing = cmd->param[3];

        count = rings_count *bullet_per_ring;
	if ( bullet_idx >= ( int )count ) return; // make sure it doesnt exceed the count
	//get the current ring, and the indx in that ring for the current bullet should be in
	uint32 ring = bullet_idx / bullet_per_ring;
	uint32 idx_in_ring = bullet_idx % bullet_per_ring;

	// angle for this bullet within its ring
	angle = ( 2*Pi32 / bullet_per_ring ) * idx_in_ring;
	real32 offset = ( real32 )ring * ring_spacing;
	// Offset the spawn position outward from spawner
	pos[0] += cosf( angle ) * offset;
	pos[1] += sinf( angle ) * offset;
	break;
    }
	
    case PATCMD_FIRE_SPIRAL: {
	count = ( uint32 )cmd->param[0];
	real32 start_angle     = degreeToRadians(cmd->param[1]);
	real32 angle_increment = degreeToRadians(cmd->param[2]);
	angle = start_angle + ( real32 )bullet_idx * angle_increment; //just inc the angle of each bullet, creating a stream
	break;
    }
	
// y(t)=A*sin(omega t+ varphi )= A*sin(2*pi ft+ varphi )[2]
	//where:
// https://en.wikipedia.org/wiki/Sine_wave
//  A     -  amplitude, the peak deviation of the function from zero.
//  t     -  the real independent variable, usually representing time in seconds.
//  omega -  angular frequency, the rate of change of the function argument in units of radians per second.
//  f     - ordinary frequency, the number of oscillations (cycles) that occur each second of time.

	
    case PATCMD_FIRE_WAVE: {
	count = ( uint32 )      cmd->param[0];
	real32 amplitude      = degreeToRadians(cmd->param[1]);
	real32 Hz             = cmd->param[2];
	real32 phase_shift    = degreeToRadians(cmd->param[3]);
	real32 vertical_shift = cmd->param[4];    
	real32 base_speed     = cmd->param[5];   // not directly used for angle, speed handled separately
	real32 delay          = cmd->param[6];        // time between bullets

	real32 phase = 2.0f * Pi32 * Hz * ( real32 )bullet_idx * delay;
	angle = amplitude * sinf( phase - phase_shift) + vertical_shift;
	break;
    }
	
    case PATCMD_FIRE_RANDOM_SPREAD: {
	count = ( uint32 )cmd->param[0];
	real32 min_angle = degreeToRadians(cmd->param[1]);
	real32 max_angle = degreeToRadians(cmd->param[2]);
	real32 min_speed = cmd->param[3];
	real32 max_speed = cmd->param[4];
	// just random every thing + lerp
	
	float t_angle = ( float )rand(  ) / ( float )RAND_MAX;
	angle = lerp(min_angle,max_angle, t_angle);
	float t_speed = ( float )rand(  ) / ( float )RAND_MAX;
	speed = lerp(min_speed, max_speed, t_speed);
	break;
    }
	
    case PATCMD_FIRE_AIMED_AT_PLAYER: {
	count = ( uint32 )cmd->param[0];
	real32 spread = degreeToRadians(cmd->param[1]);   // half-angle of cone
	
	// get the player from the globalsys
	Entity player = GlobalPatternSystem.entity;

	if ( player == 0 ) {
	    // no player set – fallback to right direction
	    angle = 0.0f;
	    break;
	}
	//track the player pos hehe
	struct Transform* player_transform = entityGetTransform( player );
	if ( !player_transform ) { angle = 0.0f; break; }
	
	real32 dx = player_transform->position[0] - spawnPos[0];
	real32 dy = player_transform->position[1] - spawnPos[1];
	real32 base_angle = atan2f( dy, dx );

        // add spread based on bullet_idx
	if ( count > 1 ) {
	    real32 t = (count > 1) ? (real32)bullet_idx / (real32)(count - 1): 0.0f;
            float min_angle = base_angle - spread;
	    float max_angle = base_angle + spread;
	    angle = lerp(min_angle, max_angle, t);
	} else {
	    angle = base_angle;
	}
	break;
    }
    default:

	break;
    }

    vec2 vel = { cosf( angle ) * speed, sinf( angle ) * speed };

    // Set transform
    entitySetTransform( bullet, &pos, &size, NULL );
    // Build shape ( quad for now.. extend to circle if needed )
    entityBuild( bullet, SHAPE_QUAD, true );// all bullets should have physical bodies
    entitySetColor( bullet, color );
    entitySetTexture( bullet, bt->texture );

    // finally add the fisixs body 
    entityAddPhysics( bullet );
    struct Body *body = physicsGetBody( g_body_indices[bullet] );
    if ( body ) {
        body->velocity[0] = vel[0];
        body->velocity[1] = vel[1];
        body->acceleration[0] = ax;
        body->acceleration[1] = ay;
	body->active = true;
	body->lifetime = lifetime;
    }

    // collisionGroupAddToGrp( bullet, "bullet" );
}

