# FYP

Backup repository for a Final Year Project (FYP) — currently under development.

Status: **Under Construction**

## Description
A specialised bullet-hell game engine / toolkit that is optimized to handle large projectile counts  and provide the necessary tools for creating a bullet-hell game.

## Key Technologies
- Primary language: C
- External Libraries: OpenGl,Sdl

## Features (Work-in-progress)
- Gameplay Loop
- input system
- rendering system : instance rendering , Camera System , objects (quad / circle ) drawing
- partial physics system : broadphase and narrow phase (using spatial hashing as the resolver)
- entty/object system
- ..... to be added

![](instanceRendering.mkv)
![](circles.mkv)
![](camera.mkv)
![](input.mkv)

## Build 
- I use a batch file for building the project.
- The engine is compiled using the microsft compiler as I use studio debugger for debugging.
- If u want you can try running build.bat and hopefully it does build the project (the current state its in is purely for testing and building what feature been added)

## Running
- navigate to the `Build` dir
- execute the BHE.exe  

## Project Structure (suggested)
├── engine/                 # Core engine systems
│   ├── dataStructs/        # Custom data structures 
│   ├── physics/            # Physics system 
│   ├── render/             # Rendering system 
│   │   └── camera/         # Camera-specific logic
│   ├── Profiling/          # Performance tools 
│   ├── *.c / *.h           # Core engine modules 
│
├── objects/                # Game objects and shape definitions
│   ├── shapes.c
│   └── shapes.h
│
├── shaders/                # GLSL shader files
│   ├── default.vert
│   └── default.frag
│
├── io/                     # File I/O utilities
│   ├── io.c
│   └── io.h
│
├── Util/                   # Utility modules 
│   ├── PCG_RNG.c
│   └── cpu.h
│
├── main.c                  # Entry point / game loop
├── helpers.c / helpers.h   # General helper functions
├── glad.c                  # OpenGL loader
├── keyboardTable.h         # Input key mapping
├── types.h                 # Shared type definitions
│
├── todo.org                #  TODOs


## License
---

---

## Current TODO
---
# TODO Rendering system

- [x] Remove per frame malloc/realloc in batching  
- [x] Stop orphaning instance VBO every draw  
- [x] Add renderShutdown() to free batch instance  
- [x] Add rotation support to Quad  
- [x] Fix renderAABB to work without breaking batched state (draw after batching or as line)  
- [x] Add circle rendering  
- [x] Add camera/view transform  
- [ ] Add Background rendering for levels  
- [ ] Add layer/Z ordering  
- [ ] Implement persistent mapped buffer for high bullet counts  
- [ ] Add simple text output (bitmap font via instanced quads) for debug  
- [ ] Add frustum culling to skip off screen  

# TODO Window events

## General

- [ ] Handle Resize  
- [ ] Fix the hitbox visualization rendering issues  

- [x] Store updated width/height in a global or struct `WindowDimensions`  
- [x] Trigger a renderer update (recalculate projection, viewport, etc.)  
- [x] Handle Focus / Minimize (what about borderless too?)  

- [ ] Capture `WM_ACTIVATEAPP` / `SDL_EVENT_WINDOW_FOCUS_GAINED` / LOST  
- [ ] Pause the game/editor loop if minimized or unfocused  
- [ ] Handle Close and saving of game data (don’t have a game yet)  


## Input

- [ ] Keyboard events (`wasDown` and `isDown` should be properly handled and logged)  
- [ ] Mouse input   
- [ ] Gamepad support (handle hotplugging properly)  


# TODO Bullet counts

- [ ] Render instancing for bullet meshes (one draw call for at least 100 bullets)  
- [ ] Pool allocate memory for bullets (don’t delete unless the stage is finished)  
---
