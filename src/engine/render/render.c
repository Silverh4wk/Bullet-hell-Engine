#include <glad/glad.h>
#include <SDL3/SDL.h>
#include "../global.h"
#include "../render.h"
#include "render_internal.h"
#include "../global.h"
#include "string.h"

#define MAX_BATCHES 64
#define MAX_INSTANCES_PER_BATCH 100000
    
static struct RenderStateInternal state = {0};

//ring buffer 
static struct InstanceData* instance_buffer = NULL;
static size_t instance_capacity = 0;
static size_t instance_used = 0;


static void
beginFrame(void) { instance_used = 0; }

static struct InstanceData*
allocateInstances(size_t count) {
    // Grow the buffer (if it ever happen or is needed)
    if (instance_used + count > instance_capacity) {
        size_t new_cap = (instance_used + count) * 2;
        instance_buffer = realloc(instance_buffer, new_cap * sizeof(struct InstanceData));
        instance_capacity = new_cap;
    }
    
    struct InstanceData* ptr = &instance_buffer[instance_used];
    instance_used += count;
    return ptr;
}


static void
renderInitBatches(int max_batches, int instances_per_batch) {
    state.batchCount = 0;
    for (int i = 0; i < max_batches; i++) {
        state.batches[i].instances = malloc(instances_per_batch * sizeof(struct InstanceData));
        state.batches[i].capacity = instances_per_batch;
        state.batches[i].count = 0;
        state.batches[i].texture = 0;
    }
}


void
renderInit(void)
{
    renderInitBatches(MAX_BATCHES, MAX_INSTANCES_PER_BATCH);
    //the following attrb must be set before creating a window
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
    
    // init video and display mode... also controller
    if (!(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO | SDL_INIT_JOYSTICK))) {
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
	ERROR_EXIT("Failed to get display bounds: %s", SDL_GetError())
	global.render.width = 800;
	global.render.height = 600;
    }

    global.render.window = renderInitWindow(global.render.width,global.render.height);
    
    
    renderInitQuad(&state,&state.vao_quad,&state.vbo_quad,&state.ebo_quad);
    renderInitShaders(&state);
    renderInitColorTexture(&state.texture_color);
};





    // 3 buffers make up what u see: color, depth and the stencil buffer 
void
renderBegin(void) {
    beginFrame();
    glClearColor(0.08,0.1,0.1,1);
    glClear(GL_COLOR_BUFFER_BIT);

    //reset batches count for each frame
    for (int i = 0; i < state.batchCount; i++) {
        state.batches[i].count = 0;
    }
    state.batchCount = 0; 	
};

void
renderEnd(void) {
    for (int i = 0; i < state.batchCount; i++) {
	struct Batch* batch = &state.batches[i];
        if (batch->count > 0) {
            renderDrawInstances(batch->texture, batch->instances, batch->count);
        }
    }

    SDL_GL_SwapWindow(global.render.window);   
};

void renderSubmitQuad(struct Quad* quad) {

    GLuint tex = quad->texture ? quad->texture : state.texture_color;

    // Find existing batch with same texture
    if(state.batchCount >= MAX_BATCHES)
	ERROR_EXIT("Exceeded MAX_BATCHES: %d... (increase max batches capacity) \n", MAX_BATCHES)
    int batchIdx = -1;
    for (int i = 0; i < state.batchCount; i++) {
        if (state.batches[i].texture == tex) {
            batchIdx = i;
            break;
        }
    }

    // If not found, create a new batch
    if (batchIdx == -1) {
        batchIdx = state.batchCount++;
        struct Batch* new_batch = &state.batches[batchIdx];
        new_batch->texture = tex;
        new_batch->count = 0;
    }

    struct Batch* batch = &state.batches[batchIdx];

    // make sure theres enough space, if not double and reallocate memory
    if (batch->count >= batch->capacity) {
	// for now, if exceed the global capacity, either change that or log and exit
        ERROR_EXIT("Batch is full, exceeded global capacity: %d... (increase max global capacity) \n" , MAX_INSTANCES_PER_BATCH)
	    return;
    }

    // Build instance data from quad into batch array
    struct InstanceData* inst = &batch->instances[batch->count++];
    mat4x4_identity(inst->model);
    mat4x4_translate(inst->model, quad->pos[0], quad->pos[1], 0.0f);
    mat4x4_rotate_Z(inst->model, inst->model, quad->rotation_angle);   // in radians
    mat4x4_scale_aniso(inst->model, inst->model, quad->size[0], quad->size[1], 1.0f);
    memcpy(inst->color, quad->color, sizeof(vec4));
}



