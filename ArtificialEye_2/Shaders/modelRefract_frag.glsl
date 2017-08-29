#version 330 core

out vec4 Color;

in vec3 p_normal;
in vec3 p_position;

uniform vec3 u_cameraPosition;
uniform float u_refractIndex;
uniform samplerCube t_skyBox;

void main()
{             
    vec3 I = normalize(p_position - u_cameraPosition);
    vec3 R = refract(I, normalize(p_normal), u_refractIndex);
    Color = vec4(texture(t_skyBox, R).rgb, 1.0);
}