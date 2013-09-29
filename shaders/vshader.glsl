#version 410

in vec3 v_position;
in vec3 v_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 f_color;

void main()
{
    f_color = v_color;
    gl_Position = projection * view * model * vec4(v_position,1.0);
}

