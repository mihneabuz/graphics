#version 330 core

in vec3 fragPos;

out vec4 fragColor;

uniform vec3 lightPos;
uniform sampler2D texture0;

void main() {
    vec3 lightColor = vec3(1.0);
    vec3 fragNormal = vec3(0.0, 1.0, 0.0);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 lightDir = normalize(lightPos - fragPos);
    float diffuseStrength = max(dot(normalize(fragNormal), lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lightColor;

    fragColor = vec4((ambient + diffuse), 1.0) * texture(texture0, fragPos.xz);
}
