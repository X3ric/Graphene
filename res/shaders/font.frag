#version 330 core

uniform sampler2D Texture;
uniform vec2 iResolution;
uniform vec2 iMouse;
uniform float iTime;
uniform vec4 Color;
uniform float Size;

in vec2 texCoord;
out vec4 fragColor;

void mainImage(in vec2 texCoord, in vec2 fragCoord, out vec4 fragColor) {
    //fragColor = vec4(texCoord, 0.0, 1.0); // uv debug
    fragColor = texture(Texture, texCoord) * Color;
}

void main() {
    mainImage(texCoord, gl_FragCoord.xy, fragColor);
}