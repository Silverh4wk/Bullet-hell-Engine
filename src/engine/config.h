#ifndef CONFIG_H
#define CONFIG_H

#include "../helpers.h"
#include "input.h"
#include <SDL3/SDL.h>


struct Config_State {
    SDL_Scancode keybinds[11];

} ;

// Initiate the engine configurations
// including :
// the rendering device config (TODO)
// sound config (TODO)
// input config (TODO)
void
config_init(void);

// binds a given key to its SDL scan code
void
config_key_bind(Input_Key key, const char *key_name);

#endif 
