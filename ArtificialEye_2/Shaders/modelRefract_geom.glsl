#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

smooth out vec3 p_normal;
smooth out vec3 p_position;

in vec3 pg_position[3];

void main()
{
	// calculate the normal:
    vec3 a = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 norm = normalize(cross(b, a));

    // apply these normals and send them off
    for(int i = 0; i < gl_in.length(); i++)
    {
	    p_normal = norm;
	    p_position = pg_position[i];
	    gl_Position = gl_in[i].gl_Position;
	    EmitVertex();
    }
	EndPrimitive();
}