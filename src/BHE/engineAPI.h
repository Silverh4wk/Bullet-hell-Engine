#ifndef ENGINE_CONTROLS_H
#define ENGINE_CONTROLS_H

// Engine Includes

#include "../engine/render.h"
#include "../engine/render/render_internal.h"
#include "../engine/camera.h"
#include "../objects/shapes.h"
#include "../engine/global.h"
#include "../engine/config.h"
#include "../engine/input.h"
#include "../engine/time.h"
#include "../engine/physics.h"
#include "../engine/entity.h"
#include "../engine/ecs_internal.h"
#include "../engine/dataStructs.h" //temp name
#include "pattern.h"

SDL_Event event;              
Camera main_camera;
// update those voids to int for proper logging


enum EngineStates{
    STATE_INITIALIZING,
    STATE_READY,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_OFF,
};


enum EngineCodes {
    ENGINE_OK,
};


enum EngineScene {
    SCENE_MENU,
    SCENE_JOYSTICK_TEST,
    SCENE_SETTINGS,
    SCENE_TERMINATE
// any more states can go here, still not sure how to do the editor part so
    // future me focus on that pls
};


extern enum EngineStates current_state;

void engineInit(void);
void engineLoadLevel( void );
void engineRun( void );
void enginePause( void );
void engineResume( void );
void engineShutdown( void );
int  engineGetState( void );
void engineSetState( int state );











#endif
