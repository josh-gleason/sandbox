#version 410

// fragment properties
in vec3 f_normal;
in vec3 f_position;

// output color
out vec4 out_color;

// light properties
layout (std140) uniform Light
{
    uniform vec3 position;
    uniform vec3 diffuse;
    uniform vec3 specular;
    uniform vec3 ambient;
} light;

// material properties
layout (std140) uniform Material
{
    uniform vec3  diffuse;
    uniform vec3  specular;
    uniform vec3  ambient;
    uniform float shininess;
} material;

void main()
{
    vec3 V = normalize(-f_position);
    vec3 L = normalize(light.position - f_position);
    vec3 N = normalize(f_normal);
    vec3 H = normalize(L + V);

    vec3 kd = material.diffuse;
    vec3 ks = material.specular;
    vec3 ka = material.ambient;
    float a = material.shininess;
   
    // remove specular highlight if light is behind face
    //if( dot(L, N) < 0.0 )
    //    ks = vec3(0.0, 0.0, 0.0);
    // optimized version of above
    float t = dot(L,N);
    ks *= max(t,0.0)/max(t,1e-10);

    vec3 id = light.diffuse;
    vec3 is = light.specular;
    vec3 ia = light.ambient;

    vec3 Ip = ka*ia + kd*max(dot(L,N),0)*id + ks*pow(max(dot(H,N),0.0),max(a,2))*is;

    out_color = vec4(Ip,1.0);
}

