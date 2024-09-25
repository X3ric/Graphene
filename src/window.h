#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
    bool  input;
    bool  wireframe;
    bool  point;
    float pointsize;
    bool  fps;
} Debug;

typedef struct {
    bool        vsync;
    bool        hided;
    bool        floating;
    bool        fullscreen;
    bool        transparent;
    bool        disablecursor;
    bool        hidecursor;
    bool        decorated;
    bool        oldvsync;
    bool        oldhided;
    bool        oldfullscreen;
    bool        oldhidecursor;
    bool        olddisablecursor;
} Options;

typedef struct {
    GLFWwindow*           w;
    Debug                 debug;
    char*                 title; 
    int                   screen_height;
    int                   screen_width;
    int                   height;
    int                   width;
    int                   refresh_rate;
    double                time;
    double                deltatime;
    int                   fpslimit;
    double                fps;
    int                   samples;
    int                   depthbits;
    Options               opt;
} Window;

extern Window window;

// INPUT
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>

    typedef struct {
        double x, y;
        double lastx, lasty;
        bool scrolling;
    } MouseScroll;

    typedef struct {
        double x, y;
        double lastx, lasty;
        MouseScroll scroll;
        bool moving;
    } Mouse;

    extern Mouse mouse;

    // Key handling
        int KeyChar(const char* character);
        int isKeyDown(const char* character);
        int isKeyUp(const char* character);
        bool isKeyPressed(const char* character, double interval);
        int isKey(const char* character);
        void isKeyReset(const char* character);
    // Mouse handling
        Mouse MouseInit();
        void SetCursorPos(float x, float y);
        int isMouseButtonDown(int button);
        int isMouseButtonUp(int button);
        int isMouseButton(const int button);
        void isMouseButtonReset(const int button);
    // Gamepad and Joystick handling

        #define MAX_JOYSTICKS (GLFW_JOYSTICK_LAST + 1)

        typedef struct {
            int joysticks[MAX_JOYSTICKS];
            int count;
        } JoystickManager;

        void LoadJoysticks(void);
        JoystickManager GetJoysticks(void);
        const char* GetJoystickName(int jid);
        bool IsGamepadConnected(int gamepadId);
        int IsGamepadButtonDown(int gamepadId, const char* buttonName);
        int IsGamepadButtonUp(int gamepadId, const char* buttonName);
        int IsGamepadButton(const char* character);
        void ResetGamepadButton(const char* character);
        int GetGamepadAxisValue(const char* axisName);
        float GetGamepadAxis(int gamepadId, const char* axisName);
// UTILS
    #include <ctype.h>
    #include <stdarg.h>
    #include <stdbool.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <math.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <limits.h>

    #ifndef PI
        #define PI 3.14159265358979323846f
    #endif

    typedef struct {
        float x, y;
    } Vec2;

    typedef struct {
        float x, y, z;
    } Vec3;


    // Error handling
    void ErrorCallback(int error, const char* description);
    // Console functions
    void ClearOutput();
    void print(const char* format, ...);
    // Text manipulation
    const char* text(const char* format, ...);
    int textint(char *str);
    float textfloat(char *str);
    unsigned int textlength(const char *text);
    const char *textsubtext(const char *text, int position, int length);
    char *textreplace(const char *text, const char *replace, const char *by);
    char *textinsert(const char *text, const char *insert, int position);
    const char *textjoin(const char **textList, const char *delimiter, int count);
    const char **textsplit(const char *text, char delimiter, int *count);
    void textappend(char *text, const char *append, int *position);
    int textfindindex(const char *text, const char *find);
    const char *textupper(const char *text);
    const char *textlower(const char *text);
    // Random number generation
    void RandomSeed(unsigned int seed);
    int RandomValue(int min, int max);
    // Utility functions
    void OpenURL(const char *url);
    void SetClipboardText(const char *text);
    char *GetClipboardText(void);
    // Basic utilities
    int Clamp(int value, int min, int max);
    // Smoothing functions
    float Easing(float t, const char *text);
    float Motion(float speed, float intensity);
    float Lerp(float start, float end, float t);
    // Time functions
    double GetTime();
    void SetTime(double time);
    bool Wait(double delaySeconds);
    // Collision checking
    bool IsInside(float x, float y, float rectX, float rectY, float rectWidth, float rectHeight);
    // Ratio resize
    int Scaling(int fontsize);
    // File checks
    bool DirExists(const char* path);
    bool FileExists(const char* filename);
    time_t GetFileModTime(const char* filePath);
    int AddWatch(int inotifyFd, const char* filePath);
    // File saving
    char* FileLoad(const char* path);
    char* FileSave(const char* path, const char* text);
    void FileClear(const char* path);
