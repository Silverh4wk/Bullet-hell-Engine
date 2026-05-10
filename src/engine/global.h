#ifndef GLOBAL_H
#define GLOBAL_H

#include "render.h"
#include "config.h"
#include "input.h"
#include "time.h"
#include "camera.h"
#include "ecs_internal.h"

//global structure that holds the global states of the engine
struct Global {
    struct Render_State render;
    struct Config_State config;
    struct Input_State  input;
    struct TimeState   time;
} ;
// ==== Global Variables ====
//

extern struct Global global;
extern SDL_Event event;
extern Camera main_camera;
extern SDL_Color colors[64];
extern int toggleHitBoxVisual;
extern bool global_running;

extern float fps_timer ;
extern int   fps_frames;
extern float fps_value ;



// storage for each component arrays
extern struct Transform      g_transforms[MAX_ENTITIES];
extern struct Sprite         g_sprites[MAX_ENTITIES];
extern struct BulletSpawner  g_spawners[MAX_ENTITIES];
extern        uint64         g_component_mask[MAX_ENTITIES];
extern        Entity         g_next_free;
extern        Type           g_type[MAX_ENTITIES];
extern struct ShapeComponent g_shapes[MAX_ENTITIES];
extern        int32          g_body_indices[MAX_ENTITIES]; // its so scuffed but to track what body belong to what
extern struct PatternEntityState g_pattern_states[MAX_ENTITIES];




#endif
