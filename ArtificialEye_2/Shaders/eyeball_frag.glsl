#version 330 core

out vec4 Color;

in vec2 p_textCoord;
in vec3 p_worldPosition;

uniform sampler2D textDiffuse0; uniform bool useTD0;
uniform sampler2D textDiffuse1; uniform bool useTD1;
uniform sampler2D textDiffuse2; uniform bool useTD2;
uniform sampler2D textDiffuse3; uniform bool useTD3;
uniform bool noText;

uniform sampler2D textSpec0;
uniform sampler2D textSpec1;
uniform sampler2D textSpec2;
uniform sampler2D textSpec3;

void main()
{    
	if (p_worldPosition.x < -0.1)
	{
		discard;
	}
	
    Color = vec4(0.f, 0.f, 0.f, 1.f);
	if (useTD0)
		Color = texture(textDiffuse0, p_textCoord);
	if (useTD1)
		Color = texture(textDiffuse1, p_textCoord);
	if (useTD2)
		Color = texture(textDiffuse2, p_textCoord);
	if (useTD3)
		Color = texture(textDiffuse3, p_textCoord);

	if (noText)
		Color = vec4(0.f, 0.f, 0.f, 1.f);
	else
	{
		if (Color.x > 0.99f && Color.y > 0.99f && Color.z > 0.99f)
		{
			Color = vec4(199.f/255.f, 199.f/255.f, 199.f/255.f, 0.5f);
		}
	}
}