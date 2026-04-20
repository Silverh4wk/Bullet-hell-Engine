#ifndef ENGINE_CONTROLS_H
#define ENGINE_CONTROLS_H

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


SDL_Event event;              
Camera main_camera;

// update those voids to int for proper logging

typedef enum {
  ENGINE_OK,
} EngineCodes;



typedef enum {
  ENGINE_RUNNING,
  ENGINE_PAUSED,
} EngineStates;


void engineInit(void);
void engineLoadLevel( void );
void engineRun( void );
void enginePause( void );
void engineResume( void );
void engineShutdown( void );
void engineGetState( void );
void TimeInit( void );
void getDeltaTime( void );










#endif
