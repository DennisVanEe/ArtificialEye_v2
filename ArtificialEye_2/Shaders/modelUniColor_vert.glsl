#version 330 core

layout(location = 0) in vec3 l_position;
layout(location = 1) in vec3 l_normals;
layout(location = 2) in vec2 l_textCoord;

uniform mat4 u_posTrans;

void main()
{
    gl_Position = u_posTrans * vec4(l_position, 1.f);
}