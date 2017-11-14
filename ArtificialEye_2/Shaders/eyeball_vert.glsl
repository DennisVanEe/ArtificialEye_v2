#version 330 core

layout(location = 0) in vec3 l_position;
layout(location = 1) in vec3 l_normal;
layout(location = 2) in vec2 l_textCoord;

out vec2 p_textCoord;
out vec3 p_worldPosition;

uniform mat4 u_posTrans;
uniform mat4 u_model;

void main()
{
	// just update the position
	gl_Position = u_posTrans * vec4(l_position, 1.0);
	p_worldPosition = vec3(u_model * vec4(l_position, 1.0));
	p_textCoord = vec2(l_textCoord.x, l_textCoord.y); //textCoord;
}