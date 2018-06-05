#version 330 core

uniform vec4 u_modelColor;

out vec4 Color;

in vec3 p_worldPosition;

void main()
{
    if (p_worldPosition.x > 0.f)
    {
        discard;
    }

    Color = u_modelColor;
}