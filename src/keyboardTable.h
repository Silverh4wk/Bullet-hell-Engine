

/*
  keyboard rules to remember
  - Do not use key events for typing purposes and what not
  - offer keybindings whenpossible, u dont know what someones keyboard layout looks like
  - still not sure if i want to use the 101 button joystick approach or not (if so use the SDL_SCANCODE_W)
  - implement more of those "press [current keybinding] to jump"
  
 */
// ==== Keyboard Table ====

// Control Keys
#define Escape        SDLK_ESCAPE
#define BackSpace     SDLK_BACKSPACE
#define Tab           SDLK_TAB
#define Return        SDLK_RETURN
#define Enter         SDLK_RETURN
#define Pause         SDLK_PAUSE
#define ScrollLock    SDLK_SCROLLLOCK
#define SysReq        SDLK_SYSREQ
#define Clear         SDLK_CLEAR

// Navigation Keys
#define Home          SDLK_HOME
#define End           SDLK_END
#define PageUp        SDLK_PAGEUP
#define PageDown      SDLK_PAGEDOWN
#define Insert        SDLK_INSERT
#define Delete        SDLK_DELETE

// Arrow Keys
#define RightArrow    SDLK_RIGHT
#define LeftArrow     SDLK_LEFT
#define DownArrow     SDLK_DOWN
#define UpArrow       SDLK_UP

// Modifiers
#define LeftShift     SDLK_LSHIFT
#define RightShift    SDLK_RSHIFT
#define LeftCtrl      SDLK_LCTRL
#define RightCtrl     SDLK_RCTRL
#define LeftAlt       SDLK_LALT
#define RightAlt      SDLK_RALT
#define LeftMeta      SDLK_LGUI
#define RightMeta     SDLK_RGUI
#define CapsLock      SDLK_CAPSLOCK
#define NumLock       SDLK_NUMLOCKCLEAR

// Function Keys
#define F1            SDLK_F1
#define F2            SDLK_F2
#define F3            SDLK_F3
#define F4            SDLK_F4
#define F5            SDLK_F5
#define F6            SDLK_F6
#define F7            SDLK_F7
#define F8            SDLK_F8
#define F9            SDLK_F9
#define F10           SDLK_F10
#define F11           SDLK_F11
#define F12           SDLK_F12
#define F13           SDLK_F13
#define F14           SDLK_F14
#define F15           SDLK_F15
#define F16           SDLK_F16
#define F17           SDLK_F17
#define F18           SDLK_F18
#define F19           SDLK_F19
#define F20           SDLK_F20
#define F21           SDLK_F21
#define F22           SDLK_F22
#define F23           SDLK_F23
#define F24           SDLK_F24

// Number Keys (Top Row)
#define Num0          SDLK_0
#define Num1          SDLK_1
#define Num2          SDLK_2
#define Num3          SDLK_3
#define Num4          SDLK_4
#define Num5          SDLK_5
#define Num6          SDLK_6
#define Num7          SDLK_7
#define Num8          SDLK_8
#define Num9          SDLK_9

// Alphabet Keys
#define A             SDLK_a
#define B             SDLK_b
#define C             SDLK_c
#define D             SDLK_d
#define E             SDLK_e
#define F             SDLK_f
#define G             SDLK_g
#define H             SDLK_h
#define I             SDLK_i
#define J             SDLK_j
#define K             SDLK_k
#define L             SDLK_l
#define M             SDLK_m
#define N             SDLK_n
#define O             SDLK_o
#define P             SDLK_p
#define Q             SDLK_q
#define R             SDLK_r
#define S             SDLK_s
#define T             SDLK_t
#define U             SDLK_u
#define V             SDLK_v
#define W             SDLK_w
#define X             SDLK_x
#define Y             SDLK_y
#define Z             SDLK_z

// Punctuation & Symbols
#define Space         SDLK_SPACE
#define Exclaim       SDLK_EXCLAIM        // !
#define QuoteDbl      SDLK_QUOTEDBL       // "
#define Hash          SDLK_HASH           // #
#define Dollar        SDLK_DOLLAR         // $
#define Percent       SDLK_PERCENT        // %
#define Ampersand     SDLK_AMPERSAND      // &
#define Quote         SDLK_QUOTE          // '
#define LeftParen     SDLK_LEFTPAREN      // (
#define RightParen    SDLK_RIGHTPAREN     // )
#define Asterisk      SDLK_ASTERISK       // *
#define Plus          SDLK_PLUS           // +
#define Comma         SDLK_COMMA          // ,
#define Minus         SDLK_MINUS          // -
#define Period        SDLK_PERIOD         // .
#define Slash         SDLK_SLASH          // /
#define Colon         SDLK_COLON          // :
#define SemiColon     SDLK_SEMICOLON      // ;
#define Less          SDLK_LESS           // <
#define Equals        SDLK_EQUALS         // =
#define Greater       SDLK_GREATER        // >
#define Question      SDLK_QUESTION       // ?
#define At            SDLK_AT             // @
#define LeftBracket   SDLK_LEFTBRACKET    // [
#define BackSlash     SDLK_BACKSLASH      // \/ heh looks like solair \[T]/
#define RightBracket  SDLK_RIGHTBRACKET   // ]
#define Caret         SDLK_CARET          // ^
#define Underscore    SDLK_UNDERSCORE     // _
#define BackQuote     SDLK_BACKQUOTE      // `

// Keypad (Numpad)
#define KP0           SDLK_KP_0
#define KP1           SDLK_KP_1
#define KP2           SDLK_KP_2
#define KP3           SDLK_KP_3
#define KP4           SDLK_KP_4
#define KP5           SDLK_KP_5
#define KP6           SDLK_KP_6
#define KP7           SDLK_KP_7
#define KP8           SDLK_KP_8
#define KP9           SDLK_KP_9
#define KPDivide      SDLK_KP_DIVIDE
#define KPMultiply    SDLK_KP_MULTIPLY
#define KPMinus       SDLK_KP_MINUS
#define KPPlus        SDLK_KP_PLUS
#define KPEnter       SDLK_KP_ENTER
#define KPPeriod      SDLK_KP_PERIOD
#define KPEquals      SDLK_KP_EQUALS
#define KPComma       SDLK_KP_COMMA
#define KPLeftParen   SDLK_KP_LEFTPAREN
#define KPRightParen  SDLK_KP_RIGHTPAREN

// Multimedia & Misc
#define PrintScreen   SDLK_PRINTSCREEN
#define Application   SDLK_APPLICATION
#define Power         SDLK_POWER
#define Menu          SDLK_MENU
#define Help          SDLK_HELP
#define Undo          SDLK_UNDO
#define Cut           SDLK_CUT
#define Copy          SDLK_COPY
#define Paste         SDLK_PASTE
#define Find          SDLK_FIND
#define Mute          SDLK_MUTE
#define VolumeUp      SDLK_VOLUMEUP
#define VolumeDown    SDLK_VOLUMEDOWN


//yea remind me to move those over to somewhere else
// ==== Keyboard Table ====

