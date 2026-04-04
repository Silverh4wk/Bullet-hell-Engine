#include "input.h"
#include "../helpers.h"
#include "global.h"

static void updateKeyState(uint8 current_state, Key_State *key_state)
{
    if(current_state)
    {
	if(*key_state > 0)
	    *key_state = KEY_HELD;
	else
	    *key_state = KEY_PRESSED;

    }
    else
	*key_state =KEY_RELEASED ;
}

void inputUpdate(void) {
    const bool *keyboard_state = SDL_GetKeyboardState(NULL);

    updateKeyState(keyboard_state[global.config.keybinds[INPUT_KEY_DOWN]],  &global.input.down);
    updateKeyState(keyboard_state[global.config.keybinds[INPUT_KEY_UP]],    &global.input.up);
    updateKeyState(keyboard_state[global.config.keybinds[INPUT_KEY_LEFT]],  &global.input.left);
    updateKeyState(keyboard_state[global.config.keybinds[INPUT_KEY_RIGHT]], &global.input.right);
    updateKeyState(keyboard_state[global.config.keybinds[INPUT_KEY_HPLUS]], &global.input.hplus);
    updateKeyState(keyboard_state[global.config.keybinds[INPUT_KEY_HMINUS]], &global.input.hminus);
    updateKeyState(keyboard_state[global.config.keybinds[INPUT_KEY_VPLUS]], &global.input.vplus);
    updateKeyState(keyboard_state[global.config.keybinds[INPUT_KEY_VMINUS]], &global.input.vminus);
    updateKeyState(keyboard_state[global.config.keybinds[INPUT_KEY_ESCAPE]],&global.input.escape);
    updateKeyState(keyboard_state[global.config.keybinds[INPUT_KEY_RETURN]],&global.input._return);
    updateKeyState(keyboard_state[global.config.keybinds[INPUT_KEY_TOGGLE]],&global.input.toggle);

};
