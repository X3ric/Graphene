#include "../window.h"
#include "modules/ui.c"

Font font;

int main(int arglenght, char** args)
{ 
    WindowInit(1920, 1080, "Grafenic - Fps");
    font = LoadFont("./res/fonts/JetBrains.ttf");font.nearest = false;
    shaderfont.hotreloading = true;
    //window.debug.fps = true;
    //window.opt.vsync = true;
    //window.fpslimit = 60.0f;
    while (!WindowState())
    {
        WindowClear();
        // Modular ui.h functions
            Fps(0, 0, font, Scaling(450)); 
        WindowProcess();
    }
    WindowClose();
    return 0;
} 
