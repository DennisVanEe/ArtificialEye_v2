#version 330 core

out vec4 Color;

in vec3 p_textCoord;

uniform samplerCube t_skyBox;

void main()
{    
    Color = texture(t_skyBox, TexCoords);
}