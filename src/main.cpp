#include <glad/glad.h>
#include "corecrt.h"
#include "helpers.h"

#include "SDL3/SDL.h"
#include <stdio.h>
#include <iostream>

#include "keyboardTable.h"

#include "engine/render.h"
#include "math.h"
#include "objects/shapes.h"

#include "engine/global.h"
#include "engine/config.h"
#include "engine/input.h"
#include "engine/time.h"
#include "engine/physics.h"
#include "stdlib.h"




global_variable SDL_Joystick * joystick = NULL;

global_variable int toggleHitBoxVisual = 0;

enum EngineState {
    STATE_MENU,
    STATE_JOYSTICK_TEST,
    STATE_SETTINGS,
    STATE_TERMINATE
// any more states can go here, still not sure how to do the editor part so
    // future me focus on that pls
};

//the init state
global_variable EngineState currentState = STATE_MENU;

// ==== Error Codes ====
//


// ==== Global Variables ====
//

global_variable bool GlobalRunning = 0; // The state of the application
global_variable SDL_Color colors[64];

global_variable vec2 pos;
global_variable vec2 qsize;

static void input_handle(void) {
    if (global.input.left == KEY_PRESSED || global.input.left == KEY_HELD)
    {
	pos[0] -= 500 * global.time.delta;
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
     if (global.input.vminus == KEY_PRESSED || global.input.vminus == KEY_HELD)
    {
	qsize[1] -= 5;
	printf("- \n");
    }
    if (global.input.vplus == KEY_PRESSED || global.input.vplus == KEY_HELD)
    {
	qsize[1] += 5;
	printf("+ \n");
    }
     if (global.input.escape == KEY_PRESSED || global.input.escape == KEY_HELD)
     {
	printf("program terminated... \n");
	GlobalRunning = 0;
    }
}

// ==========================
void MenuScene(void);
void JoystickScene(void);

void Terminate();

// ===  Main Window call back ===
//
// Processing of messages that are sent to the window
//
//

int main(int argc, char *argv[])
{
    //reminder to untie the engine to the fps (is set to 60 for now)
    time_init(90);
    config_init();
    SDL_Event event;
    renderInit();
    physicsInit();

    int body_count = pow(2.f,8.f);
    
#define testrand (float)rand()/RAND_MAX
    
	for (int i = 0; i < body_count; ++i)
	{
	      size_t body_index = physicsBodyCreate(
		  vec2{static_cast<float>((rand() % (int)global.render.width)), 
		     (float)(rand() % (int)global.render.height) },
		 vec2{5, 
		      5 },
		  vec4{testrand,testrand,testrand,testrand}
		);

	    Body* body = physicsBodyGet(body_index);
	    body->acceleration[0] = rand() % 200 - 100;
	    body->acceleration[1] = rand() % 200 - 100;
 }
		 
    Quad quad1;
    pos[0] = global.render.width * 0.5f;
    pos[1] = global.render.height * 0.25f;
    qsize[0] =  global.render.height * 0.05;
    qsize[1] =  global.render.width * 0.05f;
    vec2 testSize = {50,50};
    vec4 testColor = {0,1,0,1};
    
    QuadCreate(&quad1,qsize, pos,testColor);
    // quad1->body->aabb.radius[0] =sqrt(pow(quad1->body->aabb.half_size[0],2) + pow(quad1->body->aabb.half_size[1],2)) ;
    //quad1->body->aabb.radius[1] = quad1->body->aabb.radius[0];
    int i;
    
 
    GlobalRunning = 1;

    const char *screenText = "Plug in a joystick, please.";
    // SDL_SetRenderDrawColor(global.render.renderer, 0, 0, 0, 255);
    // SDL_RenderClear(global.render.renderer);

   
    //main loop

    /*
      SDL_PollEvent() ===> This function removes the first event from the queue.
      Copying the value into a parameter of type SDL_Event.
      If the event queue was empty, the function will return 0.
    */
    
    while (GlobalRunning) {
	time_update();
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
	
	QuadMove(&quad1, pos[0], pos[1]);
	//QuadChangeSize(&quad1, qsize[0],qsize[1]);
	while (SDL_PollEvent(&event)) {
	    if (event.type == SDL_EVENT_QUIT) {
		GlobalRunning = false;
	    } 
	}
	input_update();
	input_handle();
	physicsUpdate();
	renderBegin();
	if(toggleHitBoxVisual)
	    drawAllAABB();
	renderQuad(quad1.pos,quad1.size,quad1.color);
	
	for(int x = 0; x <body_count;++x)
	{
	    Body *body = physicsBodyGet(x);
	    
	    renderQuad(body->aabb.position,body->aabb.half_size,body->color);

	    //wall and ceiling bounce
	     if (body->aabb.position[0] > global.render.width || body->aabb.position[0] < 0)
	    {body->velocity[0] *= -2;
		//	setVec4(&body->color,testrand,testrand,testrand, testrand);
	    }
	      if (body->aabb.position [1] > global.render.height || body->aabb.position[1] < 0)
		  body->velocity[1] *= -2;
	    //if(testAABBAABB(quad1.body->aabb,body->aabb))
	    //{setVec4(&body->color,1.f,1.f,0.f,1.f);}
	    // else
	    //	setVec4(&body->color,2.f,0.f,1.f,0.f);
	    
	    if (body->velocity [0] > 500)
		body->velocity[0] = 500;
	    if (body->velocity [0] < -500)
		body->velocity[0] = -500;
	    if (body->velocity [1] > 500)
		body->velocity[1] = 500;
	    if (body->velocity [1] < -500)
		body->velocity[1] = -500;
	}
	
	renderEnd();
	time_update_late();
    }
    
    Terminate();
    return 0;
}
// (REMINDER) Set for catchup with frames here, never tie the engine speed to the game fps

void Terminate() {
    //release controllers (if any)
    if (joystick) {
        SDL_CloseJoystick(joystick);
    }
    // Quit
    SDL_DestroyWindow( global.render.window);
    SDL_Quit();
}


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