// AUDIO
    #define MINIAUDIO_IMPLEMENTATION
    #include <miniaudio.h>

    typedef struct {
        int       channels;
        int       sample_rate;
        char*     sink_title;
        ma_engine engine;
    } Audio;

    typedef struct {
        ma_sound ma;
    } Sound;

    extern Audio audio;

    // Audio functions
    void AudioInit();
    void AudioVolume(float value);
    float GetAudioVolume();
    void AudioPlay(char *file);
    void AudioStop();

    // Sound functions
    Sound* SoundLoad(char *file);
    void SoundPlay(Sound* sound);
    void SoundStop(Sound* sound);
    void SetSoundStartTime(Sound* sound, ma_uint64 time);
    void SetSoundEndTime(Sound* sound, ma_uint64 time);
    ma_uint64 GetSoundTime(Sound* sound);
    bool GetSoundEnd(Sound* sound);
    bool GetSoundPlaying(Sound* sound);
    float GetSoundPitch(Sound* sound);
    void SetSoundPitch(Sound* sound, float value);
    void SetSoundPitchSemitones(Sound* sound, float semitones);
    float GetSoundPan(Sound* sound);
    void SetSoundPan(Sound* sound, float value);
    bool GetSoundLoop(Sound* sound);
    void SetSoundLoop(Sound* sound, bool value);
    void SoundSetPositioning(ma_sound* pSound, ma_positioning positioning);
    ma_positioning SoundGetPositioning(const ma_sound* pSound);
    void SoundSetPosition(ma_sound* pSound, float x, float y, float z);
    ma_vec3f SoundGetPosition(const ma_sound* pSound);
    void SoundSetDirection(ma_sound* pSound, float x, float y, float z);
    ma_vec3f SoundGetDirection(const ma_sound* pSound);
    void SoundSetVelocity(ma_sound* pSound, float x, float y, float z);
    ma_vec3f SoundGetVelocity(const ma_sound* pSound);
    void SoundEnableSpatialization(ma_sound* pSound);
    void SoundDisableSpatialization(ma_sound* pSound);
    void SoundSetCone(ma_sound* pSound, float innerAngleInRadians, float outerAngleInRadians, float outerGain);
    void SoundGetCone(const ma_sound* pSound, float* pInnerAngleInRadians, float* pOuterAngleInRadians, float* pOuterGain);
    void SoundSetDopplerFactor(ma_sound* pSound, float dopplerFactor);
    float SoundGetDopplerFactor(const ma_sound* pSound);
    void SoundSetStopTimeWithFadeInMilliseconds(ma_sound* pSound, ma_uint64 stopAbsoluteGlobalTimeInMilliseconds, ma_uint64 fadeLengthInMilliseconds);
    ma_result SoundStopWithFadeInMilliseconds(ma_sound* pSound, ma_uint64 fadeLengthInFrames);
    void SoundSetFadeStartInMilliseconds(ma_sound* pSound, float volumeBeg, float volumeEnd, ma_uint64 fadeLengthInMilliseconds, ma_uint64 absoluteGlobalTimeInMilliseconds);
    void SoundSetFadeInMilliseconds(ma_sound* pSound, float volumeBeg, float volumeEnd, ma_uint64 fadeLengthInMilliseconds);
    float SoundGetCurrentFadeVolume(const ma_sound* pSound);
    void SoundSetAttenuationModel(ma_sound* pSound, ma_attenuation_model attenuationModel);
    ma_attenuation_model SoundGetAttenuationModel(const ma_sound* pSound);
    void SoundSetRolloff(ma_sound* pSound, float rolloff);
    float SoundGetRolloff(const ma_sound* pSound);
    void SoundSetMinGain(ma_sound* pSound, float minGain);
    float SoundGetMinGain(const ma_sound* pSound);
    void SoundSetMaxGain(ma_sound* pSound, float maxGain);
    float SoundGetMaxGain(const ma_sound* pSound);
    void SoundSetMinDistance(ma_sound* pSound, float minDistance);
    float SoundGetMinDistance(const ma_sound* pSound);
    void SoundSetMaxDistance(ma_sound* pSound, float maxDistance);
    float SoundGetMaxDistance(const ma_sound* pSound);
    void SoundSetPinnedListenerIndex(ma_sound* pSound, ma_uint32 listenerIndex);
    ma_uint32 SoundGetPinnedListenerIndex(const ma_sound* pSound);
    ma_uint32 SoundGetListenerIndex(const ma_sound* pSound);
    ma_vec3f SoundGetDirectionToListener(const ma_sound* pSound);
