#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
    bool input;
    bool wireframe;
    bool fps;
} Debug;

typedef struct {
    bool        vsync;
    bool        oldvsync;
    bool        fullscreen;
    bool        oldfullscreen;
    bool        floating;
    bool        transparent;
    bool        decorated;
    bool        disablecursor;
    bool        olddisablecursor;
    bool        hidecursor;
    bool        oldhidecursor;
    bool        hided;
    bool        oldhided;
} Options;

typedef struct {
    GLFWwindow* w;
    Debug       debug;
    char*       title; 
    int         screen_height;
    int         screen_width;
    int         height;
    int         width;
    int         refresh_rate;
    double      deltatime;
    int         fpslimit;
    double      fps;
    int         samples;
    int         depthbits;
    Options     opt;
} Window;

Window window;

#include "input.c"
#include "utils.c"
#include "audio.c"

#include "render/init.c"

void WindowFrames(){
    static double previousframetime = 0.0;
    double targetTime = 1.0 / window.fpslimit;
    window.deltatime = glfwGetTime();
    double elapsedTime = window.deltatime - previousframetime;
    if (window.fpslimit != 0) {
        while (elapsedTime < targetTime) {
            window.deltatime = glfwGetTime();
            elapsedTime = window.deltatime - previousframetime;
        }
    }
    if (elapsedTime != 0) {
        window.fps = 1.0 / elapsedTime;
    } else {
        window.fps = 0.0;
    }
    previousframetime = window.deltatime;
    if(window.debug.fps){
        print("FPS: %.0f\n",window.fps);
    }
}

void WindowClear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    WindowFrames();
}

void WindowProcess() {
    mouse = MouseInit();
    if (window.opt.fullscreen != window.opt.oldfullscreen) {
        if (window.opt.fullscreen) {
            glfwSetWindowMonitor(window.w, glfwGetPrimaryMonitor(), 0, 0, window.screen_width, window.screen_width, 0);
        } else {
            glfwSetWindowMonitor(window.w, NULL, 0, 0, window.screen_width, window.screen_height, 0);
        }
        window.opt.oldfullscreen = window.opt.fullscreen;
    }
    if (window.opt.disablecursor != window.opt.olddisablecursor) {
        if (window.opt.disablecursor) {
            glfwSetInputMode(window.w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            if (window.opt.hidecursor != window.opt.oldhidecursor) {
                if (window.opt.hidecursor) {
                    glfwSetInputMode(window.w, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                } else {
                    glfwSetInputMode(window.w, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                window.opt.oldhidecursor = window.opt.hidecursor;
            }
        }
        window.opt.oldhidecursor = window.opt.hidecursor;
    }
    if (window.opt.vsync != window.opt.oldvsync) {
        if (window.opt.vsync) {
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(0);
        }
        window.opt.oldvsync = window.opt.vsync;
    } 
    if (window.opt.hided != window.opt.oldhided) {
        if (!window.opt.hided) {
            glfwShowWindow(window.w);
        } else {
            glfwHideWindow(window.w);
        }
        window.opt.oldhided = window.opt.hided;
    }
    glfwSwapBuffers(window.w);
    glfwPollEvents();
}

void window_buffersize_callback(GLFWwindow* glfw_window, int width, int height)
{
    const int MIN_PIXEL = 1;
    window.screen_width = width < MIN_PIXEL ? MIN_PIXEL : width;
    window.screen_height = height < MIN_PIXEL ? MIN_PIXEL : height;
    glViewport(0, 0, (GLsizei)window.screen_width, (GLsizei)window.screen_height);
}

void update(void);

void window_refresh_callback(GLFWwindow* window)
{
    WindowClear();
    update();
    WindowProcess();   
}

int WindowInit(int width, int height, char* title)
{
    window.title = title;
    window.width = width;
    window.height = height;
    window.screen_width = width;
    window.screen_height = height;
    glfwSetErrorCallback(ErrorCallback);
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, window.samples);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    if (window.opt.transparent) {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE); 
    }
    if (!window.opt.decorated) {
        glfwWindowHint(GLFW_DECORATED, GL_FALSE); 
    }
    if (window.opt.floating) {
        glfwWindowHint(GLFW_RESIZABLE , GLFW_FALSE);
    } else {
        glfwWindowHint(GLFW_RESIZABLE , GLFW_TRUE);
    }
    glfwWindowHint(GLFW_REFRESH_RATE, window.refresh_rate);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//forced new opengl version
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window.w = glfwCreateWindow(window.screen_width, window.screen_height, window.title, NULL, NULL);
    if (!window.w) {
        printf("Failed to open GLFW window.w\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window.w);
    glfwSetCharCallback(window.w, CharCallback);
    glfwSetKeyCallback(window.w, KeyCallback);
    glfwSetScrollCallback(window.w, ScrollCallback);
    glfwSetFramebufferSizeCallback(window.w, window_buffersize_callback);
    glfwSetWindowRefreshCallback(window.w, window_refresh_callback);
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
    }
    if (window.opt.vsync) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }
    glfwSetInputMode(window.w, GLFW_STICKY_KEYS, GLFW_TRUE);
    glEnable(GL_MULTISAMPLE);
    if(window.depthbits != 0) {
        glfwWindowHint(GLFW_DEPTH_BITS, window.depthbits);
    } else {
        glfwWindowHint(GLFW_DEPTH_BITS, 24);
    }
    InitializeShader();
    print("Loaded\n");
    return 0;
}

int WindowState() {
    return glfwWindowShouldClose(window.w);
}

void WindowStateSet(bool state) {
    if (state) {
        glfwSetWindowShouldClose(window.w, GLFW_TRUE);
    } else  {
        glfwSetWindowShouldClose(window.w, GLFW_FALSE);            
    }
}

void WindowClose()
{
    print("Exit\n");
    AudioStop();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderdefault.Program);
    glfwDestroyWindow(window.w);
    glfwTerminate();
}
