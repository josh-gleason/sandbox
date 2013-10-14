#version 410

in vec3 v_position;
in vec3 v_normal;

// normalMatrix = transpose(inverse(mvMatrix))
layout (std140) uniform Matrices
{
    uniform mat4 mvpMatrix;
    uniform mat4 mvMatrix;
    uniform mat4 normalMatrix;
} matrices;

void main()
{
    // we want the position and normal in view coordinates not projection
    gl_Position = matrices.mvpMatrix * vec4(v_position,1.0);
}

