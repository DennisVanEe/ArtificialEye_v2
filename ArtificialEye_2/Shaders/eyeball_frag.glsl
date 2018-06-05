#version 330 core

uniform vec4 u_modelColor;

out vec4 Color;

in vec3 p_worldPosition;

void main()
{
    if (p_worldPosition.x > 0.25)
    {
        discard;
    }

    if (p_worldPosition.z > 0.65)
    {
        Color = vec4(240 / 255.f, 248 / 255.f, 1.f, 0.25f); // u_modelColor * 0.5f;
    }
    else
    {
        Color = u_modelColor;
    }
}