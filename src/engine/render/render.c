#include <glad/glad.h>
#include <SDL3/SDL.h>
#include "../global.h"
#include "../render.h"
#include "render_internal.h"
#include "../ecs_internal.h"
#include "../camera.h"
#include "../global.h"
#include "freetype/freetype.h"



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

//preallocate the batches data for both quad and circle
static void
renderInitBatches(int max_batches, int instances_per_batch) {
    state.batchCount = state.circle_batch_count = 0;
    for (int i = 0; i < max_batches; i++) {
        state.quad_batches[i].instances = malloc(instances_per_batch * sizeof(struct InstanceData));
        state.circle_batches[i].instances = malloc(instances_per_batch * sizeof(struct InstanceData));
        state.quad_batches[i].capacity = instances_per_batch;
        state.circle_batches[i].capacity = instances_per_batch;
        state.quad_batches[i].count = state.circle_batches[i].count = 0;
        state.quad_batches[i].texture = state.circle_batches[i].texture = 0;
    }
    
}

//final draw for a quad batch 
static void
renderDrawQuadInstances(GLuint texture,struct InstanceData* instances, size_t count) {
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
    GLint useTexture = (texture == state.texture_color) ? 0 : 1;
    glUniform1i(glGetUniformLocation(state.shader_default, "useTexture"),useTexture);

    glBindVertexArray(state.vao_quad);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(state.shader_default, "texture_ID"), 0);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)count);
    glBindVertexArray(0);
}

//final draw for a circle batch
static void
renderDrawCircleInstances(GLuint texture, struct InstanceData* instances, size_t count) {
    if (count == 0) return;
    
    if (count > state.instance_capacity) {
        size_t newcap = state.instance_capacity;
        while (newcap < count) newcap *= 2;
        state.instance_capacity = newcap;
        glBindBuffer(GL_ARRAY_BUFFER, state.instance_vbo);
        glBufferData(GL_ARRAY_BUFFER, state.instance_capacity * sizeof(struct InstanceData), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, state.instance_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(struct InstanceData), instances);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(state.shader_default);
    GLint useTexture = (texture == state.texture_color) ? 0 : 1;
    glUniform1i(glGetUniformLocation(state.shader_default, "useTexture"),useTexture);
    glBindVertexArray(state.vao_circle);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(state.shader_default, "texture_ID"), 0);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, (GLsizei)state.circle_vertex_count, (GLsizei)count);
    glBindVertexArray(0);
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
    
    //initializiation
    renderInitQuad(&state);
    renderInitCircle(&state);
    renderInitShaders(&state);
    renderInitColorTexture(&state.texture_color);
    renderInitText(&state);
};


    // 3 buffers make up what u see: color, depth and the stencil buffer 
void
renderBegin(void) {
    beginFrame();
    glClearColor(0.08,0.1,0.1,1);
    glClear(GL_COLOR_BUFFER_BIT);

    //reset batches count for each frame
    for (int i = 0; i < state.batchCount; i++) {
        state.quad_batches[i].count = 0;
    }
    state.batchCount = 0; 	
    for (int i = 0; i < state.circle_batch_count; i++) {
        state.circle_batches[i].count = 0;
    }
    state.circle_batch_count = 0;
};

void
renderEnd(void) {
    for (int i = 0; i < state.batchCount; i++) {
	struct Batch* batch = &state.quad_batches[i];
        if (batch->count > 0) {
            renderDrawQuadInstances(batch->texture, batch->instances, batch->count);
        }
    }

    for (int i = 0; i < state.circle_batch_count; i++) {
        struct Batch* batch = &state.circle_batches[i];
        if (batch->count > 0) {
            renderDrawCircleInstances(batch->texture, batch->instances, batch->count);
        }
    }

    SDL_GL_SwapWindow(global.render.window);   
};

