#include "../global.h"


void fpsUpdate(void)
{
    fps_timer += global.time.delta;
    fps_frames++;

    if (fps_timer >= 1.0f)
    {
        fps_value = (float)fps_frames / fps_timer;

        fps_frames = 0;
        fps_timer = 0.0f;
    }
}

void averageFPS() {};
void MaxFPS() {};
void MinFPS() {};
