#version 330 core

uniform vec2 iMouse;
uniform float iTime;
uniform vec2 iResolution;
uniform sampler2D screenTexture;

uniform vec4 Color;

in vec2 texCoord;
out vec4 fragColor;

void mainImage(in vec2 texCoord, in vec2 fragCoord, out vec4 fragColor) {
    fragColor = texture(screenTexture, texCoord) * Color;
}

void main() {
    mainImage(texCoord, gl_FragCoord.xy, fragColor);
}
