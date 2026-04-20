#include <glad/glad.h>
#include "corecrt.h"
#include "helpers.h"

#include "SDL3/SDL.h"
#include <stdio.h>
#include "stdlib.h"
#include <linmath.h>

#include "keyboardTable.h"
#include "BHE/engineControls.h"

#define BODY_COUNT 5000
#define ENTITY_TEST_COUNT 1000

static struct Array_List* all_quads = NULL;

//(TESTING)
// Collision callback for player
void player_onCollision(struct Body* self, struct Body* other) {
    printf("Player hit by bullet!\n");
    other->active = false;   // destroy bullet
  
}

// Collision callback for bullets
void bullet_onCollision(struct Body* self, struct Body* other) {
    // Bullet hits player – handled by player callback, but safe to also destroy
    if (other->type == BODY_PLAYER) {
        self->active = false;
    }
}


static void spawn_bouncy_balls(int count) {
    for (int i = 0; i < count; i++) {
        vec2 pos = { rand() % global.render.width, rand() % global.render.height };
        vec2 size = { 8, 4 };
        vec4 color = { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1.0f };

        struct ShapeUnion ball = shapeCircleCreate(pos, size[1], &color, BODY_BULLET, true);
        struct Shape* q = ball.shape;
        if (!q || !q->body) continue;

        float angle = ((float)rand() / (float)RAND_MAX) * 2*PI; 
        float speed = 50.0f + (float)(rand() % 200);
        q->body->velocity[0] = cosf(angle) * speed;
        q->body->velocity[1] = sinf(angle) * speed;
        q->body->active = true;
        arrayListAppend(all_quads, &q);
    }
}


static float fps_timer = 0.0f;
static int   fps_frames = 0;
static float fps_value = 0.0f;

void fpsUpdate(void)
{
    fps_timer += global.time.delta;
    fps_frames++;

    if (fps_timer >= 1.0f)
    {
        fps_value = (float)fps_frames / fps_timer;

        fps_frames = 0;
        fps_timer = 0.0f;
    }
}

enum EngineState {
    STATE_MENU,
    STATE_JOYSTICK_TEST,
    STATE_SETTINGS,
    STATE_TERMINATE
// any more states can go here, still not sure how to do the editor part so
    // future me focus on that pls
};


// ==== Error Codes ====
//




//the init state
global_variable enum EngineState currentState = STATE_MENU;

global_variable SDL_Joystick * joystick = NULL;


global_variable vec2 pos;
global_variable vec2 qsize;
global_variable real32 angle;


//MOVE THIS (soon just wait) 
static void input_handle(void) {
    if (global.input.left == KEY_PRESSED || global.input.left == KEY_HELD)
    {
	pos[0] -= 500 * global.time.delta;
	angle += 0.1;
	printf("A \n");}
	
    if (global.input.right == KEY_PRESSED || global.input.right == KEY_HELD)
    {
	pos[0] += 500 * global.time.delta;
	printf("D \n");
    }    
    if (global.input.up == KEY_PRESSED || global.input.up == KEY_HELD)
    {
	pos [1] += 500* global.time.delta;
    	printf("W \n");
    }
    if (global.input.toggle == KEY_PRESSED)
    {
	toggleHitBoxVisual = !toggleHitBoxVisual;
    	printf("P \n");
    }
    
    if (global.input.down == KEY_PRESSED || global.input.down == KEY_HELD)
    {
	pos[1] -= 500 * global.time.delta;
    	printf("S \n");
    }
    if (global.input.hminus == KEY_PRESSED || global.input.hminus == KEY_HELD)
    {
	qsize[0] -= 5;
	printf("- \n");
    }
    if (global.input.hplus == KEY_PRESSED || global.input.hplus == KEY_HELD)
    {
	qsize[0] += 5;
	printf("+ \n");
    }
     if (global.input.escape == KEY_PRESSED || global.input.escape == KEY_HELD)
     {
	printf("program terminated... \n");
	GlobalRunning = 0;
    }
}

// ==========================
// (TODO) a proper scene setup
// (TODO) a general game intro screen,
// One with, start level, load level, configs and exit

void MenuScene(void);

void JoystickScene(void);


