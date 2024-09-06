#include "grafenic/init.c"
#include "grafenic/ui.c"

Font font;
Shader custom;

void update(void){
    // Shader on Screen "custom"
        int x = 0; int y = 0;
        int width = window.screen_width;
        int height = window.screen_height;
        Rect((RectObject){
            {x, y + height, 0.0f},         // Bottom Left
            {x + width, y + height, 0.0f}, // Bottom Right
            {x, y, 0.0f},                  // Top Left
            {x + width, y, 0.0f},          // Top Right
            custom,                        // Shader
            camera,                        // Camera
        });
    // Modular ui.c functions
        //Fps(0, 0, font, Scaling(50));
        ExitPromt(font);  
}

int main(int arglenght, char** args)
{ 
    window.opt.disablecursor = true;
    WindowInit(1920, 1080, "Grafenic - Fractal");
    font = LoadFont("./res/fonts/Monocraft.ttf");font.nearest = true;
    custom = LoadShader("./res/shaders/default.vert","./res/shaders/fractal.frag");
    ClearColor((Color){75, 75, 75,100});
    while (!WindowState())
    {
        WindowClear();
            update();
        WindowProcess();
    }
    WindowClose();
    return 0;
} 
