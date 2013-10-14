#version 410

// fragment properties
in vec3 f_normal;
in vec3 f_pos;
in vec2 f_uvCoord;

// light properties
uniform vec3 u_lightPos;
uniform vec3 u_lightDiffuse;
uniform vec3 u_lightSpecular;
uniform vec3 u_lightAmbient;

// material properties
uniform ub_Material
{
    uniform vec3  u_diffuse;
    uniform vec3  u_specular;
    uniform vec3  u_ambient;
    uniform float u_shininess;
};

// texture properties
uniform sampler2D u_diffuseMap;
uniform float u_texBlend;

// output color
out vec4 out_color;

void main()
{
    //vec3 N = normalize(tbn * (texture2D(u_bumpMap, f_uvCoord) * 2.0 - 1.0).xyz);
    vec3 N = normalize(f_normal);
    vec3 V = normalize(-f_pos);
    vec3 L = normalize(u_lightPos + V);
    vec3 R = normalize(2.0 * dot(L,N)*N - L);

    vec3 kd = mix(u_diffuse, texture2D(u_diffuseMap, f_uvCoord).xyz, u_texBlend);
    vec3 ks = u_specular;
    vec3 ka = u_ambient;
    float a = u_shininess;

    vec3 id = u_lightDiffuse;
    vec3 is = u_lightSpecular;
    vec3 ia = u_lightAmbient;

    vec3 Ip = ka*ia + kd*max(dot(L,N),0)*id + ks*pow(max(dot(R,V),0),a)*is;

    out_color = vec4(Ip,1.0);
}

