#version 330 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;

out vec4 fragColor;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct MaterialMap {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform bool useTexture;
uniform Material material;
uniform MaterialMap materialMap;

struct Light {
    vec3 pos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(light.pos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);

    float dist = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    vec3 ambientColor = useTexture ? texture(materialMap.diffuse, fragTexCoords).rgb : material.ambient;
    vec3 diffuseColor = useTexture ? texture(materialMap.diffuse, fragTexCoords).rgb : material.diffuse;
    vec3 specularColor = useTexture ? texture(materialMap.specular, fragTexCoords).rgb : material.specular;
    float shininess = useTexture ? materialMap.shininess : material.shininess;

    vec3 ambient = light.ambient * ambientColor * attenuation;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * diffuseColor) * attenuation;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.specular * (spec * specularColor) * attenuation;

    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}
