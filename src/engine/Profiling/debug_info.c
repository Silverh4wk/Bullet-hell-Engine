#include "../debug_info.h"
#include "../render.h"
#include "../global.h"
#include "../../Util/colors.h"
#include "../FPS_counter.h"

static inline
void glDebugInfo(void) {
    renderText(debug_info0, XCORNER(0.02), YCORNER(0.04), &COL_GREEN);  
}
void showDebugStats(int level)
{
    switch (level) {
    case 1 :
	fpsUpdate();
	break;
    case 2 :
	fpsUpdate();
	glDebugInfo();
	    break;
    case 3 :
	    break;
    default:
	;}
    
}
