#version 330 core

uniform float iTime;           // Time in seconds
uniform vec2 iResolution;      // Resolution of the screen
uniform vec2 iMouse;           // Mouse position (x, y)

in vec2 texCoord;              // Texture coordinate from vertex shader
out vec4 fragColor;            // Output color

#define AA 2                   // Anti-aliasing factor
#define ZOOM_SPEED 15.0        // Speed of zooming (higher values = faster zoom)
#define ZOOM_EXPONENT 1.00     // Exponent for zoom (affects zoom rate)
#define SENSITIVITY_BASE 1.0   // Base sensitivity value (affects mouse sensitivity scaling)
#define SENSITIVITY_SCALE 0.5  // Sensitivity scaling factor (affects how sensitivity decreases with zoom)
#define SMOOTH 1.0             // Smooth factor for Mandelbrot set rendering

float mandelbrot(in vec2 c) {
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
    return mix(l, sl, SMOOTH);
}

vec3 generateColor(float time) {
    return vec3(
        0.5 + 0.5 * sin(0.1 * time + 0.0),
        0.5 + 0.5 * sin(0.1 * time + 2.0),
        0.5 + 0.5 * sin(0.1 * time + 4.0)
    );
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec3 col = vec3(0.0);
    vec2 mouseNormalized = vec2(iMouse.x / iResolution.x - 0.5, iMouse.y / iResolution.y - 0.5) * 2.0;
    mouseNormalized.y = -mouseNormalized.y;
    float zoomFactor = ZOOM_SPEED * iTime;
    float zoom = pow(zoomFactor, ZOOM_EXPONENT);
    vec2 zoomCenter = mouseNormalized;
    #if AA > 1
    for (int m = 0; m < AA; m++) {
        for (int n = 0; n < AA; n++) {
            vec2 p = (-iResolution.xy + 2.0 * (fragCoord.xy + vec2(float(m), float(n)) / float(AA))) / iResolution.y;
            float w = float(AA * m + n);
            float time = iTime + 0.5 * (1.0 / 24.0) * w / float(AA * AA);
    #else
            vec2 p = (-iResolution.xy + 2.0 * fragCoord.xy) / iResolution.y;
            float time = iTime;
    #endif
            vec2 c = (p - zoomCenter) / zoom + zoomCenter;
            float l = mandelbrot(c);
            vec3 color = generateColor(time);
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
