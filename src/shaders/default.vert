#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uvs;

layout (location = 2) in mat4 instance_model;
layout (location = 6) in vec4 instance_color;

out vec2 uvs;
out vec4 color;

uniform mat4 projection;

void main(){
     uvs = a_uvs;
     color = instance_color;
     
     gl_Position = projection * instance_model * vec4(a_pos,1.0);
}