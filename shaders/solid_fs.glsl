#version 330 core

out vec4 fragColor;

uniform vec3 solidColor;

void main() {
    fragColor = vec4(solidColor, 1.0);
}
