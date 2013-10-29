#version 420

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 g_normal[3];
in vec3 g_position[3];

noperspective out vec3 f_coords;
out vec3 f_normal;
out vec3 f_position;

void main()
{
    vec2 p0 = gl_in[0].gl_Position.xy/gl_in[0].gl_Position.w;
    vec2 p1 = gl_in[1].gl_Position.xy/gl_in[1].gl_Position.w;
    vec2 p2 = gl_in[2].gl_Position.xy/gl_in[2].gl_Position.w;
    vec2 v0 = p2 - p1;
    vec2 v1 = p2 - p0;
    vec2 v2 = p1 - p0; 

    float area = abs(v1.x*v2.y - v1.y * v2.x);
/*    float area = abs(p0.x*(p1.y - p2.y) +
                     p1.x*(p2.y - p0.y) +
                     p2.x*(p0.y - p1.y)) * 0.5f;
    */
    f_coords = vec3(area/length(v0), 0.0, 0.0);
    f_normal = g_normal[0];
    f_position = g_position[0];
    gl_Position = gl_in[0].gl_Position;
    f_coords = vec3(1.0, 0.0, 0.0);
    EmitVertex();
    
    f_coords = vec3(0.0, area/length(v1), 0.0);
    f_normal = g_normal[1];
    f_position = g_position[1];
    gl_Position = gl_in[1].gl_Position;
    f_coords = vec3(0.0, 1.0, 0.0);
    EmitVertex();
    
    f_coords = vec3(0.0, 0.0, area/length(v2));
    f_normal = g_normal[2];
    f_position = g_position[2];
    gl_Position = gl_in[2].gl_Position;
    f_coords = vec3(0.0, 0.0, 1.0);
    EmitVertex();
}

