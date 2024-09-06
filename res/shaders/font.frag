#version 330 core
precision highp float;
uniform sampler2D fontTexture;
in vec2 texCoord;
out vec4 fragColor;

void main() {
    fragColor = texture(fontTexture, texCoord);
}
