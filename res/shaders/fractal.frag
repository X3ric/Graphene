#version 330 core

uniform float iTime;         // Time in seconds
uniform vec2 iResolution;    // Resolution of the screen
uniform vec4 iMouse;         // Mouse position (xy), and click state (zw)
in vec2 texCoord;            // Texture coordinate from vertex shader
out vec4 fragColor;          // Output color

// Increase this for better anti-aliasing if you have a very fast GPU
#define AA 2
#define smooth 1.0
#define zoom (0.62 + 0.38 * cos(.07 * iTime))
#define colorchangespeed (7.5 + zoom)

float mandelbrot(in vec2 c) {
    #if 1
    {
        float c2 = dot(c, c);
        // Skip computation inside M1 and M2, for optimization
        if (256.0 * c2 * c2 - 96.0 * c2 + 32.0 * c.x - 3.0 < 0.0) return 0.0;
        if (16.0 * (c2 + 2.0 * c.x + 1.0) - 1.0 < 0.0) return 0.0;
    }
    #endif
    const float B = 256.0;
    float l = 0.0;
    vec2 z = vec2(0.0);
    for (int i = 0; i < 512; i++) {
        z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
        if (dot(z, z) > (B * B)) break;
        l += 1.0;
    }
    if (l > 511.0) return 0.0;
    float sl = l - log2(log2(dot(z, z))) + 4.0;
    l = mix(l, sl, smooth);
    return l;
}

vec3 generateColor(float time) {
    return vec3(
        0.5 + 0.5 * sin(0.1 * time + 0.0), // R
        0.5 + 0.5 * sin(0.1 * time + 2.0), // G
        0.5 + 0.5 * sin(0.1 * time + 4.0)  // B
    );
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec3 col = vec3(0.0);
    #if AA > 1
    // Anti-aliasing: sample the pixel multiple times
    for (int m = 0; m < AA; m++) {
        for (int n = 0; n < AA; n++) {
            vec2 p = (-iResolution.xy + 2.0 * (fragCoord.xy + vec2(float(m), float(n)) / float(AA))) / iResolution.y;
            float w = float(AA * m + n);
            float time = iTime + 0.5 * (1.0 / 24.0) * w / float(AA * AA);
    #else
            vec2 p = (-iResolution.xy + 2.0 * fragCoord.xy) / iResolution.y;
            float time = iTime;
    #endif
    
            // Dynamic zoom and rotation based on time
            float size = 1.0 * zoom;
            vec2 xy = vec2(p.x * size - p.y * size, p.x * size + p.y * size);
            // Generate color
            vec2 c = vec2(-0.745, 0.186) + xy * zoom;
            float l = mandelbrot(c);
            vec3 color = generateColor(iTime * colorchangespeed);
            col += 0.5 + 0.5 * cos(3.0 + l * 0.15 + color);
    #if AA > 1
        }
    }
    col /= float(AA * AA);
    #endif
    fragColor = vec4(col, 1.0);
}

void main() {
    mainImage(fragColor, gl_FragCoord.xy);
}
