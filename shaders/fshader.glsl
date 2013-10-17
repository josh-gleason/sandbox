#version 410

// fragment properties
in vec3 f_normal;
in vec3 f_position;

struct LightInfo
{
    vec3 position;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
};

// light properties
layout(std140) uniform Lights
{
    uniform int count;
    uniform LightInfo info[8];
} lights;

// material properties
layout(std140) uniform Material
{
    uniform vec3  diffuse;
    uniform vec3  specular;
    uniform vec3  ambient;
    uniform float shininess;
    uniform float texBlend;
} material;

// output color
out vec4 out_color;

vec3 computeLighting(int idx)
{
    vec3 V = normalize(-f_position);
    vec3 L = normalize(lights.info[idx].position - f_position);
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

    vec3 id = lights.info[idx].diffuse;
    vec3 is = lights.info[idx].specular;
    vec3 ia = lights.info[idx].ambient;

    vec3 Ip = ka*ia + kd*max(dot(L,N),0)*id + ks*pow(max(dot(H,N),0.0),max(a,2))*is;

    return Ip;
}

void main()
{
    vec3 finalColor = vec3(0.0, 0.0, 0.0);
    for ( int i = 0; i < lights.count; ++i )
        finalColor = finalColor + computeLighting(i);
    out_color = vec4(finalColor, 1.0);
}
