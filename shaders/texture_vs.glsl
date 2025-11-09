#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 vertTexCoord;

uniform mat3 rotation;

void main() {
    gl_Position = vec4(rotation * aPos, 1.0);
    vertTexCoord = aTexCoord;
}
