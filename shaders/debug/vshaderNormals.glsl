#version 410

layout(location=0) in vec3 v_position;
layout(location=1) in vec3 v_normal;

out vec3 g_normal;

void main()
{
    g_normal = normalize(v_normal);
    gl_Position = vec4(v_position,1.0);
}

