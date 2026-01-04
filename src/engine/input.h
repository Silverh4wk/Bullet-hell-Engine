#pragma once

typedef enum Input_Key {
    INPUT_KEY_DOWN,
    INPUT_KEY_UP,
    INPUT_KEY_LEFT,
    INPUT_KEY_RIGHT,
    INPUT_KEY_HPLUS,
    INPUT_KEY_HMINUS,
    INPUT_KEY_VPLUS,
    INPUT_KEY_VMINUS,
    INPUT_KEY_ESCAPE,
    INPUT_KEY_RETURN,
    INPUT_KEY_TOGGLE
} Input_Key;

typedef enum Key_State {
    KEY_HELD,
    KEY_RELEASED,
    KEY_PRESSED
}Key_State;

typedef struct Input_State {
    Key_State down;
    Key_State up;
    Key_State left;
    Key_State right;
    Key_State hminus;
    Key_State hplus;
    Key_State vminus;
    Key_State vplus;
    Key_State toggle;
    Key_State escape;
    Key_State _return;
    
}Input_State;

void input_update(void);
