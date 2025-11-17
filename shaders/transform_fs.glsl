#version 330 core

out vec4 fragColor;

in vec2 vertTexCoord;

uniform sampler2D texture0;

void main() {
    fragColor = texture(texture0, vertTexCoord);
}
