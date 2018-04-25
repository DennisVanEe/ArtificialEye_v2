#version 330 core

// Refract:
// Per-face refraction using the default drawable shader interface

layout (location = 0) in vec3 l_position;
layout (location = 1) in vec3 l_normals;
//layout (location = 2) in vec2 l_textCoord;

uniform mat4 u_posTrans;
uniform mat4 u_model;

out vec3 pg_position;
out mat4 pg_model;

void main()
{
    gl_Position = u_posTrans * vec4(l_position, 1.f);
    pg_position = vec3(u_model * vec4(l_position, 1.f));
    pg_model = u_model;
}