// SHADER
    typedef struct {
        GLuint Program;
        const char* vertex;
        const char* fragment;
        time_t lastvertmodtime;
        time_t lastfragmodtime;
        bool hotreloading;
    } Shader;

    extern GLuint VAO;
    extern GLuint VBO;
    extern GLuint EBO;

    #define FLOAT_PER_VERTEX 5

    extern Shader shaderdefault;
    extern Shader shaderfont;

    // SHADER UTILS
        // Shader Utils
            GLuint CompileShader(const char* shaderSource, GLenum type);
            const char* LoadShaderText(const char* filepath);
            GLuint LinkShaders(const char* vertex, const char* fragment);
            Shader LoadShader(const char* vertex, const char* fragment);
            Shader ShaderHotReload(Shader shader);
            void DeleteShader(Shader shader);
        // OpenGL Utils
            void UnbindTexture();
            void glTexOpt(GLint filter, GLint warp);
            GLint GLuint1i(Shader shader, const char* var, float in);
            GLint GLuint1f(Shader shader, const char* var, float in);
            GLint GLuint2f(Shader shader, const char* var, float in1, float in2);
            GLint GLuint3f(Shader shader, const char* var, float in1, float in2, float in3);
            GLint GLuint4f(Shader shader, const char* var, float in1, float in2, float in3, float in4);
            GLint GLumatrix4fv(Shader shader, const char* var, GLfloat* in);
    // SHADER MATH
        void MatrixIdentity(GLfloat* out);
        void MatrixMultiply(const GLfloat* a, const GLfloat* b, GLfloat* result);
        void MatrixLookAt(GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ, GLfloat centerX, GLfloat centerY, GLfloat centerZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat* matrix);
        void MatrixRotate(GLfloat angleX, GLfloat angleY, GLfloat angleZ, GLfloat* matrix);
        void MatrixTranslate(GLfloat tx, GLfloat ty, GLfloat tz, GLfloat *result);
        void MatrixPerspective(GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far, bool is3d, GLfloat* matrix);
        void MatrixOrthographic(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar, GLfloat *matrix);
        void MatrixOrthographicZoom(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar, GLfloat zoomFactor, bool is3d, GLfloat *matrix);
        Vec3 MatrixMultiplyVector(const GLfloat matrix[16], Vec3 vector);
        void TransformVertices(GLfloat *vertices, size_t vertexCount, const GLfloat *rotationMatrix, const Vec3 *positionOffset);
        Vec3 Vec3Add(const Vec3 vec1, const Vec3 vec2);
        void CombineTransformation(GLfloat* modelMatrix, const GLfloat* translationMatrix, const GLfloat* rotationMatrix);
    // SHADER CAMERA
        typedef struct {
            Vec3 position;
            Vec3 localposition;
            Vec3 rotation;
        } Transform;

        typedef struct {
            Transform transform;
            float fov;
            float far;
            float near;
        } Camera;

        typedef struct {
            Camera cam;
            Shader shader;
            GLfloat *vertices;
            GLuint *indices;
            GLfloat size_vertices;
            GLfloat size_indices;
            Transform transform;
            bool is3d;
        } ShaderObject;

        extern Camera camera;

        typedef struct {
            Vec3 vert0;
            Vec3 vert1;
            Vec3 vert2;
            Shader shader;
            Camera cam;
        } TriangleObject;

        typedef struct {
            Vec3 vert0;
            Vec3 vert1;
            Vec3 vert2;
            Vec3 vert3;
            Shader shader;
            Camera cam;
        } RectObject;

        typedef struct {
            Transform transform;
            int size;
            Shader shader;
            Camera cam;
        } CubeObject;

        void CalculateProjections(ShaderObject obj, GLfloat *Model, GLfloat *Projection, GLfloat *View);
        void RenderShader(ShaderObject obj);
        void Triangle(TriangleObject triangle);
        void Zelda(TriangleObject triangle);
        void Rect(RectObject rect);
        void Cube(CubeObject cube);

    void InitializeShader();
    void TerminateShader(void);
