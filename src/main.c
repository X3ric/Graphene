#include "grafenic/init.c"
#include "grafenic/ui.c"

Font font;
Shader custom;

void update(void){
    // Shader on Screen "custom"
        int x = 0; int y = 0;
        int width = window.screen_width;
        int height = window.screen_width;
        Rect((RectObject){
            {x, y + height, 0.0f},         // Bottom Left
            {x + width, y + height, 0.0f}, // Bottom Right
            {x, y, 0.0f},                  // Top Left
            {x + width, y, 0.0f},          // Top Right
            custom,                        // Shader
            camera,                        // Camera
        });
    // Input Example
        //      Key                    MouseButton                      Info
        // isKey,isKeyReset  // isMouseButton,isMouseButtonReset  // key Bool
        // isKeyDown,isKeyUp // isMouseButtonDown,isMouseButtonUp // key Press/Relase 
        // isKeyPressed      //                                   // key Mantain 
        //if (isKeyPressed("A", 0.06f)) { // Repeat key every 0.06 seconds
        //    DrawCircle(mouse.x,mouse.y, Scaling(35), (Color){0, 0, 0, 75});
        //}
        //if(isKey("V")){ // Change Vsync state
        //    window.opt.vsync = true;
        //} else {
        //    window.opt.vsync = false;
        //}
    // Easing Lerp
        //int sizeball  = Lerp(0, 50,  Easing(Motion(1.0,0.5), "Linear"));
        //int positionx = Lerp(0, window.screen_width,  Easing(Motion(1.0,1.0), "CubicInOut"));
        //int positiony = Lerp(0, window.screen_height, Easing(Motion(1.0,1.0), "CubicInOut"));
        //DrawCircle(positionx, positiony, sizeball, VIOLET);
    // DrawLine Examples "Cross Screen"
        //DrawLine(0, 0, window.screen_width, window.screen_height, Scaling(5), BLACK);
        //DrawLine(0, window.screen_height, window.screen_width, 0, Scaling(5), BLACK);
    // DrawTriangle Examples
        //int x = window.screen_width / 2;
        //int y = window.screen_height;
        //static int size = 50;
        //if (isKeyDown("+")) {
        //    size += 1;
        //}
        //if (isKeyDown("-") && size > 0) {
        //    size -= 1;
        //}
        //DrawTriangle(x - size, y, x + size, y, x, y - size, PURPLE);
        //Zelda((TriangleObject){
        //{x - size, y, 0.0f},  // Vert0: x, y, z
        //{x + size, y, 0.0f},  // Vert1: x, y, z
        //{x, y - size, 0.0f},  // Vert2: x, y, z
        //shaderdefault,        // Shader
        //false                 // Pespective 
        //});
    // Modular ui.c functions
        // Left Text Bar
            //int texts = 9;
            //DrawTextColumn(font,1,texts, text("V = vsync state: %s", window.opt.vsync ? "ON" : "OFF"));
            //DrawTextColumn(font,2,texts, text("Esc = exitbar state: %s", isKey("Esc") ? "ON" : "OFF"));
        //Fps(0, 0, font, Scaling(50));
        ExitPromt(font);  
}

int main(int arglenght, char** args)
{ 
    WindowInit(1920, 1080, "Grafenic");
    font = LoadFont("./res/fonts/Monocraft.ttf");font.nearest = true;
    custom = LoadShader("./res/shaders/default.vert","./res/shaders/fractal.frag");
    custom.hotreloading = true;shaderdefault.hotreloading = true;
    ClearColor((Color){75, 75, 75,100});
    // Saving how many times you boot
        //char* path = "./data.txt";
        //char* boot = FileLoad(path);
        //boot = FileSave(path, text("%d", (textint(boot) + 1)));
        //printf("Booted times: %s\n", boot);
        ////FileClear(path); // clear all data
    // Audio inizialization
        //AudioInit();
        // Play a stream with audio
            //AudioPlay("./res/sounds/sound.wav");
        // Load a file edit and play
            //Sound* sound = SoundLoad("./res/sounds/sound.wav");
            //SetSoundPitchSemitones(sound,-24.0);
            //SoundPlay(sound);//SoundStop(sound);
    while (!WindowState())
    {
        WindowClear();
            update();
        WindowProcess();
    }
    WindowClose();
    return 0;
} 
