#version 330 core

layout(location = 0) in vec3 l_position;
layout(location = 1) in vec3 l_normals;
layout(location = 2) in vec2 l_textCoord; // not super efficient, but whatever

uniform mat4 u_posTrans;
uniform mat4 u_model;

out vec3 p_normal;
out vec3 p_position;

void main()
{
    gl_Position = u_posTrans * vec4(l_position, 1.f);
    p_normal = mat3(transpose(inverse(u_model))) * l_normals;
    p_position = vec3(u_model * vec4(l_position, 1.f));
}
