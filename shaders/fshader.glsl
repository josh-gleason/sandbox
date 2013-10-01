#version 410

in vec3 f_color;
in vec2 f_uvCoord;

out vec4 out_color;

void main()
{
    if ( f_uvCoord.x < 0.f || fract(f_uvCoord.s*8) < 0.1f || fract(f_uvCoord.t*8) < 0.1f )
        out_color = vec4(f_color, 1.0);
    else
        discard;
}

