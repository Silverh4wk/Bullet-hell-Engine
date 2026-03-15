#ifndef GLOBAL_H
#define GLOBAL_H

#include "render.h"
#include "config.h"
#include "input.h"
#include "time.h"

//global structure that holds the global states of the engine
struct Global {
    struct Render_State render;
    struct Config_State config;
    struct Input_State  input;
    struct Time_State   time;
} ;

extern struct Global global;


#endif
