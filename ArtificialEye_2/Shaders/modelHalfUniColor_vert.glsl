#version 330 core

layout(location = 0) in vec3 l_position;

uniform mat4 u_posTrans;
uniform mat4 u_model;

out vec3 p_worldPosition;

void main()
{
    gl_Position = u_posTrans * vec4(l_position, 1.f);
    p_worldPosition = vec3(u_model * vec4(l_position, 1.f));
}