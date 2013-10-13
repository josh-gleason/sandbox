#version 410

in vec3 v_position;
in vec3 v_normal;
in vec2 v_uvCoord;

uniform mat4 u_mvMatrix;
uniform mat4 u_mvpMatrix;
uniform mat4 u_normalMatrix;  // for bump mapping {u_normalMatrix = transpose(inverse(u_mvMatrix))}

out vec3 f_pos;
out vec3 f_normal;
out vec2 f_uvCoord;

void main()
{
    // we want the position and normal in view coordinates not projection
    f_pos = vec3(u_mvMatrix * vec4(v_position,1.0)).xyz;
    f_normal = normalize(u_normalMatrix * vec4(v_normal,1.0)).xyz;
    f_uvCoord = v_uvCoord;
    gl_Position = u_mvpMatrix * vec4(v_position,1.0);
}

