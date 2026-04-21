//temporary storage for these functions

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
