typedef struct {
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
} Color;

#define LIGHTGRAY  (Color){ 200, 200, 200}   // Light Gray
#define GRAY       (Color){ 130, 130, 130}   // Gray
#define DARKGRAY   (Color){ 80, 80, 80}      // Dark Gray
#define YELLOW     (Color){ 253, 249, 0}     // Yellow
#define GOLD       (Color){ 255, 203, 0}     // Gold
#define ORANGE     (Color){ 255, 161, 0}     // Orange
#define PINK       (Color){ 255, 109, 194}   // Pink
#define RED        (Color){ 230, 41, 55}     // Red
#define MAROON     (Color){ 190, 33, 55}     // Maroon
#define GREEN      (Color){ 0, 228, 48}      // Green
#define LIME       (Color){ 0, 158, 47}      // Lime
#define DARKGREEN  (Color){ 0, 117, 44}      // Dark Green
#define SKYBLUE    (Color){ 102, 191, 255}   // Sky Blue
#define BLUE       (Color){ 0, 121, 241}     // Blue
#define DARKBLUE   (Color){ 0, 82, 172}      // Dark Blue
#define PURPLE     (Color){ 200, 122, 255}   // Purple
#define VIOLET     (Color){ 135, 60, 190}    // Violet
#define DARKPURPLE   (Color){ 112, 31, 126}    // Dark Purple
#define BEIGE      (Color){ 211, 176, 131}   // Beige
#define BROWN      (Color){ 127, 106, 79}    // Brown
#define DARKBROWN    (Color){ 76, 63, 47}      // Dark Brown
#define WHITE      (Color){ 255, 255, 255}   // White
#define BLACK      (Color){ 0, 0, 0}         // Black
#define MAGENTA    (Color){ 255, 0, 255}     // Magenta
#define BLANK        (Color){ 0, 0, 0}         // Blank (Transparent)

Color HexToColor(const char* hex) {
    Color color;
    if (strlen(hex) == 7) {
        sscanf(hex, "#%2hhx%2hhx%2hhx", &color.r, &color.g, &color.b);
        color.a = 255;
    } else if (strlen(hex) == 9) {
        sscanf(hex, "#%2hhx%2hhx%2hhx%2hhx", &color.r, &color.g, &color.b, &color.a);
    } else {
        color = (Color){0, 0, 0, 255};
    }
    return color;
}

void glColor(Color color){
    if (color.a == 0) { color.a = 255; }
    glColor4f((GLclampf)color.r/255.0f, (GLclampf)color.g/255.0f, (GLclampf)color.b/255.0f, (GLclampf)color.a/255.0f);
}

void ClearColor(Color color) {
    if (color.a == 0) { color.a = 255; }
    glClearColor((GLclampf)color.r/255.0f, (GLclampf)color.g/255.0f, (GLclampf)color.b/255.0f, (GLclampf)color.a/255.0f);
}
