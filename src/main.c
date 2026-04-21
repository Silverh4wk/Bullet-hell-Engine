#include <glad/glad.h>
#include "SDL3/SDL.h"
#include <stdio.h>
#include "stdlib.h"
#include "keyboardTable.h"
#include "helpers.h"
#include "BHE/engineAPI.h"

// these stays to help with testing 
global_variable SDL_Joystick * joystick = NULL;
global_variable vec2 pos;
global_variable vec2 qsize;
global_variable real32 angle;



//MOVE THIS (soon just wait) 
static void input_handle(void) {
    if (global.input.left == KEY_PRESSED || global.input.left == KEY_HELD)
    {
	pos[0] -= 500 * global.time.delta;
	angle += 0.1;
	printf("A \n");}
	
    if (global.input.right == KEY_PRESSED || global.input.right == KEY_HELD)
    {
	pos[0] += 500 * global.time.delta;
	printf("D \n");
    }    
    if (global.input.up == KEY_PRESSED || global.input.up == KEY_HELD)
    {
	pos [1] += 500* global.time.delta;
    	printf("W \n");
    }
    if (global.input.toggle == KEY_PRESSED)
    {
	toggleHitBoxVisual = !toggleHitBoxVisual;
	if ( engineGetState() != STATE_PAUSED )
	    engineSetState( STATE_PAUSED );
	else
	    engineSetState( STATE_RUNNING );
	printf("P \n");
    }
    
    if (global.input.down == KEY_PRESSED || global.input.down == KEY_HELD)
    {
	pos[1] -= 500 * global.time.delta;
    	printf("S \n");
    }
    if (global.input.hminus == KEY_PRESSED || global.input.hminus == KEY_HELD)
    {
	qsize[0] -= 5;
	printf("- \n");
    }
    if (global.input.hplus == KEY_PRESSED || global.input.hplus == KEY_HELD)
    {
	qsize[0] += 5;
	printf("+ \n");
    }
     if (global.input.escape == KEY_PRESSED || global.input.escape == KEY_HELD)
     {
	printf("program terminated... \n");
	global_running = false;
	engineSetState( STATE_OFF ) ;
    }
}


int main(int argc, char *argv[])
{
    engineInit();
    
    
    //main game loop
    while ( global_running ) {
	timeUpdate();
	
#ifdef DEBUG_MODE
	fpsUpdate();
#endif
	
	while (SDL_PollEvent(&event)) {
	    if (event.type == SDL_EVENT_QUIT) {
		engineSetState( STATE_OFF) ;
	    }
	    if (event.window.type == SDL_EVENT_WINDOW_RESIZED) {
		printf("MESSAGE:Resizing window...\n");
		SDL_GetWindowSize(global.render.window, &global.render.width, &global.render.height);
		SDL_UpdateWindowSurface(global.render.window);
	    }
	}

	inputUpdate();
	input_handle();
	
	if (engineGetState() != STATE_PAUSED )
	{
	    physicsUpdate();
	    //broadPhaseResolve(); 
	    //physicsRemoveInactiveBodies();
	}
	
	camera_update( &main_camera, global.time.delta );
	camera_apply( &main_camera );
	renderBegin();//rendering block begin
     	renderECS();
       renderEnd();  //rendering block end

#ifdef DEBUG_MODE
       if( toggleHitBoxVisual )
	   drawAllAABB();
#endif
       
       timeUpdateLate();
    }
    
    engineShutdown();

    return 0;
}

