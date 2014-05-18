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

// attenuation (constant, linear, quadratic)
// attenuation = 1/(x + y*d + z*d*d)
// Things are BRIGHTER the closer the coefficient is to 1
const vec3 ATTENUATION_COEF = vec3(0.1, 0.0, 0.98);

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

    float dist = length(L);
    float attenuation = clamp(0.0, 1.0, 1.0/(
            ATTENUATION_COEF.x +
            ATTENUATION_COEF.y * dist +
            ATTENUATION_COEF.z * dist * dist
    ));

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
    
    return Ip*attenuation;
}

void main()
{
    vec3 finalColor = vec3(0.0, 0.0, 0.0);
    for ( int i = 0; i < lights.count; ++i )
        finalColor = finalColor + computeLighting(i);
    out_color = vec4(finalColor, 1.0);
}
