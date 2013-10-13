#version 410

in vec3 v_position;
in vec3 v_normal;
in vec2 v_uvCoord;

uniform mat4 u_mvpMatrix;
uniform vec3 u_color;

out vec3 f_color;
out vec2 f_uvCoord;

void main()
{
    f_uvCoord = v_uvCoord;
    f_color = clamp((v_normal+1.0)*0.25,0.f,0.5f)+u_color*0.5;
    gl_Position = u_mvpMatrix * vec4(v_position,1.0);
}