// COLOR
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

    Color HexToColor(const char* hex);
    void glColor(Color color);
    void ClearColor(Color color);
// CACHE
    typedef struct {
        GLuint texture;
        Color color;
        int width;
        int height;
        bool linear;
        bool isBitmap;
    } CachedTexture;

    // Texture Cache functions
        GLuint CreateTextureFromBitmap(const unsigned char* bitmapData, int width, int height, bool linear);
        GLuint CreateTextureFromColor(Color color, bool linear);
        GLuint GetCachedTexture(Color color, bool linear, bool isBitmap, const unsigned char* bitmapData, int width, int height);
        void CleanUpTextureCache(void);
// DRAW
    void DrawRect(int x, int y, int width, int height, Color color);
    void DrawRectBorder(int x, int y, int width, int height, int thickness, Color color);
    void DrawLine(float x0, float y0, float x1, float y1, int thickness, Color color);
    void DrawCircle(int x, int y, int r, Color color);
    void DrawCircleBorder(int x, int y, int r, int thickness, Color color);
    void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color);
    void DrawTriangleBorder(int x1, int y1, int x2, int y2, int x3, int y3, int thickness, Color color);
    void DrawCube(GLfloat size, GLfloat x, GLfloat y, GLfloat z, GLfloat rotx, GLfloat roty, GLfloat rotz, Color color);
// IMAGE
    #define STB_IMAGE_IMPLEMENTATION
    #include <stb_image.h>
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include <stb_image_write.h>
    
    typedef struct {
        GLuint raw;         
        unsigned char* data;
        int width, height;  
        int channels;
    } Img;

    typedef struct {
        const char *filename;
        bool nearest;
    } ImgInfo;

    Img LoadImage(ImgInfo info);
    void BindImg(Img image);
    void DrawImage(Img image, float x, float y, float width, float height, GLfloat angle);
    void DrawImageShader(Img image, float x, float y, float width, float height, GLfloat angle, Shader shader);
    void SaveScreenshot(const char *filename, int x, int y, int width, int height);
// FONT
    
    #include <ft2build.h>
    #include FT_FREETYPE_H
    #include FT_GLYPH_H
    #include FT_OUTLINE_H
    #include FT_BITMAP_H

    #define STB_TRUETYPE_IMPLEMENTATION
    #include <stb_truetype.h>

    typedef struct {
        float x0, y0, x1, y1;  // Coordinates of the glyph in the atlas (in pixels)
        float xoff, yoff;      // Left/top offsets
        float xadvance;        // Advance width
        float u0, v0;          // Texture coordinates for the top-left corner of the glyph
        float u1, v1;          // Texture coordinates for the bottom-right corner of the glyph
    } Glyph;

    #define MAX_GLYPHS 256

    typedef struct {
        FT_Library library;       // FreeType library instance
        FT_Face face;             // Font face
        unsigned char* atlasData; // Atlas texture data
        GLuint textureID;         // OpenGL texture ID
        int atlasWidth;           // Dimensions of the atlas Width
        int atlasHeight;          // Dimensions of the atlas Height
        Glyph glyphs[MAX_GLYPHS]; // Glyph data for ASCII characters 32-127
        float fontSize;           // Font size for which glyphs were generated
        bool nearest;             // Nearest filter
    } Font;

    typedef struct {
        int width;
        int height;
    } TextSize;

    typedef struct FontCacheNode {
        float fontSize;
        Font font;
        struct FontCacheNode* next;
    } FontCacheNode;

    int CalculateAtlasSize(int numGlyphs, float fontSize, int oversampling);
    Font GenAtlas(Font font);
    Font LoadFont(const char* fontPath);
    Font SetFontSize(Font font, float fontSize);
    TextSize GetTextSize(Font font, float fontSize, const char* text);
    void RenderShaderText(ShaderObject obj, Color color, float fontSize);
    void DrawText(int x, int y, Font font, float fontSize, const char* text, Color color);
    void DrawTextEditor(int x, int y, Font font, float fontSize, const char* text, Color color, int cursorStart, int cursorEnd, Shader shaderfont, Shader shaderfontcursor);
    void FreeFontCache();
// END

int WindowInit(int width, int height, char* title);
void WindowFrames();
void WindowClear();
void WindowProcess();
void window_buffersize_callback(GLFWwindow* glfw_window, int width, int height);
int WindowState();
void WindowStateSet(bool state);
void WindowClose();