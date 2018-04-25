#version 330 core

layout(location = 0) out vec4 Color;

smooth in vec3 p_normal;
smooth in vec3 p_position;

uniform samplerCube t_skyBox;

uniform vec3 u_cameraPosition;
uniform float u_refractIndex;

// some quick modifications so that I can do this:
const vec3 lightPos = vec3(2.0, 0.5, 0.0);

void main()
{      
	// if (p_position.x < -0.25)
	// {
	// 	 discard;
	// }
       
     float eta = 1.0 / u_refractIndex;
     vec3 I = normalize(p_position - u_cameraPosition);
     vec3 R = refract(I, normalize(p_normal), eta);
     Color = texture(t_skyBox, R);
}