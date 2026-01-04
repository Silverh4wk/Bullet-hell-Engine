#pragma once
#include "../helpers.h"
#include "input.h"
#include <SDL3/SDL.h>

typedef struct Config_State {
    SDL_Scancode keybinds[11];

} Config_State;

void config_init(void);
void config_key_bind(Input_Key key, const char *key_name);
