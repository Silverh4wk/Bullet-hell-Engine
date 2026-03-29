#ifndef RENDER_INTERNAL_H
#define RENDER_INTERNAL_H

#include <SDL3/SDL.h>
#include "../../types.h"
#include <Glad/glad.h>
#include <linmath.h>

// per-instance data layout sent to GPU 
 struct InstanceData {
    mat4x4 model;    
    vec4 color;
 };

struct Batch {
    GLuint texture;                 // texture ID for this batch
    struct InstanceData* instances; // array of instances
    size_t count;                   // number of instances currently in batch
    size_t capacity;                 // allocated size of instances array
};



struct RenderStateInternal{
    uint32 vao_quad; 
    //sending data to the graphic cards can be relatively slow
    // so when possible, we send whatever we can at once and have the
    // vertex buffer objects (VBO) store it in the GPU memory
    uint32 vbo_quad;
    uint32 ebo_quad;
    uint32 shader_default;
    uint32 texture_color;
    mat4x4 projection;

    // instancing 
    GLuint instance_vbo;
    size_t instance_capacity;  //current size of gpu buffer

    //batching
    struct Batch batches[64];        // max 64 different textures per frame
    int batchCount;
};

SDL_Window *
renderInitWindow(int width, int height);

void
renderInitQuad(struct RenderStateInternal *state,uint32 *vao, uint32 *vbo, uint32 *ebo);

void
renderInitColorTexture(uint32 *texture);

void
renderInitShaders(struct RenderStateInternal *state);

uint32
renderShaderCreate(const char *path_vert, const char *path_frag);

#endif
