#version 420

uniform sampler2D texMap;

in vec2 fUvCoord;
out vec4 color;

void main()
{
    color = texture2D(texMap, fUvCoord);    
}

