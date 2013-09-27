#version 410

in vec3 f_color;
out vec4 gl_FragColor;

void main()
{
    gl_FragColor = vec4(f_color, 1.0);
}