// ===  Main Window call back ===
//
// Processing of messages that are sent to the window
//
//
int main(int argc, char *argv[])
{
    engineInit();
   
    
    // Set engine state to running at the beginning
    for (int i = 0; i < ENTITY_TEST_COUNT; i++) {
        
        Entity e = entityCreate(i); 
        vec2 pos = {
            (float)(rand() % global.render.width),
            (float)(rand() % global.render.height)
        };
        float angle = 0.0f;
        entitySetTransform(e, &pos, &angle);
    }
    vec2 size = {10,10};
    all_quads = arrayListCreate(sizeof(struct Quad*), BODY_COUNT + 10);
    spawn_bouncy_balls(BODY_COUNT);
// (TODO) Figure out what to do with this later
    const char *screenText = "Plug in a joystick, please.";

    vec4 bulletColor = {1.0f, 0.0f, 1.0f, 1.0f}; 
    
    /* struct ShapeUnion player = shapeCircleCreate((vec2){720,720}, 64, NULL, BODY_PLAYER,true); */
    
    /* player.shape->body->onCollision = player_onCollision; */
    /* player.shape->body->active = true; */
    
    //main game loop
    while (GlobalRunning) {
	time_update();
	fpsUpdate();
	
// Scenes management section
	
	//    switch (currentState) {
	//        case STATE_MENU:
	//            {MenuScene();}
	//            break;
	//        case STATE_JOYSTICK_TEST:
	//            {JoystickScene();}
	//            break;
	//           {map editor}
	//           break;
	// 	 	//other scenes can go here
	//    }
	
	while (SDL_PollEvent(&event)) {
	    if (event.type == SDL_EVENT_QUIT) {
		GlobalRunning = false;
	    }
	    if (event.window.type == SDL_EVENT_WINDOW_RESIZED) {
		printf("MESSAGE:Resizing window...\n");
		SDL_GetWindowSize(global.render.window, &global.render.width, &global.render.height);
		SDL_UpdateWindowSurface(global.render.window);
	    }
	}
	
	//player.shape->data.quad.rotation_angle = angle;
	inputUpdate();
	input_handle();
	//testing camera panning
     if (global.input.vminus == KEY_PRESSED || global.input.vminus == KEY_HELD)
    {
        camera_set_offset(&main_camera, main_camera.offset[0], main_camera.offset[1] - 200.0f * global.time.delta);
	printf("- \n");
    }
    if (global.input.vplus == KEY_PRESSED || global.input.vplus == KEY_HELD)
    {
	camera_set_offset(&main_camera, main_camera.offset[0], main_camera.offset[1] + 200.0f * global.time.delta);
	printf("+ \n");
    }
	physicsUpdate();
	//broadPhaseResolve(); 
	//physicsRemoveInactiveBodies();
	//shapeMove(player.shape, pos[0],pos[1] );
	camera_update(&main_camera, global.time.delta);
	camera_apply(&main_camera);
	renderBegin();


	//renderSubmitShape(player.shape);
	
	// for testing, probably should put it under a flag 
	renderECS();
       renderEnd();
	//rendering block end

        if(toggleHitBoxVisual)
	    drawAllAABB();
	
	
        time_update_late();
    }
    
    engineShutdown();

    return 0;
}
// (REMINDER) Set for catchup with frames here, never tie the engine speed to the game fps


//switch to other files maybe later
void RenderMenu(void) {
    const char *title = "=== MAIN MENU ===";
    const char *option1 = "Press ENTER to Test Controller";
    const char *option2 = "Press ESC to Quit";

    SDL_SetRenderDrawColor(global.render.renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(global.render.renderer, 100, 100, title);
    SDL_RenderDebugText(global.render.renderer, 100, 140, option1);
    SDL_RenderDebugText(global.render.renderer, 100, 180, option2);
}

void MenuScene(void) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            GlobalRunning = 0;
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            // FIXED: check actual SDL key symbols
            if (event.key.key == Return) {
                currentState = STATE_JOYSTICK_TEST;
                return;
            } else if (event.key.key == Escape) {
                GlobalRunning = 0;
                return;
            }
        }
    }

    SDL_SetRenderDrawColor(global.render.renderer, 0, 0, 0, 255);
    SDL_RenderClear(global.render.renderer);
    SDL_SetRenderDrawColor(global.render.renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(global.render.renderer, (float)global.render.width / 2 - 100, (float)global.render.height / 2 - 40, "=== MAIN MENU ===");
    SDL_RenderDebugText(global.render.renderer, (float)global.render.width / 2 - 150, (float)global.render.height / 2, "Press ENTER to Test Controller");
    SDL_RenderDebugText(global.render.renderer,(float) global.render.width / 2 - 100,(float) global.render.height / 2 + 40, "Press ESC to Quit");
    SDL_RenderPresent(global.render.renderer);
}

