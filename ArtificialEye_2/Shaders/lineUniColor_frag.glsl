#version 330 core

out vec4 Color;

uniform vec3 u_color;

void main()
{
    Color = vec4(u_color, 1.f);
}