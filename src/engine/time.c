#include <stdio.h>
#include <SDL3/SDL.h>

#include "time.h"
#include "global.h"

void
timeInit(uint32 frame_rate) {
    global.time.frame_rate = frame_rate;
    global.time.frame_delay = 1000.0f/ frame_rate;
};

//to be called at the beggining of each frame
void
timeUpdate(void) {
    char buffer[1024];
    global.time.now = (real32)SDL_GetTicks();
    global.time.delta = (global.time.now - global.time.last) / 1000.0f;
    global.time.last = global.time.now;
    ++global.time.frame_count;

}


void
timeUpdateLate(void) {
    global.time.frame_time = (real32)SDL_GetTicks() - global.time.now;
    //if the frame finished faster than the targetted framerate
    //delay it 
    if(global.time.frame_delay> global.time.frame_time)
	SDL_Delay(global.time.frame_delay - global.time.frame_time); 
}

real32 getDeltaTime( void ) { return global.time.delta;};
