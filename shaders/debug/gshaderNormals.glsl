#version 410

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

layout(std140) uniform Matrices
{
    uniform mat4 mvpMatrix;
    uniform mat4 mvMatrix;
    uniform mat4 normalMatrix;
} matrices;

uniform mat4 u_projectionMatrix;

in vec3 g_normal[3];

void main()
{
    for ( int i = 0; i < 3; ++i )
    {
        gl_Position = matrices.mvpMatrix * gl_in[i].gl_Position;
        EmitVertex();
        
        vec4 normal = matrices.normalMatrix * vec4(g_normal[i],1.0);
        //normal = normal / normal.w;
        vec4 position = matrices.mvMatrix * gl_in[i].gl_Position;
        //position = position / position.w;
        
        gl_Position = u_projectionMatrix * vec4(position.xyz + 0.05 * normal.xyz, 1.0);
        EmitVertex();

        EndPrimitive();
    }
}

