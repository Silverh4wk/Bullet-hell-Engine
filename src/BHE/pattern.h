#ifndef PATTERN_H
#define PATTERN_H

#include "../types.h"
#include "../engine/component.h"
#include "../engine/entity.h"


// Bullet Types

struct BulletType {
    vec4      color;
    vec2      size;               // this is becoming a deep problem but vec2 size for quad and vec2 radius for circle where both radii (r[0] == r[1]) are the same size
    vec2      acceleration;       // horizontal and vertical
    real32    speed;             
    real32    lifetime;         // 0 = infinite
    GLuint    texture;  
} ;

uint32              bulletTypeCreate         ( vec4 color, vec2 size, real32 speed, real32 lifetime, GLuint texture );
void                bulletTypeDestroy        ( uint32 bullet_id );
void                bulletTypeSetAcceleration( uint32 bullet_id, real32 ax, real32 ay );
void                bulletFreeListPush       ( Entity entity );
Entity              bulletFreeListPop        ( void );
void                bulletSystemUpdate       ( real32 delta_time );
struct BulletType*  bulletTypeGet            ( uint32 id );


typedef enum {
    PATCMD_DELAY,
    PATCMD_SET_BULLET_TYPE,
    PATCMD_SET_SPEED,
    PATCMD_SET_ACCELERATION,
    PATCMD_SET_COLOR,
    PATCMD_SET_SIZE,
    PATCMD_SET_LIFETIME,
    PATCMD_SET_ROTATION,
    PATCMD_SET_OSCILLATION,
    PATCMD_SET_WIGGLE,
    PATCMD_FIRE_CIRCLE,
    PATCMD_FIRE_ARC,
    PATCMD_FIRE_RING,
    PATCMD_FIRE_SPIRAL,
    PATCMD_FIRE_WAVE,
    PATCMD_FIRE_RANDOM_SPREAD,
    PATCMD_FIRE_AIMED_AT_PLAYER,
    PATCMD_CHAIN_PATTERN,
    PATCMD_REPEAT_BEGIN,
    PATCMD_REPEAT_END,
    PATCMD_TIMING_FUNCTION,
} PatternCommandOpcode;

// pattern Commands
struct PatternCommand {
    PatternCommandOpcode opcode;
    real32 param[8];            // generic parameters, depending on the function
    uint32 intParam[2];         // bullet_type_id, child_pattern_id, and so on.
};

// pttern Definition
struct Pattern {
    struct Array_List *commands;          // list to store PatternCommand(s)
    uint32 id;
    uint32 default_bullet_type_id;        // initial bullet type
    uint32 global_repeat;                 // how many times a cmd list should repeat, default is 1 time before ending execution every loop
    uint32 timing_function;               // PatternTiming value
    real32 duration;                      // 0 = infinite
    };


void
patternSystemInit( void ) ;

// public API
// ----  Creation & Setup ----
uint32 PatternCreate             ( void );
void   PatternDestroy            ( uint32 pattern_id );
void   PatternSetRepeats         ( uint32 pattern_id, uint32 repeats );
void   PatternSetDuration        ( uint32 pattern_id, real32 seconds );
				    
// ----  Angle & Spread ----	    
void PatternAddCircle            ( uint32 pattern_id, uint32 count, real32 speed, real32 delay );
void PatternAddArc               ( uint32 pattern_id, uint32 count, real32 start_angle, real32 end_angle, real32 speed, real32 delay );
void PatternAddRing              ( uint32 pattern_id, uint32 rings_count, uint32 bullets_per_ring, real32 speed, real32 ring_spacing, real32 delay );
void PatternAddSpiral            ( uint32 pattern_id, uint32 count, real32 start_angle, real32 angle_increment, real32 speed, real32 acceleration, real32 delay );
void PatternAddWave              ( uint32 pattern_id, uint32 count, real32 amplitude, real32 frequency, real32 phase_shift, real32 vertical_shift, real32 base_speed, real32 delay );
void PatternAddRandomSpread      ( uint32 pattern_id, uint32 count, real32 min_angle, real32 max_angle, real32 min_speed, real32 max_speed, real32 delay );
void PatternAddAimedAtPlayer     ( uint32 pattern_id, uint32 count, real32 spread, real32 speed, real32 delay );
				    
// ----  Chaining & Timing ----	    
void PatternAddDelay             ( uint32 pattern_id, real32 delay );
void PatternChainPattern         ( uint32 parent_id, uint32 child_id, real32 delay );
void PatternSetTimingFunction    ( uint32 pattern_id, int function_type );
				    
// ----  Overrides ----		    
void PatternSetBulletType        ( uint32 pattern_id, uint32 bullet_id );
void PatternSetSpeed             ( uint32 pattern_id, real32 speed );
void PatternSetAcceleration      ( uint32 pattern_id, real32 accel_x, real32 accel_y );
void PatternSetColor             ( uint32 pattern_id, vec4 color );
void PatternSetSize              ( uint32 pattern_id, real32 width, real32 height );
void PatternSetLifetime          ( uint32 pattern_id, real32 lifetime );
				    
// ----  Transform & Animation - - --
void PatternSetRotation          ( uint32 pattern_id, real32 degrees_per_second );
void PatternSetOscillation       ( uint32 pattern_id, real32 amplitude, real32 frequency );
void PatternAddWiggle            ( uint32 pattern_id, real32 intensity, real32 frequency );


// ----  Presets ----
uint32 PatternCreateStreamingStyle ( uint32 num_streams, real32 density, real32 speed );
uint32 PatternCreateCurtainFire    ( uint32 columns, uint32 rows, real32 spacing, real32 speed );
uint32 PatternCreateRandomBarrage  ( uint32 count, real32 min_speed, real32 max_speed, real32 duration );
uint32 PatternCreateShotgun        ( uint32 pellets, real32 spread, real32 speed );
uint32 PatternCreateLaser          ( real32 width, real32 length, real32 sweep_angle, real32 duration );


// runtime spawning ,  this to be called every frame
void patternSystemUpdate( real32 delta_time );

// for targeting the player 
void patternSetPlayerEntity( Entity player );

void patternSystemShutdown( void );

#endif
