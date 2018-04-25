#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

smooth out vec3 p_normal;
smooth out vec3 p_position;

in vec3 pg_position[3];
in mat4 pg_model[3];

void main()
{
	// calculate the normal:
    vec3 a = vec3(pg_position[1]) - vec3(pg_position[0]);
    vec3 b = vec3(pg_position[2]) - vec3(pg_position[0]);
    vec3 norm = normalize(cross(b, a));
    if (dot(norm, vec3(pg_position[0])) > 0)
    {
        norm = -norm;
    }

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