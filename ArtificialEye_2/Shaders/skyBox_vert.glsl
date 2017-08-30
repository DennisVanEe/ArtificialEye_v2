#version 330 core

layout (location = 0) in vec3 l_position;

uniform mat4 u_posTrans;
uniform mat4 u_model;

out vec3 p_textCoord;

void main()
{
    p_textCoord = l_position;
    vec4 position = u_posTrans * vec4(l_position, 1.0);
    gl_Position = position.xyww;
} 
