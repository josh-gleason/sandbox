#version 410

// output color
out vec4 out_color;

// light properties
layout (std140) uniform Light
{
    uniform vec3 position;
    uniform vec3 diffuse;
    uniform vec3 specular;
    uniform vec3 ambient;
} light;

// material properties
layout (std140) uniform Material
{
    uniform vec3  diffuse;
    uniform vec3  specular;
    uniform vec3  ambient;
    uniform float shininess;
} material;

void main()
{
    out_color = vec4(material.diffuse,1.0);
}

