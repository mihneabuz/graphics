#version 330 core

out vec4 fragColor;

in vec2 vertTexCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float blend;

void main() {
    vec4 color0 = texture(texture0, vertTexCoord);
    vec4 color1 = texture(texture1, vertTexCoord);
    fragColor = mix(color0, color1, blend * color1.a / color0.a);
}
