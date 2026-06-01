#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H

#define XCORNER(PERC) ((PERC) * global.render.width)
#define YCORNER(PERC) ((PERC)*global.render.height)

void showDebugStats(int level);

#endif