void
renderSubmitShape(struct Shape* shape) {
    if (!shape) return;

    GLuint tex = shape->texture ? shape->texture : state.texture_color;

    if (shape->shape_type == SHAPE_CIRCLE) {
        // find/create circle batch for this texture
        if (state.circle_batch_count >= MAX_BATCHES)
            ERROR_EXIT("Exceeded MAX_BATCHES for circles: %d... \n", MAX_BATCHES)

        int batchIdx = -1;
        for (int i = 0; i < state.circle_batch_count; i++) {
            if (state.circle_batches[i].texture == tex) {
                batchIdx = i;
                break;
            }
        }

        if (batchIdx == -1) {
            batchIdx = state.circle_batch_count++;
            struct Batch* new_batch = &state.circle_batches[batchIdx];
            new_batch->texture = tex;
            new_batch->count = 0;
        }
	
        struct Batch* batch = &state.circle_batches[batchIdx];
        if (batch->count >= batch->capacity) {
            ERROR_EXIT("Circle batch is full, exceeded capacity: %d\n", MAX_INSTANCES_PER_BATCH);
            return;
        }

        struct InstanceData* inst = &batch->instances[batch->count++];
        mat4x4_identity(inst->model);
        mat4x4_translate(inst->model, shape->pos[0], shape->pos[1], 0.0f);
        mat4x4_scale_aniso(inst->model, inst->model, shape->data.circle.radius, shape->data.circle.radius, 1.0f);
        memcpy(inst->color, shape->color, sizeof(vec4));
        return;
    }

    // default: treat as quad
    // Find existing batch with same texture
    if(state.batchCount >= MAX_BATCHES)
        ERROR_EXIT("Exceeded MAX_BATCHES: %d... (increase max batches capacity) \n", MAX_BATCHES)
    int batchIdx = -1;
    for (int i = 0; i < state.batchCount; i++) {
        if (state.quad_batches[i].texture == tex) {
            batchIdx = i;
            break;
        }
    }

    // If not found, create a new batch
    if (batchIdx == -1) {
        batchIdx = state.batchCount++;
        struct Batch* new_batch = &state.quad_batches[batchIdx];
        new_batch->texture = tex;
        new_batch->count = 0;
    }

    struct Batch* batch = &state.quad_batches[batchIdx];

    // make sure theres enough space, if not double and reallocate memory
    if (batch->count >= batch->capacity) {
	// for now, if exceed the global capacity, either change that or log and exit
        ERROR_EXIT("Batch is full, exceeded global capacity: %d... (increase max global capacity) \n" , MAX_INSTANCES_PER_BATCH)
        return;
    }

    // Build instance data from quad into batch array
    struct InstanceData* inst = &batch->instances[batch->count++];
    mat4x4_identity(inst->model);
    mat4x4_translate(inst->model, shape->pos[0], shape->pos[1], 0.0f);
    mat4x4_rotate_Z(inst->model, inst->model, shape->data.quad.rotation_angle);   // in radians
    mat4x4_scale_aniso(inst->model, inst->model, shape->data.quad.size[0], shape->data.quad.size[1], 1.0f);
    memcpy(inst->color, shape->color, sizeof(vec4));
}

