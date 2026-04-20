#include "engineControls.h"

bool GlobalRunning = 0;
SDL_Color colors[64] = {0};
int toggleHitBoxVisual = 0;
SDL_Event event;
Camera main_camera;

void
engineInit(void) {
    GlobalRunning = 1;
    
    //reminder to untie the engine to the fps (is set to 60 for now)
    time_init(60);

    config_init();
    physicsInit();
    renderInit();
    InitEnginePools();
    camera_init(&main_camera);
    
};

void engineShutdown() {
    //release controllers (if any)
    /* if (joystick) { */
    /*     SDL_CloseJoystick(joystick); */
    /* } */
    
    // Quit
    renderShutdown();
    SDL_DestroyWindow(global.render.window);

    SDL_Quit();
}
