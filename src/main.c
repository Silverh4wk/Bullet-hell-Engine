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



    vec4 COL_CRIMSON = {1.0f, 0.1f, 0.2f, 1.0f};
    vec4 COL_WHITE   = {1.0f, 1.f, 1.f, 1.0f};
    vec4 COL_GOLD    = {1.0f, 0.85f, 0.0f, 1.0f};
    vec4 COL_VIOLET  = {0.7f, 0.2f, 1.0f, 1.0f};
    vec4 COL_CYAN    = {0.1f, 0.8f, 1.0f, 1.0f};
    vec4 COL_ORANGE  = {1.0f, 0.5f, 0.0f, 1.0f};
    vec4 COL_MAGENTA = {1.0f, 0.0f, 0.7f, 1.0f};

    //initiate an entity
    //Build the entity
    //set its data
    // -----------------------------------------------
    // 1. create bullet types
    // -----------------------------------------------
    GLuint texBulletRed  = renderLoadTexture("I:/FYP/src/assets/cat.png");
    GLuint texBulletBlue = renderLoadTexture("I:/FYP/src/assets/bullet_texture.png");

    uint32 btRed   = bulletTypeCreate((vec4){1,1,1,1}, (vec2){130,120}, 100, 5, texBulletRed); 
//    uint32 btRed   = bulletTypeCreate((vec4){1,0,0.2,1}, (vec2){30,20}, 100, 5, 0); 

    uint32 btBlue  = bulletTypeCreate((vec4){1,1,1,1}, (vec2){120,130}, 200, 5,texBulletBlue);   
//    uint32 btBlue  = bulletTypeCreate((vec4){1,1,1,1}, (vec2){20,30}, 200, 5,0);   
    Entity spawner1 = entityInit(BODY_BULLET); 
    
    
    // -----------------------------------------------
    // 2. create patterns
    // -----------------------------------------------
    // --- Phase 1 --
    uint32 phase1 = PatternCreate();
    PatternSetBulletType(phase1, btRed);
    PatternSetTimingFunction(phase1, TIMING_EASE_IN);
    PatternSetRotation(phase1, 90.0f);
    PatternAddCircle(phase1, 36, 220.0f, 0.02f);
    PatternSetRepeats(phase1, 2);
    
    // -----------------------------------------------
    // 3. Spawn test "enemy" entities 
    // -----------------------------------------------
  
    entitySetTransform(spawner1,
                       &(vec2){(real32)global.render.width/2, (real32)global.render.height/2},
                       &(vec2){30,30}, NULL);

    entityBuild(spawner1, SHAPE_CIRCLE);

    entitySetColor(spawner1, (vec4){1,0.4,1,1});

   struct BulletSpawner s1 = { .pattern_id = phase1 };
   
   ComponentAttach(spawner1, COMPONENT_BULLET_SPAWNER, &s1);
   
   
    // "player" entity 
    Entity player = entityInit(BODY_PLAYER);
    entitySetTransform(player,
                       &(vec2){600, 400},
                       &(vec2){20,20}, NULL);
    entityBuild(player, SHAPE_QUAD);
    entitySetColor(player, (vec4){1,1,1,1});
    patternSetPlayerEntity(player);

    
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
	    //broadPhaseResolve(); 
	    //physicsRemoveInactiveBodies();
	    shapeMove(g_shapes[player].shape, pos[0], pos[1]);
	    entitySetTransform(player, &pos, NULL, NULL); 
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
	
	if (engineGetState() != STATE_PAUSED )
	{
	    timeUpdateLate();
	}
    }
    engineShutdown();

    return 0;
}

