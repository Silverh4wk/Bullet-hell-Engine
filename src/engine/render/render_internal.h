#ifndef RENDER_INTERNAL_H
#define RENDER_INTERNAL_H

#include <SDL3/SDL.h>
#include "../render.h"

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
};

SDL_Window *
renderInitWindow(int width, int height);

void
renderInitQuad(uint32 *vao, uint32 *vbo, uint32 *ebo);

void
renderInitColorTexture(uint32 *texture);

void
renderInitShaders(struct RenderStateInternal *state);

uint32
renderShaderCreate(const char *path_vert, const char *path_frag);

#endif
