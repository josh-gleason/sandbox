#version 410

in vec3 v_position;
in vec3 v_color;

uniform mat4 u_mvpMatrix;

out vec3 f_color;

void main()
{
    f_color = v_color;
    gl_Position = u_mvpMatrix * vec4(v_position,1.0);
}

