#version 330 core

// Uniforms
uniform sampler2D screenTexture;   // Background texture
uniform vec4 Color;                // Overlay color

in vec2 texCoord;                  // Texture coordinates
out vec4 fragColor;                // Final fragment color output

void main() {
    vec4 texColor = texture(screenTexture, texCoord);
    texColor.a = 1.0 - texColor.a;
    fragColor = texColor * Color;
}
