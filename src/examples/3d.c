#include "../window.h"
#include "modules/ui.c"

Font font;
Img img;
Vec3 cube;
double lastscrolly = 0.0;
double targetZ = 0.0;

Camera cam = {
    {
    0.0f, 0.0f, 0.0f,   // Position: x, y, z
    0.0f, 0.0f, 0.0f,   // LocalPosition: x, y, z
    0.0f, 0.0f, 0.0f    // Rotation: x, y, z
    },
    60.0f,              // Fov 0 means Orthographic
    1000.0f,            // Far Distance
    0.0f                // Near Distance 
};

void Update(void) {
    // Movement Camera
        double speed;
        //print(text("Cam lerp: %.5f\n", (float)window.deltatime));
        if (isKeyDown("LeftShift")) {
            speed = 0.3f;
        } else {
            speed = 0.15f;
        }
        if (isKeyDown("w")) {
            cube.y += speed * (float)window.deltatime;
        } else if (isKeyDown("s")) {
            cube.y -= speed * (float)window.deltatime;
        }
        if (isKeyDown("a")) {
            cube.x -= speed * (float)window.deltatime;
        } else if (isKeyDown("d")) {
            cube.x += speed * (float)window.deltatime;
        }
        if (isKeyDown("e")) {
            targetZ = targetZ + speed * (float)window.deltatime;
        } else if (isKeyDown("q")) {
            targetZ = targetZ - speed * (float)window.deltatime;
        }
        if (isKeyDown("r")) {
            speed = 0.0f;
            targetZ = 0.0f;
            mouse.scroll.y = 0;
            lastscrolly = mouse.scroll.y;
            cube.x = Lerp(cube.x, 0.0f, 5.0f * (float)window.deltatime);
            cube.y = Lerp(cube.y, 0.0f, 5.0f * (float)window.deltatime);
            cube.z = Lerp(cube.z, 0.0f, 5.0f * (float)window.deltatime);
        } else {
            if (lastscrolly != mouse.scroll.y) {
                targetZ = cube.z + (mouse.scroll.y - lastscrolly);
                mouse.scroll.y = 0;
            }
            cube.z = Lerp(cube.z, targetZ, 5.0f * (float)window.deltatime);
            lastscrolly = mouse.scroll.y;
        }
    // Rotation Vec3
        Vec3 rot;
        rot.x = (float)window.time * 0.5f;
        rot.y = (float)window.time * 0.6f;
        rot.z = (float)window.time * 0.7f;
    // 3d envoiriment
        //window.debug.point = true;
        //window.debug.pointsize = 10.0f;
        //window.debug.wireframe = true; //window.debug single part
        //DrawCube(1.0f, cube.x, cube.y, cube.z, rot.x, rot.y, rot.z, GRAY);
        BindImg(img);
        Cube((CubeObject){{
            cube.x, cube.y, cube.z, // Position: x, y, z
            0.0f, 0.0f, 0.0f,       // LocalPosition: x, y, z
            rot.x, rot.y, rot.z,    // Rotation: x, y, z
        }, 1.0,                     // Size
        shaderdefault,              // Shader
        cam,                        // Camera
        });
        //window.debug.point = false;
        //window.debug.wireframe = false; //stop debugging
    // Modular ui.h functions
        Fps(0, 0, font, Scaling(35));
        ExitPromt(font);  
}

int main(int argc, char** argv) {
    WindowInit(1920, 1080, "Grafenic - 3d");
    font = LoadFont("./res/fonts/Monocraft.ttf");font.nearest = true;
    img = LoadImage((ImgInfo){"./res/images/Stone.png", true});
    shaderdefault.hotreloading = true;
    while (!WindowState()) {
        WindowClear();
        Update();
        WindowProcess();
    }
    WindowClose();
    return 0;
}
