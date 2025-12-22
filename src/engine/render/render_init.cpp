//(TODO:done)read opengl getting started chapter tomorrow

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <stdlib.h>

#include "../global.h"
#include "../../helpers.h"

#include "../render.h"
#include "render_internal.h"


SDL_Window *renderInitWindow(int width, int height) {

    int i ;
    float x, y;
    
 
    SDL_Window * window = SDL_CreateWindow("BHE",
					   global.render.width-200,
					   global.render.height-200,
					   SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
			   
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



    // 1. bind Vertex Array Object
    // 2. copy our vertices array in a buffer for OpenGL to use
    // 3. then set our vertex attributes pointers
void renderInitQuad(uint32 *vao, uint32 *vbo, uint32 *ebo) {

    real32 vertices[] =
	{ 
	    //uv coordinates tell open gl how to map a texture
	    // for xyz, most engines render from top left or bottom right
	    // but here we trying to make it use the center of the screen
	    // so each vertics is  gonna sit at 0.5
            //x     y   z  u  v
	     0.5,  0.5, 0, 0, 0, 
	     0.5, -0.5, 0, 0, 0, 
	    -0.5, -0.5, 0, 0, 0, 
	    -0.5,  0.5, 0, 0, 0 
	};

    // a quad is just two triangles
    // so we try to tell opengl the order of what makes up the triangle
    uint32 indices [] =
	{
	    0,1,3, //first triangle
	    1,2,3 // second triangle
	};


    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glGenBuffers(1, ebo);

    glBindVertexArray(*vao);

    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
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

    glBindVertexArray(0);
};


void renderInitColorTexture(uint32 *texture)
    {
	glGenTextures(1,texture);
	glad_glBindTexture(GL_TEXTURE_2D,*texture);
	uint8 white[4] = {255,255,255,255};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1,1,0, GL_RGBA, GL_UNSIGNED_BYTE, white);
	glBindTexture(0,GL_TEXTURE_2D);
    };
//reminder to do something abt this
void renderInitShaders(Render_State_Internal *state){
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
};
