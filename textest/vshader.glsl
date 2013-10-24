#version 420

layout(location=0) in vec3 vPosition;
layout(location=1) in vec2 vUvCoord;

out vec2 fUvCoord;

void main()
{
    fUvCoord = vUvCoord;
    gl_Position = vec4(vPosition,1.0);
}

