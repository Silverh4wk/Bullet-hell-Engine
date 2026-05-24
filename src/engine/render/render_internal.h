#ifndef RENDER_INTERNAL_H
#define RENDER_INTERNAL_H

#include <SDL3/SDL.h>

#include "../../types.h"
#include <Glad/glad.h>
#include <linmath.h>


#define ATLAS_WIDTH  1024
#define ATLAS_HEIGHT 1024
#define FIRST_CHAR   32
#define LAST_CHAR    127

typedef struct {
    float ax, ay;      // advance.x, advance.y
    float bw, bh;      // bitmap width, height
    float bl, bt;      // bearing left, top
    float tx0, ty0;    // normalised texture coordinates
    float tx1, ty1;
} GlyphInfo;


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

    //sending data to the graphic cards can be relatively slow
    // so when possible, we send whatever we can at once and have the
    // vertex buffer objects (VBO) store it in the GPU memory
    uint32 vao_quad;
    uint32 vbo_quad;
    uint32 ebo_quad;
    uint32 shader_default;
    uint32 texture_color;
    mat4x4 projection;

    // instancing 
    GLuint instance_vbo;
    size_t instance_capacity;  //current size of gpu buffer

    // Circle primitive 
    uint32 vao_circle;
    uint32 vbo_circle;
    size_t circle_vertex_count;

    //text primitives
    GLuint text_vao, text_vbo, text_texture;
    GLuint text_sampler;
    GLuint text_shader;
    GLuint  atlas_tex;
    GlyphInfo glyphs[LAST_CHAR - FIRST_CHAR];
    int atlas_built;
    
    //batching
    struct Batch quad_batches[64];        // max 64 different textures per frame
    int batchCount;
    struct Batch circle_batches[64]; 
    int circle_batch_count;

    //camera view
    GLint view_uniform; 
};

// text rendering VAO and VBO's


SDL_Window *
renderInitWindow(int width, int height);

void
renderInitQuad(struct RenderStateInternal *state);

void 
renderInitCircle(struct RenderStateInternal* state);

void
renderInitColorTexture(uint32 *texture);

void
renderInitShaders(struct RenderStateInternal *state);

uint32
renderShaderCreate(const char *path_vert, const char *path_frag);


uint32
renderLoadTexture(const char *path) ;

#endif
