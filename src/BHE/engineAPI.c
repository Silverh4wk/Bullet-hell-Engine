#include "engineAPI.h"

// extern vars decalration
SDL_Color colors[64] = {0};
int toggleHitBoxVisual = 0;
SDL_Event event;
Camera main_camera;
float fps_timer = 0.0f;
int   fps_frames = 0;
float fps_value = 0.0f;
bool global_running = true;
enum EngineStates current_state = STATE_OFF;

void
engineInit(void)
{
    //reminder to untie the engine to the fps (is set to 60 for now)
    timeInit(60);
    config_init();
    physicsInit();
    renderInit();
    InitEnginePools();
    patternSystemInit();
    camera_init(&main_camera);
    current_state = STATE_READY;
    
};

void engineLoadLevel( void );
void engineRun( void ) { engineSetState( STATE_INITIALIZING ); }
void enginePause( void ) { engineSetState( STATE_PAUSED ); }
void engineResume( void ) { engineSetState( STATE_RUNNING ); }

void engineShutdown() {
    //release controllers (if any)
    /* if (joystick) { */
    /*     SDL_CloseJoystick(joystick); */
    /* } */
    renderShutdown();
    patternSystemShutdown();
    SDL_DestroyWindow(global.render.window);
    SDL_Quit();
}


//its a global variable but this function is still neat to have
/////////////////////////////////////////////////////////////////
void engineSetState( int state ) { current_state = state; };
int engineGetState( void ) { return current_state; };
/////////////////////////////////////////////////////////////////

