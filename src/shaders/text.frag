#version 410 core
uniform sampler2D texture_ID; 
uniform vec4 color;
in vec2 texCoords;
out vec4 fragColor;

void main(void) {
    fragColor = vec4(1, 1, 1, texture(texture_ID, texCoords).r) * color;
}