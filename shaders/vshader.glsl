#version 410

in vec3 v_position;
in vec3 v_normal;

uniform mat4 u_mvpMatrix;
uniform vec3 u_color;

out vec3 f_color;

void main()
{
    f_color = v_normal*0.5+u_color*0.5; //clamp(v_normal*0.5 + u_color*0.5,0.0f,1.0f);
    gl_Position = u_mvpMatrix * vec4(v_position,1.0);
}

