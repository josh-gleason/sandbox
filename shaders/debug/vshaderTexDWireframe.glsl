#version 410

layout(location=0) in vec3 v_position;
layout(location=1) in vec3 v_normal;
layout(location=2) in vec2 v_uvCoord;

// normalMatrix = transpose(inverse(mvMatrix))
layout(std140) uniform Matrices
{
    uniform mat4 mvpMatrix;
    uniform mat4 mvMatrix;
    uniform mat4 normalMatrix;
} matrices;

out vec3 g_position;
out vec3 g_normal;
out vec2 g_uvCoord;

void main()
{
    // we want the position and normal in view coordinates not projection
    g_position = (matrices.mvMatrix * vec4(v_position,1.0)).xyz;
    g_normal = normalize(matrices.normalMatrix * vec4(v_normal,1.0)).xyz;
    g_uvCoord = v_uvCoord;
    gl_Position = matrices.mvpMatrix * vec4(v_position,1.0);
}

