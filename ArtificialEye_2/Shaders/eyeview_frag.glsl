#version 330 core

in vec2 TextCoords;
out vec4 color;

uniform sampler2D eyeViewText;

void main()
{ 
	// this is a pretty simple shader as one can see!
	color = texture(eyeViewText, TextCoords);
}