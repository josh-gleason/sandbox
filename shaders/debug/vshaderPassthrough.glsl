#version 410

layout(location=0) in vec3 v_position;
layout(location=5) in vec3 v_color;

// normalMatrix = transpose(inverse(mvMatrix))
layout(std140) uniform Matrices
{
    uniform mat4 mvpMatrix;
    uniform mat4 mvMatrix;
    uniform mat4 normalMatrix;
} matrices;

out vec3 f_color;

void main()
{
    // we want the position and normal in view coordinates not projection
    f_color = v_color;
    gl_Position = matrices.mvpMatrix * vec4(v_position,1.0);
}

