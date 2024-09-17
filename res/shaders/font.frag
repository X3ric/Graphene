#version 330 core

uniform sampler2D Texture;
uniform vec2 iResolution;
uniform vec2 iMouse;
uniform float iTime;
uniform vec4 Color;
uniform float Size;

in vec2 texCoord;
out vec4 fragColor;

void main() {
    vec2 subpixelOffset = vec2(1.0 / iResolution.x, 0.0);
    float minSize = 0.001;
    float minEdge = 0.0;
    float maxSize = 32.0;
    float maxEdge = 0.1;
    float edgeWidth;
    if (Size <= minSize) {
        edgeWidth = minEdge;
    } else if (Size >= maxSize) {
        edgeWidth = maxEdge;
    } else {
        edgeWidth = minEdge + ((Size - minSize) / (maxSize - minSize)) * (maxEdge - minEdge);
    }
    float alphaR = smoothstep(0.5 - edgeWidth, 0.5 + edgeWidth, texture(Texture, texCoord - subpixelOffset).a); // Red subpixel
    float alphaG = smoothstep(0.5 - edgeWidth, 0.5 + edgeWidth, texture(Texture, texCoord).a);                  // Green subpixel
    float alphaB = smoothstep(0.5 - edgeWidth, 0.5 + edgeWidth, texture(Texture, texCoord + subpixelOffset).a); // Blue subpixel
    vec3 subpixelAlpha = vec3(alphaR, alphaG, alphaB);
    fragColor = vec4(Color.rgb * subpixelAlpha, max(subpixelAlpha.r, max(subpixelAlpha.g, subpixelAlpha.b)) * Color.a);
}
