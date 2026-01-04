
#include <glad/glad.h>
#include "../global.h"
#include "../render.h"
#include "render_internal.h"

global_variable Render_State_Internal state = {0};


void renderInit(void)
{
 //the following attrb must be set before creating a window
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
    int i ;
    float x, y;
    
    // init video and display mode... also controller
    if (!(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK))) {
        ERROR_EXIT("Failed to SDL_Init : %s\n", SDL_GetError());
    }
	
    global.render.displayID = SDL_GetPrimaryDisplay();
    if (global.render.displayID == 0) {
	ERROR_EXIT("Failed to get primary display: %s\n", SDL_GetError());
	
    }

    SDL_Rect displayBounds;
    if (SDL_GetDisplayBounds(global.render.displayID, &displayBounds)) {
	SDL_Log("Display bounds: %dx%d", displayBounds.w, displayBounds.h);
	global.render.width  = displayBounds.w;
	global.render.height = displayBounds.h;
    } else {
	ERROR_EXIT("Failed to get display bounds: %s", SDL_GetError());
	global.render.width = 800;
	global.render.height = 600;
    }

    global.render.window = renderInitWindow(global.render.width,global.render.height);
    
    
    renderInitQuad(&state.vao_quad,&state.vbo_quad, &state.ebo_quad);
    renderInitShaders(&state);
    renderInitColorTexture(&state.texture_color);
    };

// 3 buffers make up what u see: color, depth and the stencil buffer (google those) 
void renderBegin(void) {
    glClearColor(0.08,0.1,0.1,1);
    glClear(GL_COLOR_BUFFER_BIT);
};
void renderEnd(void) {
    SDL_GL_SwapWindow(global.render.window);   
};
void renderQuad(Quad quad) {
    glUseProgram(state.shader_default);

    mat4x4 model;
    mat4x4_identity(model);

    mat4x4_translate(model,quad.pos[0], quad.pos[1],0);
    mat4x4_scale_aniso(model,model,quad.size[0], quad.size[1],1);

    glUniformMatrix4fv(glGetUniformLocation(state.shader_default,"model"),
			    1,
			    GL_FALSE,
			    &model[0][0]);
    glUniform4fv(glGetUniformLocation(state.shader_default, "color"),1,quad.color);

    glBindVertexArray(state.vao_quad);
    glBindTexture(GL_TEXTURE_2D,state.texture_color);

    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT, NULL);
    
    glBindVertexArray(0);
};



void renderQuad(vec2 pos, vec2 size, vec4 color) {
    glUseProgram(state.shader_default);

    mat4x4 model;
    mat4x4_identity(model);

    mat4x4_translate(model,pos[0], pos[1],0);
    mat4x4_scale_aniso(model,model,size[0], size[1],1);

    glUniformMatrix4fv(glGetUniformLocation(state.shader_default,"model"),
			    1,
			    GL_FALSE,
			    &model[0][0]);
    glUniform4fv(glGetUniformLocation(state.shader_default, "color"),1,color);

    glBindVertexArray(state.vao_quad);
    glBindTexture(GL_TEXTURE_2D,state.texture_color);

    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT, NULL);
    
    glBindVertexArray(0);
};

void renderAABB(AABB* aabb, vec4 color) {
    if (!aabb) return;

    glUseProgram(state.shader_default);

    //  full size from half_size
    float w = aabb->half_size[0] * 2.0f;
    float h = aabb->half_size[1] * 2.0f;

    mat4x4 model;
    mat4x4_identity(model);
    mat4x4_translate(model, aabb->position[0], aabb->position[1], 0);
    mat4x4_scale_aniso(model, model, w, h, 1);

    glUniformMatrix4fv(glGetUniformLocation(state.shader_default, "model"),
                       1, GL_FALSE, &model[0][0]);
    glUniform4fv(glGetUniformLocation(state.shader_default, "color"), 1, color);

    glBindVertexArray(state.vao_quad);
    glBindTexture(GL_TEXTURE_2D, state.texture_color);

    // draw the outline of the quad using GL_LINE 
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(0);
}

void drawAllAABB(void) {
    size_t count = physicsGetBodyCount();
    vec4 wireColor = {1,0,0,1};
    for (size_t i = 0; i < count; i++) {
        Body* body = physicsGetBody(i);
	renderAABB(&body->aabb,wireColor);
        
    }
}
