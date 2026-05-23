//(TODO:done)read opengl getting started chapter tomorrow

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <math.h>

#include "../global.h"
#include "../../helpers.h"

#include "../render.h"
#include "render_internal.h"


SDL_Window *
renderInitWindow(int width, int height) {

    int i ;
    float x, y;

    SDL_Window * window = SDL_CreateWindow("BHE",
					   global.render.width,
					   global.render.height,
					   SDL_WINDOW_OPENGL  |SDL_WINDOW_INPUT_FOCUS|SDL_WINDOW_RESIZABLE);
			   
    if (!window) {
        ERROR_EXIT("Failed to create a window: %s\n", SDL_GetError());
    }

    
    //Glad basically load all the pointers from memory into the right place so we can use them
    
    SDL_GL_CreateContext(window);
    
    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {	ERROR_EXIT( "Failed to LoadGL: %s\n", SDL_GetError());
    }
    puts("Open Gl loaded");
    printf("Vendor:   %s\n", glGetString(GL_VENDOR))  ; 
    printf("Renderer: %s\n", glGetString(GL_RENDERER)); 
    printf("Version:  %s\n", glGetString(GL_VERSION)) ; 
    
    return window;
};

void renderInitCircle(struct RenderStateInternal *state)
{
    const int segments = 32;
    const int vert_count = segments + 2; // center + segments + repeat of first
    state->circle_vertex_count = vert_count;

    float *vertices = malloc(sizeof(float) * vert_count * 5);
    if (!vertices) return;

    // center
    vertices[0] = 0.0f;
    vertices[1] = 0.0f;
    vertices[2] = 0.0f;
    vertices[3] = 0.0f;
    vertices[4] = 0.0f;

    for (int i = 0; i <= segments; ++i) {
        float theta = (float)i / (float)segments * 2.0f * Pi32;
        float x = cosf(theta);
        float y = sinf(theta);
        int idx = (i + 1) * 5;
        vertices[idx + 0] = x;
        vertices[idx + 1] = y;
        vertices[idx + 2] = 0.0f;
        vertices[idx + 3] = 0.0f;
        vertices[idx + 4] = 0.0f;
    }

    glGenVertexArrays(1, &state->vao_circle);
    glGenBuffers(1, &state->vbo_circle);

    glBindVertexArray(state->vao_circle);
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo_circle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vert_count * 5, vertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);
    // uv 
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // attach instance attributes (mat4 + color) using the shared instance VBO
    glBindBuffer(GL_ARRAY_BUFFER, state->instance_vbo);
    GLsizei stride = sizeof(struct InstanceData);
    for (int i = 0; i < 4; ++i) {
        GLuint loc = 2 + i;
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 4 * i));
        glVertexAttribDivisor(loc, 1);
    }
    GLuint color_loc = 6;
    glEnableVertexAttribArray(color_loc);
    glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 16));
    glVertexAttribDivisor(color_loc, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    free(vertices);
}


// 1. bind Vertex Array Object
// 2. copy our vertices array in a buffer for OpenGL to use
// 3. then set our vertex attributes pointers
void renderInitQuad(struct RenderStateInternal *state) {

    real32 vertices[] =
	{ 
	    //uv coordinates tell open gl how to map a texture
	    // for xyz, most engines render from top left or bottom right
	    // but here we trying to make it use the center of the screen
	    // so each vertics is  gonna sit at 0.5
            //x     y   z  u  v
	     0.5,  0.5, 0, 1.0,1.0, 
	     0.5, -0.5, 0, 1.0, 0, 
	    -0.5, -0.5, 0, 0, 0, 
	    -0.5,  0.5, 0, 0, 1.0 
	};
    
    // a quad is just two triangles
    // so we try to tell opengl the order of what makes up the triangle
    uint32 indices [] =
	{
	    0,1,3, //first triangle
	    1,2,3 // second triangle
	};


    glGenVertexArrays(1,&state->vao_quad);
    glGenBuffers(1, &state->vbo_quad);
    glGenBuffers(1, &state->ebo_quad);

    glBindVertexArray(state->vao_quad);

    glBindBuffer(GL_ARRAY_BUFFER, state->vbo_quad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state->ebo_quad);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    //stride: Amount of bytes from the beginning of one element to the beginning of the following element. If you pass a zero as stride, it means they are tightly packed. If you have an array of floats, which contains vertices like this, x1,y1,z1,x2,y2,z2... and so on, you can set stride to either zero (as tightly packed), or 12 (3 floats*4 bytes each from the beginning of vertex one to the beginning of vertex two)

    //xyz
    glVertexAttribPointer(	0,
				3,
				GL_FLOAT,
				GL_FALSE,
				5*sizeof(real32),
				NULL);
    
    //uv
    glVertexAttribPointer(	1,
				2,
				GL_FLOAT,
				GL_FALSE,
				5*sizeof(real32),
				(void*) (3* sizeof(real32))  );

    glEnableVertexAttribArray(0); 
    glEnableVertexAttribArray(1);

    //instancing setup

    glGenBuffers(1, &state->instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, state->instance_vbo);

    state->instance_capacity = 1024;

    glBufferData(
        GL_ARRAY_BUFFER,
        state->instance_capacity * sizeof(struct InstanceData),
        NULL,
        GL_DYNAMIC_DRAW
    );

    GLsizei stride = sizeof(struct InstanceData);

    // mat4 -> uses locations 2,3,4,5
    for(int i = 0; i < 4; i++)
    {
        GLuint loc = 2 + i;

        glEnableVertexAttribArray(loc);

        glVertexAttribPointer(
            loc,
            4,
            GL_FLOAT,
            GL_FALSE,
            stride,
            (void*)(sizeof(float) * 4 * i)
        );

        glVertexAttribDivisor(loc,1);
    }

    // color attribute
    GLuint color_loc = 6;

    glEnableVertexAttribArray(color_loc);

    glVertexAttribPointer(
        color_loc,
        4,
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*)(sizeof(float) * 16)
    );

    glVertexAttribDivisor(color_loc,1);


    glBindVertexArray(0);

};


void renderInitColorTexture(uint32 *texture)
{
    
    glGenTextures(1,texture);
    glBindTexture(GL_TEXTURE_2D,*texture);
    
    uint8 white[4] = {255,255,255,255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1,1,0, GL_RGBA, GL_UNSIGNED_BYTE,white);
    glBindTexture(GL_TEXTURE_2D,0);
};

//reminder to do something abt this
void renderInitShaders(struct RenderStateInternal *state){
    state->shader_default = renderShaderCreate("I:/FYP/src/shaders/default.vert",
					       "I:/FYP/src/shaders/default.frag");
    mat4x4_ortho(state->projection, 0, global.render.width, 0, global.render.height,-2, 2);
    glUseProgram(state->shader_default);
    glUniformMatrix4fv(
	glGetUniformLocation(state->shader_default,"projection"),
	1,
	GL_FALSE,
	&state->projection[0][0]
	);
    //get the view uniform location
      state->view_uniform = glGetUniformLocation(state->shader_default, "view");
};
