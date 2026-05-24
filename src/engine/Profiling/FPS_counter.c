#include "../global.h"
#include "../../Util/colors.h"

#define XCORNER  0.02 * global.render.width
#define YCORNER  0.02 * global.render.height

char fps_buffer[256];

void fpsUpdate(void)
{
    
    renderText(fps_buffer,XCORNER,YCORNER,&COL_GREEN);
    if(global.time.now - global.time.frame_last >= 1000.f)
    {
	global.time.frame_rate = global.time.frame_count;
	sprintf(fps_buffer,"FPS: %d", global.time.frame_count);
	global.time.frame_count = 0;
	global.time.frame_last = global.time.now;
    }
}

void averageFPS() {};
void MaxFPS() {};
void MinFPS() {};
