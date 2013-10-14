#version 410

in vec3 v_position;
in vec3 v_normal;

out vec3 f_position;
out vec3 f_normal;

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
    f_position = (matrices.mvMatrix * vec4(v_position,1.0)).xyz;
//    vec4 viewPosition = matrices.mvMatrix * vec4(v_position,1.0);
//    f_position = viewPosition.xyz / viewPosition.w;
    f_normal = normalize(matrices.normalMatrix * vec4(v_normal,1.0)).xyz;
    gl_Position = matrices.mvpMatrix * vec4(v_position,1.0);
}

