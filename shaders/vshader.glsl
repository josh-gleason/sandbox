#version 410

in vec3 v_position;
in vec3 v_normal;

out vec3 f_position;
out vec3 f_normal;

// normalMatrix = transpose(inverse(mvMatrix))
layout (std140) uniform Matrices
{
    uniform mat4 mvMatrix;
    uniform mat4 mvpMatrix;
    uniform mat4 normalMatrix;
};

void main()
{
    // we want the position and normal in view coordinates not projection
    vec4 viewPosition = Matrices.mvMatrix * vec4(v_position,1.0);
    f_position = viewPosition.xyz / viewPosition.w;
    f_normal = normalize(Matrices.normalMatrix * vec4(v_normal,1.0)).xyz;
    gl_Position = Matrices.mvpMatrix * vec4(v_position,1.0);
}

