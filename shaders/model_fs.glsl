#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 pos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;

out vec4 fragColor;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 ViewDir);

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPos);
    fragColor = vec4(CalcLight(light, normal, fragPos, viewDir), 1);
}

vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.pos - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = vec3(texture(material.diffuse, fragTexCoords));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularColor = vec3(texture(material.specular, fragTexCoords));

    float dist = length(light.pos - fragPos);
    float attenuation = 1 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    vec3 ambient = attenuation * light.ambient * diffuseColor;
    vec3 diffuse = attenuation * light.diffuse * diff * diffuseColor;
    vec3 specular = attenuation * light.specular * spec * specularColor;

    return ambient + diffuse + specular + diffuseColor;
}
