#version 410

in vec3 f_color;
in vec2 f_uvCoord;

out vec4 out_color;

uniform sampler2D texMap;

void main()
{
    vec4 texColor = (texture2D(texMap, f_uvCoord.xy) * 0.5) + (0.5 * vec4(f_color,1.0));
    out_color = texColor;
}

