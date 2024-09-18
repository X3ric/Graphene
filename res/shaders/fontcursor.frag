#version 330 core

uniform sampler2D Texture;
uniform vec2 iResolution;
uniform vec2 iMouse;
uniform float iTime;
uniform vec4 Color;
uniform float Size;

in vec2 texCoord;
out vec4 fragColor;

float SmoothEdge(float alpha, float edgeWidth) {
    return smoothstep(0.5 - edgeWidth, 0.5 + edgeWidth, alpha);
}

vec3 SubpixelRender(vec2 texCoord, sampler2D texture, float edgeWidth, vec2 resolution) {
    vec2 subpixelOffset = vec2(1.0 / resolution.x, 0.0);
    float alphaR = SmoothEdge(texture2D(texture, texCoord - subpixelOffset).a, edgeWidth);
    float alphaG = SmoothEdge(texture2D(texture, texCoord).a, edgeWidth);
    float alphaB = SmoothEdge(texture2D(texture, texCoord + subpixelOffset).a, edgeWidth);
    return vec3(alphaR, alphaG, alphaB);
}

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
    vec3 subpixelAlpha = SubpixelRender(texCoord, Texture, edgeWidth, iResolution);
    vec4 texColor = vec4(Color.rgb * subpixelAlpha, max(subpixelAlpha.r, max(subpixelAlpha.g, subpixelAlpha.b)) * Color.a);
    texColor.a = -texColor.a;
    fragColor = 1.0 - texColor * Color;
}

