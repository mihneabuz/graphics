#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 dir;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 pos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

#define MAX_POINT_LIGHTS 4

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;

out vec4 fragColor;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 ViewDir);

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = CalcDirLight(dirLight, normal, viewDir);

    for (int i = 0; i < MAX_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], normal, fragPos, viewDir);

    fragColor = vec4(result, 1);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    return vec3(texture(material.diffuse, fragTexCoords));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 ViewDir) {
    return vec3(0);
}
