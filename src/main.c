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
	toggleHitBoxVisual = !toggleHitBoxVisual;
	printf("- \n");
    }
    if (global.input.hplus == KEY_PRESSED || global.input.hplus == KEY_HELD)
    {
	
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



    vec4 COL_CRIMSON = {1.0f, 0.1f, 0.2f, 1.0f};
    vec4 COL_WHITE   = {1.0f, 1.f, 1.f, 1.0f};
    vec4 COL_GOLD    = {1.0f, 0.85f, 0.0f, 1.0f};
    vec4 COL_VIOLET  = {0.7f, 0.2f, 1.0f, 1.0f};
    vec4 COL_CYAN    = {0.1f, 0.8f, 1.0f, 1.0f};
    vec4 COL_ORANGE  = {1.0f, 0.5f, 0.0f, 1.0f};
    vec4 COL_MAGENTA = {1.0f, 0.0f, 0.7f, 1.0f};

    // "player" entity 
    Entity player = entityInit(BODY_PLAYER);
    entitySetTransform(player,
                       &(vec2){600, 400},
                       &(vec2){100,100}, NULL);
    entityBuild(player, SHAPE_QUAD, true);
    entitySetColor(player, COL_CYAN);
    camera_follow(&main_camera, pos, 5.0f);

    /* //"enemy" entity */
    /* Entity enemy = entityInit(BODY_ENEMY); */
    /* entitySetTransform(enemy, */
    /*                    &(vec2){700, 400}, */
    /*                    &(vec2){100,100}, NULL); */
    /* entityBuild(enemy, SHAPE_CIRCLE, true); */
    /* entitySetColor(enemy, COL_CRIMSON); */

    
    //main game loop
    while ( global_running ) {
	if (engineGetState() != STATE_PAUSED )
	{
	    timeUpdate();
	}
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
	    patternSystemUpdate(global.time.delta);
	    bulletSystemUpdate(global.time.delta);
	    physicsUpdate();
	    shapeMove(g_shapes[player].shape, pos[0], pos[1]);
	    entitySetTransform(player, &pos, NULL, NULL); 
	}
	
	camera_update( &main_camera, global.time.delta );
	camera_apply( &main_camera );
	renderBegin();//rendering block begin
	renderECS();
	
#ifdef DEBUG_MODE
	drawAllAABB(toggleHitBoxVisual);
#endif
	
	renderEnd();  //rendering block end

	
	if (engineGetState() != STATE_PAUSED )
	{
	    timeUpdateLate();
	}
    }
    engineShutdown();

    return 0;
}

