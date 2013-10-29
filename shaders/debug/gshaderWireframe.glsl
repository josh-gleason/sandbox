#version 420

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 g_normal[3];
in vec3 g_position[3];

uniform vec2 u_windowSize;

// don't divide by the w value of the position for this attribute
noperspective out vec3 f_coords;

out vec3 f_normal;
out vec3 f_position;

void main()
{
    vec2 p0 = u_windowSize * gl_in[0].gl_Position.xy/gl_in[0].gl_Position.w;
    vec2 p1 = u_windowSize * gl_in[1].gl_Position.xy/gl_in[1].gl_Position.w;
    vec2 p2 = u_windowSize * gl_in[2].gl_Position.xy/gl_in[2].gl_Position.w;
    vec2 v0 = p2 - p1;
    vec2 v1 = p2 - p0;
    vec2 v2 = p1 - p0; 

    float area = abs(p0.x*(p1.y - p2.y) +
                     p1.x*(p2.y - p0.y) +
                     p2.x*(p0.y - p1.y)) * 0.5f;
    
    float d0 = area/length(v0);
    float d1 = area/length(v1);
    float d2 = area/length(v2);

    f_coords = vec3(d0, 0.0, 0.0);
    f_normal = g_normal[0];
    f_position = g_position[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    
    f_coords = vec3(0.0, d1, 0.0);
    f_normal = g_normal[1];
    f_position = g_position[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    
    f_coords = vec3(0.0, 0.0, d2);
    f_normal = g_normal[2];
    f_position = g_position[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
}

