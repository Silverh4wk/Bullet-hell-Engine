#pragma once
#include "../helpers.h"

typedef struct Time_State {

    real32 delta;// the time between frames
    real32 now; // the time of the current frame
    real32 last;// the time of the last frame

    real32 frame_last;  // the last time the fps was calc
    real32 frame_delay; // the maximum amount of time a frame should take (to play catchup)
    real32 frame_time; // how long the current frame took

    uint32 frame_rate;
    uint32 frame_count;
}Time_State;


void time_init(uint32 frame_rate);
void time_update(void);
void time_update_late(void);
