#version 330 core

out vec4 fragColor;

in vec2 vertTexCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float blend;

void main() {
    fragColor = mix(texture(texture0, vertTexCoord), texture(texture1, vertTexCoord), blend);
}
