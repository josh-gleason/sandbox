#version 410

in vec3 v_position;
in vec3 v_normal;

// u_normalMatrix = transpose(inverse(u_mvMatrix))
uniform mat4 u_mvMatrix;
uniform mat4 u_normalMatrix;
uniform mat4 u_mvpMatrix;

out vec3 f_pos;
out vec3 f_normal;

void main()
{
    // we want the position and normal in view coordinates not projection
    f_pos = vec3(u_mvMatrix * vec4(v_position,1.0)).xyz;
    f_normal = normalize(u_normalMatrix * vec4(v_normal,1.0)).xyz;
    gl_Position = u_mvpMatrix * vec4(v_position,1.0);
}

