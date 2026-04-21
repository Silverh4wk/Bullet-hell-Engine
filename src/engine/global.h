#ifndef GLOBAL_H
#define GLOBAL_H

#include "render.h"
#include "config.h"
#include "input.h"
#include "time.h"
#include "camera.h"
//global structure that holds the global states of the engine
struct Global {
    struct Render_State render;
    struct Config_State config;
    struct Input_State  input;
    struct Time_State   time;
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


#endif
