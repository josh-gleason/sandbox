#version 410

// fragment properties
in vec3 f_color;

// material properties
layout(std140) uniform Material
{
    uniform vec3  diffuse;
    uniform vec3  specular;
    uniform vec3  ambient;
    uniform float shininess;
    uniform float texBlend;
} material;

// output color
out vec4 out_color;

void main()
{
    out_color = vec4(f_color,1.0);
}

