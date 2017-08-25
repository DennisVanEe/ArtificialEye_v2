#version 330 core

out vec4 Color;
in vec3 p_normal;
in vec3 p_fragPosition;

struct Material 
{
    vec3 m_ambient;
    vec3 m_diffuse;
    vec3 m_specular;
    float m_shininess;
};

uniform Material u_modelMaterial;
uniform vec3 u_lightPosition;
uniform vec3 u_modelColor;

void main()
{
    vec3 normal = normalize(p_normal);

    vec3 ambient = u_modelMaterial.m_ambient * u_modelColor;

    vec3 lightDir = normalize(u_lightPosition - p_fragPosition);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = (diff * u_modelMaterial.m_diffuse);

    Color = vec4(ambient + diffuse, 1.f);
}