#version 330 core

out vec4 fragColor;

in vec2 vertTexCoord;

uniform sampler2D texture0;

void main() {
    vec4 color0 = texture(texture0, vertTexCoord);
    float dist = 1.2 - abs(vertTexCoord.x - 0.5) - abs(vertTexCoord.y - 0.5);
    vec4 color1 = vec4(0.24, 0.68, 0.42, 1) * dist;
    fragColor = mix(color0, color1, 1 - color0.a);
}