void
renderQuad(struct Quad* quad) {

    struct InstanceData inst;
    mat4x4_identity(inst.model);
    mat4x4_translate(inst.model, quad->pos[0], quad->pos[1], 0.0f);
    mat4x4_scale_aniso(inst.model, inst.model, quad->size[0], quad->size[1], 1.0f);
    memcpy(inst.color, quad->color, sizeof(vec4));
    GLuint tex = quad->texture ? quad->texture : state.texture_color;
    renderDrawInstances(tex,&inst, 1); 

};

void renderDrawQuadsInstanced(struct Quad* quads, size_t count) {
    if (count == 0) return;

    // grow GPU buffer if needed
    if (count > state.instance_capacity) {
        // grow to at least `count` 
        size_t newcap = state.instance_capacity;
        while (newcap < count) newcap *= 2;
        state.instance_capacity = newcap;	
        glBindBuffer(GL_ARRAY_BUFFER, state.instance_vbo);
        glBufferData(GL_ARRAY_BUFFER, state.instance_capacity * sizeof(struct InstanceData), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // prepare CPU-side instances
    struct InstanceData *instances = allocateInstances(count);
    if(!instances)
	ERROR_EXIT("RENDER ERROR "
		     "FAILED to allocate instances \n")
    for (size_t i = 0; i < count; ++i) {
        struct Quad *quad = &quads[i];

        mat4x4 model;
        mat4x4_identity(model);
        mat4x4_translate(model, quad->pos[0], quad->pos[1], 0.0f);
	mat4x4_rotate_Z(model, model, quad->rotation_angle);   // in radians
        mat4x4_scale_aniso(model, model, quad->size[0], quad->size[1], 1.0f);

	memcpy(instances[i].model, model, sizeof(mat4x4));
	
        // copy color
        for (int c = 0; c < 4; ++c) instances[i].color[c] = quad->color[c];
    }

    // upload the instance data 
    glBindBuffer(GL_ARRAY_BUFFER, state.instance_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(struct InstanceData), instances);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // draw
    glUseProgram(state.shader_default);
    glBindVertexArray(state.vao_quad);

    glBindTexture(GL_TEXTURE_2D, state.texture_color);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)count);

    glBindVertexArray(0);
}



static void
renderDrawInstances(GLuint texture,struct InstanceData* instances, size_t count) {
    if (count == 0) return;

    // make sure the GPU buffer is large enough
    if (count > state.instance_capacity) {
        size_t newcap = state.instance_capacity;
        while (newcap < count) newcap *= 2;
        state.instance_capacity = newcap;
	glBindBuffer(GL_ARRAY_BUFFER, state.instance_vbo);
        glBufferData(GL_ARRAY_BUFFER, state.instance_capacity * sizeof(struct InstanceData), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    
    // upload instance data
    glBindBuffer(GL_ARRAY_BUFFER, state.instance_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(struct InstanceData), instances);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // drawing
    glUseProgram(state.shader_default);
    glBindVertexArray(state.vao_quad);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)count);
    glBindVertexArray(0);
}

//render the visual representation of AABB bodies 
void renderAABB(AABB* aabb, vec4 color) {
    if (!aabb) return;

    glUseProgram(state.shader_default);

    //  full size from half_size
    float w = aabb->dims[0] * 2.0f;
    float h = aabb->dims[1] * 2.0f;

    mat4x4 model;
    mat4x4_identity(model);
    mat4x4_translate(model, aabb->coords[0], aabb->coords[1], 0);
    mat4x4_scale_aniso(model, model, w, h, 1);

    glUniformMatrix4fv(glGetUniformLocation(state.shader_default, "model"),
                       1, GL_FALSE, &model[0][0]);
    glUniform4fv(glGetUniformLocation(state.shader_default, "color"), 1, color);
    vec4 white = { 255,255,255,1};
    glBindVertexArray(state.vao_quad);
    
    glUniform1i(glGetUniformLocation(state.shader_default, "useTexture"), 0);
    
    // draw the outline of the quad using GL_LINE 
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(0);
}

void
drawAllAABB(void) {
    size_t count = physicsGetBodyCount();
    vec4 wireColor = {1.0f,0.0f,0.0f,1.0f};
    for (size_t i = 0; i < count; i++) {
        struct Body* body = physicsGetBody(i);
	renderAABB(&body->aabb,wireColor);
        
    }
}



void renderShutdown(void) {
//    free batch instance arrays (double free or something is occuring at the moment. fix this )
    for (int i = 0; i < MAX_BATCHES; i++) {
        free(state.batches[i].instances);
    }
    // free ring buffer
    free(instance_buffer);
    instance_buffer = NULL;
    // delete GL resources
    glDeleteProgram(state.shader_default);
    glDeleteTextures(1, &state.texture_color);
    glDeleteVertexArrays(1, &state.vao_quad);
    glDeleteBuffers(1, &state.vbo_quad);
    glDeleteBuffers(1, &state.ebo_quad);
    glDeleteBuffers(1, &state.instance_vbo);

}
