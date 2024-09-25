#include "../window.h"
#include "modules/ui.c"

Font font;
Img img;
Shader custom;
Color pixelColor;

int main(int arglenght, char** args)
{ 
    WindowInit(1920, 1080, "Grafenic - Image");
    custom = LoadShader("./res/shaders/default.vert","./res/shaders/default.frag");
    custom.hotreloading = true;//hotreloading for the shader or put this in the update "shader = ShaderHotReload(shader);" > in this case > "custom = ShaderHotReload(custom);"
    shaderdefault.hotreloading = true;// hot reload on the default pixel shader
    font = LoadFont("./res/fonts/JetBrains.ttf");font.nearest = false;
    img = LoadImage((ImgInfo){"./res/images/Test.png"});
    while (!WindowState())
    {
        WindowClear();
        // Movement camera.transform
            double speed;
            //print(text("Cam lerp: %.5f\n", (float)window.deltatime));
            if (isKeyDown("LeftShift")) {
                speed = 0.1f;
            } else {
                speed = 0.05f;
            }
            float movement_smothing = 0.75f;
            if (isKeyDown("w")) {
                camera.transform.position.y = Lerp(camera.transform.position.y, camera.transform.position.y + speed * 7500 * (float)window.deltatime, movement_smothing);
            } else if (isKeyDown("s")) {
                camera.transform.position.y = Lerp(camera.transform.position.y, camera.transform.position.y - speed * 7500 * (float)window.deltatime, movement_smothing);
            }
            if (isKeyDown("a")) {
                camera.transform.position.x = Lerp(camera.transform.position.x, camera.transform.position.x + speed * 7500 * (float)window.deltatime, movement_smothing);
            } else if (isKeyDown("d")) {
                camera.transform.position.x = Lerp(camera.transform.position.x, camera.transform.position.x - speed * 7500 * (float)window.deltatime, movement_smothing);
            }
            if (isKeyDown("r")) {
                speed = 0.0f;
                camera.transform.position.x = Lerp(camera.transform.position.x, 0.0f, 5.0f * (float)window.deltatime);
                camera.transform.position.y = Lerp(camera.transform.position.y, 0.0f, 5.0f * (float)window.deltatime);
                camera.transform.position.z = Lerp(camera.transform.position.z, 1.0f, 5.0f * (float)window.deltatime);
                camera.transform.rotation.z = Lerp(camera.transform.rotation.z, (GLfloat)0.0f, 5.0f * (float)window.deltatime);
                mouse.scroll.y = 0.0f;
            } else {
                if (camera.transform.position.z < 0.0f) {
                    camera.transform.position.z = Lerp(1.0f, mouse.scroll.y * 0.1f, 5.0f * (float)window.deltatime);
                } else {
                    camera.transform.position.z = Lerp(camera.transform.position.z, mouse.scroll.y * 0.1f + 1.0f , 5.0f * (float)window.deltatime);
                }
            }
            if (mouse.scroll.y <= 0) {mouse.scroll.y = 0;}
            if (isKeyDown("e")) {
                camera.transform.rotation.z -= speed * (float)window.deltatime;
            } else if (isKeyDown("q")) {
                camera.transform.rotation.z += speed * (float)window.deltatime;
            }
        // DrawImage
            DrawImageShader(img,0, 0, window.screen_width, window.screen_height,0,custom);
        // Getting Pixel Pointed Color 
            // Uncomment "GetPixel Pointed color" in main
            //int fontsize = Scaling(50);
            //const char* textContent = text("R:%d G:%d B:%d A:%d", pixelColor.r, pixelColor.g, pixelColor.b, pixelColor.a);
            //TextSize textSize = GetTextSize(font, fontsize, textContent);
            // DrawText(window.screen_width/2-(textSize.width/2), 0, font, fontsize, textContent, WHITE);
        // Modular ui.h functions
            Fps(0, 0, font, Scaling(50));
            ExitPromt(font);  
        WindowProcess();
        // GetPixel Pointed color "slow"
            //pixelColor = GetPixel(mouse.x, mouse.y);
        // Screenshot
            if(isKey("F11")){
                SaveScreenshot("Screenshot.png", 0, 0, window.screen_width, window.screen_height);
                isKeyReset("F11");
            }
    }
    WindowClose();
    DeleteShader(custom);
    return 0;
} 
