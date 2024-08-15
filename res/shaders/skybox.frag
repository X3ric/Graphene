#version 330 core

uniform float iTime;         // Time in seconds
uniform vec2 iResolution;   // Resolution of the screen

in vec2 texCoord;           // Texture coordinate from vertex shader
out vec4 fragColor;         // Output color

// Function to create smooth color transitions
vec3 smoothColorTransition(vec2 uv, float time) {
    vec3 color1 = vec3(0.2, 0.3, 0.8); // Soft Blue
    vec3 color2 = vec3(0.9, 0.4, 0.6); // Warm Pink
    float shift = 0.5 + 0.5 * sin(time * 0.2); // Smooth color shift
    return mix(color1, color2, shift);
}

// Function to create subtle animated patterns
vec3 animatedPatterns(vec2 uv, float time) {
    float n = sin(dot(uv * 10.0, vec2(0.1, 0.2)) + time * 0.5);
    return vec3(0.5 + 0.5 * n, 0.5 + 0.5 * n * 0.8, 0.5 + 0.5 * n * 0.5);
}

// Function to create soft bloom and glow effect
vec3 softBloomEffect(vec3 color, float time) {
    float brightness = max(max(color.r, color.g), color.b);
    vec3 bloom = vec3(1.0) * pow(brightness, 0.5) * 0.2; // Soft bloom
    return color + bloom;
}

void main() {
    vec2 uv = texCoord;

    // Create smooth color transitions
    vec3 backgroundColor = smoothColorTransition(uv, iTime);
    
    // Add subtle animated patterns
    vec3 patterns = animatedPatterns(uv, iTime);
    
    // Combine background and patterns
    vec3 combinedColor = mix(backgroundColor, patterns, 0.6);
    
    // Apply soft bloom effect
    vec3 finalColor = softBloomEffect(combinedColor, iTime);
    
    // Output final color with full opacity
    fragColor = vec4(finalColor, 1.0);
}
