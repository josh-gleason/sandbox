#version 410

in vec3 f_color;
in vec2 f_uvCoord;

out vec4 out_color;

uniform sampler2D texMap;

void main()
{
    if ( f_uvCoord.x < 0 )
        out_color = vec4(f_color, 1.0);
    else
        out_color = texture2D(texMap, f_uvCoord.xy);
}

