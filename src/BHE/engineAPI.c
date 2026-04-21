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
    current_state = STATE_INITIALIZING;
    //reminder to untie the engine to the fps (is set to 60 for now)
    time_init(60);
    config_init();
    physicsInit();
    renderInit();
    InitEnginePools();
    camera_init(&main_camera);
    current_state = STATE_READY;
};

void engineShutdown() {
    //release controllers (if any)
    /* if (joystick) { */
    /*     SDL_CloseJoystick(joystick); */
    /* } */
    renderShutdown();
    SDL_DestroyWindow(global.render.window);
    SDL_Quit();
}

void engineLoadLevel( void );
void engineRun( void );
void enginePause( void );
void engineResume( void );

//its a global variable but this function is still neat to have
/////////////////////////////////////////////////////////////////
void engineSetState( int state ) { current_state = state; };
int engineGetState( void ) { return current_state; };
/////////////////////////////////////////////////////////////////
void TimeInit( void );
void getDeltaTime( void );
