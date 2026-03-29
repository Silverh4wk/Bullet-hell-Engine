
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include "../global.h"
#include "../render.h"
#include "render_internal.h"

static struct RenderStateInternal state = {0};



void
renderInit(void)
{
 //the following attrb must be set before creating a window
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);

    int i ;
    float x, y;
    
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

state.batchCount = 0;
    for (int i = 0; i < 64; i++) {
        state.batches[i].instances = NULL;
        state.batches[i].count = 0;
        state.batches[i].capacity = 0;}    
};


    // 3 buffers make up what u see: color, depth and the stencil buffer 
void
renderBegin(void) {
    glClearColor(0.08,0.1,0.1,1);
    glClear(GL_COLOR_BUFFER_BIT);

    //reset batches count for each frame
    int prevBatchCount = state.batchCount;
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
        // Do not free memory, just reset count next frame
    }

    SDL_GL_SwapWindow(global.render.window);   
};

void renderSubmitQuad(struct Quad* quad) {

    GLuint tex = quad->texture ? quad->texture : state.texture_color;

    // Find existing batch with same texture
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
        struct Batch* newBatch = &state.batches[batchIdx];
        newBatch->texture = tex;
        newBatch->count = 0;
        newBatch->capacity = 1024; // initial capacity
        newBatch->instances = malloc(newBatch->capacity * sizeof(struct InstanceData));
    }

    struct Batch* batch = &state.batches[batchIdx];

    // make sure theres enough space, if not double
    if (batch->count >= batch->capacity) {
        batch->capacity *= 2;
        batch->instances = realloc(batch->instances, batch->capacity * sizeof(struct InstanceData));
    }

    // Build instance data from quad
    struct InstanceData* inst = &batch->instances[batch->count++];
    mat4x4_identity(inst->model);
    mat4x4_translate(inst->model, quad->pos[0], quad->pos[1], 0.0f);
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
    struct InstanceData *instances = malloc(count * sizeof(struct InstanceData));
    for (size_t i = 0; i < count; ++i) {
        struct Quad *q = &quads[i];

        mat4x4 model;
        mat4x4_identity(model);
        mat4x4_translate(model, q->pos[0], q->pos[1], 0.0f);
        mat4x4_scale_aniso(model, model, q->size[0], q->size[1], 1.0f);

        // copy model into instance 
        float *m = &model[0][0];
	memcpy(instances[i].model, model, sizeof(mat4x4));
	
        // copy color
        for (int c = 0; c < 4; ++c) instances[i].color[c] = q->color[c];
    }

    // upload the instance data 
    glBindBuffer(GL_ARRAY_BUFFER, state.instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, state.instance_capacity * sizeof(struct InstanceData), NULL, GL_DYNAMIC_DRAW); // orphan
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(struct InstanceData), instances);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // draw
    glUseProgram(state.shader_default);
    glBindVertexArray(state.vao_quad);

    glBindTexture(GL_TEXTURE_2D, state.texture_color);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)count);

    glBindVertexArray(0);
    free(instances);
}



void renderDrawInstances(GLuint texture,struct InstanceData* instances, size_t count) {
    if (count == 0) return;

    // make sure the GPU buffer is large enough
    if (count > state.instance_capacity) {
        size_t newcap = state.instance_capacity;
        while (newcap < count) newcap *= 2;
        state.instance_capacity = newcap;
    }

    // upload instance data
    glBindBuffer(GL_ARRAY_BUFFER, state.instance_vbo);
    //maybe instead of orphanining every draw call, just do it once using MAX_INSTANCE 
    glBufferData(GL_ARRAY_BUFFER, state.instance_capacity * sizeof(struct InstanceData),
                 NULL, GL_DYNAMIC_DRAW); // orphan
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(struct InstanceData), instances);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // drawing
    glUseProgram(state.shader_default);
    glBindVertexArray(state.vao_quad);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)count);
    glBindVertexArray(0);
}

//(reminder to fix this)
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
