#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 orthographic;

void main() {
    gl_Position = orthographic * vec4(position, 1.0);
    TexCoord = aTexCoord;
}