// is it better to not have params or should i pass it for the sake of readability i wonder?
void JoystickScene(void) {

    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            GlobalRunning = 0;
        } else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == Escape) {
            currentState = STATE_MENU;
            return;
        } else if (event.type == SDL_EVENT_JOYSTICK_ADDED) {
            if (joystick == NULL) {
                joystick = SDL_OpenJoystick(event.jdevice.which);
                if (!joystick)
                    SDL_Log("Failed to open joystick ID %u: %s", (uint8_t) event.jdevice.which, SDL_GetError());
            }
        } else if (event.type == SDL_EVENT_JOYSTICK_REMOVED) {
            if (joystick && (SDL_GetJoystickID(joystick) == event.jdevice.which)) {
                SDL_CloseJoystick(joystick);
                joystick = NULL;
            }
        }
    }
    
    SDL_SetRenderDrawColor(global.render.renderer, 0, 0, 0, 255);
    SDL_RenderClear(global.render.renderer);
    if (joystick) {  /* we have a stick opened? */
	int i;
	float x, y;
	const float size = 30.0f;
	int total;
	
        /* draw axes as bars going across middle of screen.
	   We don't know if it's an X or Y or whatever axis, so we can't do more than this. */
        total = SDL_GetNumJoystickAxes(joystick);
	y = (global.render.height - (total * size)) / 2;
	x = ((float) global.render.width) / 2.0f;
        for (i = 0; i < total; i++) {
            const SDL_Color *color = &colors[i % SDL_arraysize(colors)];
            const float val = (((float) SDL_GetJoystickAxis(joystick, i)) / 32767.0f);  /* make it -1.0f to 1.0f */
            const float dx = x + (val * x);
            const SDL_FRect dst = { dx, y, x - SDL_fabsf(dx), size };
            SDL_SetRenderDrawColor(global.render.renderer, color->r, color->g, color->b, color->a);
            SDL_RenderFillRect(global.render.renderer, &dst);
            y += size;
        }

        /* draw buttons as blocks across top of window.
	   We only know the button numbers, but not where they are on the device. */
        total = SDL_GetNumJoystickButtons(joystick);
        x = (global.render.width - (total * size)) / 2;
        for (i = 0; i < total; i++) {
            const SDL_Color *color = &colors[i % SDL_arraysize(colors)];
            const SDL_FRect dst = { x, 0.0f, size, size };
            if (SDL_GetJoystickButton(joystick, i)) {
                SDL_SetRenderDrawColor(global.render.renderer, color->r, color->g, color->b, color->a);
            } else {
                SDL_SetRenderDrawColor(global.render.renderer, 0, 0, 0, 255);
            }
            SDL_RenderFillRect(global.render.renderer, &dst);
            SDL_SetRenderDrawColor(global.render.renderer, 255, 255, 255, color->a);
            SDL_RenderRect(global.render.renderer, &dst);  /* outline it */
            x += size;
        }

        /* draw hats across the bottom of the screen. */
        total = SDL_GetNumJoystickHats(joystick);
        x = ((global.render.width - (total * (size * 2.0f))) / 2.0f) + (size / 2.0f);
        y = ((float) global.render.height) - size;
        for (i = 0; i < total; i++) {
            const SDL_Color *color = &colors[i % SDL_arraysize(colors)];
            const float thirdsize = size / 3.0f;
            const SDL_FRect cross[] = { { x, y + thirdsize, size, thirdsize }, { x + thirdsize, y, thirdsize, size } };
            const Uint8 hat = SDL_GetJoystickHat(joystick, i);

            SDL_SetRenderDrawColor(global.render.renderer, 90, 90, 90, 255);
            SDL_RenderFillRects(global.render.renderer, cross, SDL_arraysize(cross));

            SDL_SetRenderDrawColor(global.render.renderer, color->r, color->g, color->b, color->a);

            if (hat & SDL_HAT_UP) {
                const SDL_FRect dst = { x + thirdsize, y, thirdsize, thirdsize };
                SDL_RenderFillRect(global.render.renderer, &dst);
            }

            if (hat & SDL_HAT_RIGHT) {
                const SDL_FRect dst = { x + (thirdsize * 2), y + thirdsize, thirdsize, thirdsize };
                SDL_RenderFillRect(global.render.renderer, &dst);
            }

            if (hat & SDL_HAT_DOWN) {
                const SDL_FRect dst = { x + thirdsize, y + (thirdsize * 2), thirdsize, thirdsize };
                SDL_RenderFillRect(global.render.renderer, &dst);
            }

            if (hat & SDL_HAT_LEFT) {
                const SDL_FRect dst = { x, y + thirdsize, thirdsize, thirdsize };
                SDL_RenderFillRect(global.render.renderer, &dst);
            }
	    
            x += size * 2;
        }}
    else {
        const char *screenText = "Plug in a joystick, please. (ESC to Menu)";
        float x = (((float)global.render.width) - (SDL_strlen(screenText) * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE)) / 2.0f;
        float y = (((float)global.render.height) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2.0f;
        SDL_SetRenderDrawColor(global.render.renderer, 255, 255, 255, 255);
        SDL_RenderDebugText(global.render.renderer, x, y, screenText);
    }

    SDL_RenderPresent(global.render.renderer);
}