//render the visual representation of AABB bodies 
void renderAABB(AABB* aabb, vec4 color, int toggle) {

    if (!aabb) return;

    glUseProgram(state.shader_default);

    //  full size from half_size
    float w = aabb->dims[0] * 2.0f;
    float h = aabb->dims[1] * 2.0f;

    mat4x4 model;
    mat4x4_identity(model);
    mat4x4_translate(model, aabb->coords[0], aabb->coords[1], 1);
    mat4x4_scale_aniso(model, model, w, h, 1.0f);

       glUniform1i(
        glGetUniformLocation(state.shader_default, "use_uniform_model"),
        1
    );

    glUniform1i(
        glGetUniformLocation(state.shader_default, "use_uniform_color"),
        1
    );

    // upload uniforms
    glUniformMatrix4fv(
        glGetUniformLocation(state.shader_default, "model"),
        1,
        GL_FALSE,
        &model[0][0]
    );

    glUniform4fv(
        glGetUniformLocation(state.shader_default, "uniform_color"),
        1,
        color
    );

    glUniform1i(
        glGetUniformLocation(state.shader_default, "useTexture"),
        0
    );
    
    glBindVertexArray(state.vao_quad);
    
    // draw the outline of the quad using GL_LINE
    if (toggle)
    {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    else 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(0);

        glUniform1i(
        glGetUniformLocation(state.shader_default, "use_uniform_model"),
        0
    );

    glUniform1i(
        glGetUniformLocation(state.shader_default, "use_uniform_color"),
        0
    );
}

void
drawAllAABB(int toggle) {
    size_t count = physicsGetBodyCount();
    vec4 wireColor = {1.0f,0.0f,0.0f,1.0f}; //red
    for (size_t i = 0; i < count; i++) {
        struct Body* body = physicsGetBody(i);
	renderAABB(&body->aabb,wireColor, toggle);
        
    }
}


void renderShutdown(void) {
//    free batch instance arrays (double free or something is occuring at the moment. fix this )
    for (int i = 0; i < MAX_BATCHES; i++) {
        free(state.quad_batches[i].instances);
    }
    for (int i = 0; i < MAX_BATCHES; i++) {
        free(state.circle_batches[i].instances);
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
    glDeleteVertexArrays(1, &state.vao_circle);
    glDeleteBuffers(1, &state.vbo_circle);
    glDeleteBuffers(1, &state.instance_vbo);

}

void
renderECS(void) {
	for (Entity e = 1; e <= g_next_free; e++) {
        // check for both shape and transform data
        uint64 required = (1 << COMPONENT_SHAPE) | (1 << COMPONENT_TRANSFORM);
        if ((g_component_mask[e] & required) != required) continue;
	
	struct Shape* shape = g_shapes[e].shape;
        if (!shape) continue;
  
        renderSubmitShape(shape);
    }
}

// camera = combining renderer projection and camera view 
void camera_apply(const Camera* cam) {
    if (!cam) return;
    mat4x4 combined;
    mat4x4_mul(combined, state.projection, *camera_get_view_matrix((Camera*)cam));
    glUseProgram(state.shader_default);
    glUniformMatrix4fv(glGetUniformLocation(state.shader_default, "projection"), 1, GL_FALSE, &combined[0][0]);
}

//https://gedge.ca/blog/2013-12-08-opengl-text-rendering-with-freetype/
void
renderText(char *text, real32 x, real32 y, vec4* color) {
    real32 text_scale_x = 2.0f / global.render.width;
    real32 text_scale_y = 2.0f / global.render.height;
    
    glUseProgram(state.text_shader);

    static mat4x4 identity;
    static int identity_init = 0;
    if (!identity_init) {
        mat4x4_identity(identity);
        identity_init = 1;
    }
    glUniformMatrix4fv(glGetUniformLocation(state.text_shader, "projection"),
                       1, GL_FALSE, &identity[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state.atlas_tex);
    glUniform1i(glGetUniformLocation(state.text_shader, "texture_ID"), 0);

       GLint colorLoc = glGetUniformLocation(state.text_shader, "color");
       if ( color != NULL) {
	   glUniform4fv(colorLoc, 1, *color);
       } else {
	   // Default white if no color provided
	   vec4 white = {1.0f, 1.0f, 1.0f, 1.0f};
	   glUniform4fv(colorLoc, 1, white);
    }
    
    glBindVertexArray(state.text_vao);
    glBindBuffer(GL_ARRAY_BUFFER, state.text_vbo);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    #define MAX_VERTICES 4096
    float vertices[MAX_VERTICES * 4];  // x, y, s, t per vertex
    int vert_count = 0;

    float pen_x = x;
    float pen_y = y;

    while (*text && vert_count + 6 <= MAX_VERTICES) {
        int c = *text;
        if (c < FIRST_CHAR || c >= LAST_CHAR) {
            text++;
            continue;
        }

        GlyphInfo *g = &state.glyphs[c - FIRST_CHAR];
        if (g->bw == 0 && g->bh == 0) {
            pen_x += g->ax;
            pen_y += g->ay;
            text++;
            continue;
        }

        // Convert pixel coordinates to NDC 
        float vx = (pen_x + g->bl) * text_scale_x - 1.0f;
        float vy = 1.0f - (pen_y - g->bt) * text_scale_y; 
        float w = g->bw * text_scale_x;
        float h = g->bh * text_scale_y;

        // Add 6 vertices (two triangles)
        float *v = &vertices[vert_count * 4];
	
        v[0]  = vx;      v[1]  = vy;      v[2]  = g->tx0; v[3]  = g->ty0;
        v[4]  = vx;      v[5]  = vy - h;  v[6]  = g->tx0; v[7]  = g->ty1;
        v[8]  = vx + w;  v[9]  = vy;      v[10] = g->tx1; v[11] = g->ty0;

        v[12] = vx + w;  v[13] = vy;      v[14] = g->tx1; v[15] = g->ty0;
        v[16] = vx;      v[17] = vy - h;  v[18] = g->tx0; v[19] = g->ty1;
        v[20] = vx + w;  v[21] = vy - h;  v[22] = g->tx1; v[23] = g->ty1;

        vert_count += 6;

        pen_x += g->ax;
        pen_y += g->ay;
        text++;
    }
    
    if (vert_count > 0) {
        glBufferData(GL_ARRAY_BUFFER, vert_count * 4 * sizeof(float),
                     vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, vert_count);
    }

    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
