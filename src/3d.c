#include "grafenic/init.c"
#include "grafenic/ui.c"

Font font;
Image img;
Vec3 cube;
double lastscrolly = 0.0;
double targetZ = 0.0;

Camera cam = {
    {
    0.0f, 0.0f, 0.0f,   // Position: x, y, z
    0.0f, 0.0f, 0.0f,   // LocalPosition: x, y, z
    0.0f, 0.0f, 0.0f    // Rotation: x, y, z
    },
    0.0f,               // Fov 0 means Orthographic
    0.0f,               // Far Distance when 0.0 is def to 1000.0f
    -1000.0f            // Near Distance 
};

void update(void) {
    // Movement Camera
        double speed;
        double clampz;
        clampz = (window.deltatime);
        //print(text("Cam lerp: %.5f\n", clampz));
        if (isKeyDown("LeftShift")) {
            speed = 0.0001f;
        } else {
            speed = 0.00005f;
        }
        if (isKeyDown("w")) {
            cube.y += speed * clampz;
        } else if (isKeyDown("s")) {
            cube.y -= speed * clampz;
        }
        if (isKeyDown("a")) {
            cube.x += speed * clampz;
        } else if (isKeyDown("d")) {
            cube.x -= speed * clampz;
        }
        if (isKeyDown("e")) {
            targetZ = targetZ + speed * clampz;
        } else if (isKeyDown("q")) {
            targetZ = targetZ - speed * clampz;
        }
        if (isKeyDown("r")) {
            speed = 0.0f;
            targetZ = 0.0f;
            mousescroll.y = 0;
            lastscrolly = mousescroll.y;
            cube.x = Lerp(cube.x, 0.0f, 0.0001f * clampz);
            cube.y = Lerp(cube.y, 0.0f, 0.0001f * clampz);
            cube.z = Lerp(cube.z, 0.0f, 0.0001f * clampz);
        } else {
            if (lastscrolly != mousescroll.y) {
                targetZ = cube.z + (mousescroll.y - lastscrolly);
                mousescroll.y = 0;
            }
            cube.z = Lerp(cube.z, targetZ, 0.0001f * clampz);
            lastscrolly = mousescroll.y;
        }
    // Rotation Vec3
        Vec3 rot;
        rot.x = window.deltatime * 0.2f;
        rot.y = window.deltatime * 0.3f;
        rot.z = window.deltatime * 0.4f;
    // 3d envoiriment
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
        //window.debug.wireframe = false; //stop debugging
    // Modular ui.c functions
        Fps(0, 0, font, Scaling(50));
        ExitPromt(font);  
}

int main(int argc, char** argv) {
    WindowInit(1920, 1080, "Grafenic");
    font = LoadFont("./res/fonts/Monocraft.ttf");font.nearest = true;
    img = LoadImage("./res/images/Test.png");
    shaderdefault.hotreloading = true;
    ClearColor((Color){75, 75, 75, 100});
    while (!WindowState()) {
        WindowClear();
        update();
        WindowProcess();
    }
    WindowClose();
    return 0;
}
