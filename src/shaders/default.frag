#version 330 core

out vec4 frag_color;

in vec2 uvs;
in vec4 color;


uniform sampler2D texture_ID;
uniform bool useTexture;

void main(){
      if (useTexture) {
        frag_color = texture(texture_ID, uvs) * color;
    } else {
        frag_color = color;
    }	
}