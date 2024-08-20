#include "grafenic/init.c"
#include "grafenic/ui.c"

Font font;
Image img;
Shader custom;
Color pixelColor;

void update(void){
    // Movement camera.transform
        double speed;
        double clampz;
        clampz = (window.deltatime);
        //print(text("Cam lerp: %.5f\n", clampz));
        if (isKeyDown("LeftShift")) {
            speed = 0.1f;
        } else {
            speed = 0.05f;
        }
        float movement_smothing = 0.75f;
        if (isKeyDown("w")) {
            camera.transform.position.y = Lerp(camera.transform.position.y, camera.transform.position.y + speed * clampz, movement_smothing);
        } else if (isKeyDown("s")) {
            camera.transform.position.y = Lerp(camera.transform.position.y, camera.transform.position.y - speed * clampz, movement_smothing);
        }
        if (isKeyDown("a")) {
            camera.transform.position.x = Lerp(camera.transform.position.x, camera.transform.position.x + speed * clampz, movement_smothing);
        } else if (isKeyDown("d")) {
            camera.transform.position.x = Lerp(camera.transform.position.x, camera.transform.position.x - speed * clampz, movement_smothing);
        }
        if (isKeyDown("r")) {
            speed = 0.0f;
            camera.transform.position.x = Lerp(camera.transform.position.x, 0.0f, 0.0003f * clampz);
            camera.transform.position.y = Lerp(camera.transform.position.y, 0.0f, 0.0003f * clampz);
            camera.transform.position.z = Lerp(camera.transform.position.z, 1.0f, 0.0003f * clampz);
            camera.transform.rotation.z = Lerp(camera.transform.rotation.z, (GLfloat)0.0f, 0.0003f * clampz);
            mouse.scroll.y = 0.0f;
        } else {
            if (camera.transform.position.z == 0.0f) {
                camera.transform.position.z = Lerp(1.0f, mouse.scroll.y * 0.1f, 0.0003f * clampz);
            } else {
                camera.transform.position.z = Lerp(camera.transform.position.z, mouse.scroll.y * 0.1f + 1.0f , 0.0003f * clampz);
            }
        }
        if (mouse.scroll.y <= 0) {mouse.scroll.y = 0;}
        if (isKeyDown("e")) {
           camera.transform.rotation.y -= (speed / 1000.0f) * clampz;
        } else if (isKeyDown("q")) {
           camera.transform.rotation.z += (speed / 1000.0f) * clampz;
        }
    // DrawImage
        DrawImageShader(img,0, 0, window.screen_width, window.screen_height,0,custom);
    // Getting Pixel Pointed Color 
        // Uncomment "GetPixel Pointed color" in main
        //int fontsize = Scaling(50);
        //const char* textContent = text("R:%d G:%d B:%d A:%d", pixelColor.r, pixelColor.g, pixelColor.b, pixelColor.a);
        //TextSize textSize = GetTextSize(font, fontsize, textContent);
        // DrawText(window.screen_width/2-(textSize.width/2), 0, font, fontsize, textContent, WHITE);
    // Modular ui.c functions
        Fps(0, 0, font, Scaling(50));
        ExitPromt(font);  
} 

int main(int arglenght, char** args)
{ 
    WindowInit(1920, 1080, "Grafenic");
    custom = LoadShader("./res/shaders/default.vert","./res/shaders/default.frag");
    custom.hotreloading = true;//hotreloading for the shader or put this in the update "shader = ShaderHotReload(shader);" > in this case > "custom = ShaderHotReload(custom);"
    shaderdefault.hotreloading = true;// hot reload on the default pixel shader
    font = LoadFont("./res/fonts/Monocraft.ttf");font.nearest = true;
    img = LoadImage("./res/images/Test.png");
    ClearColor((Color){75, 75, 75,100});
    while (!WindowState())
    {
        WindowClear();
            update();
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
