#version 330 core

in vec3 worldPos;

out vec4 fragColor;

uniform sampler2D texture0;

void main() {
    fragColor = texture(texture0, worldPos.xz);
}
