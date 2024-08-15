#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

Img LoadImage(ImgInfo info) {
    Img img;
    stbi_set_flip_vertically_on_load(true);
    img.data = stbi_load(info.filename, &img.width, &img.height, &img.channels, STBI_rgb_alpha);
    if (img.data == NULL) {
        img.raw = 0;
        return img;
    }
    glGenTextures(1, &img.raw);
    glBindTexture(GL_TEXTURE_2D, img.raw);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
    glTexOpt(info.nearest ? GL_NEAREST : GL_LINEAR, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(img.data);
    img.data = NULL;
    return img;
}

void BindImg(Img image){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, image.raw);
}

void DrawImage(Img image, float x, float y, float width, float height, GLfloat angle) {
    BindImg(image);
    Rect((RectObject){
        { x, y + height, 0.0f },         // Bottom Left
        { x + width, y + height, 0.0f }, // Bottom Right
        { x, y, 0.0f },                  // Top Left
        { x + width, y, 0.0f },          // Top Right
        shaderdefault,                   // Shader
        camera,                          // Camera
    });
    UnbindTexture();
}

void DrawImageShader(Img image, float x, float y, float width, float height, GLfloat angle, Shader shader) {
    BindImg(image);
    Rect((RectObject){
        { x, y + height, 0.0f },         // Bottom Left
        { x + width, y + height, 0.0f }, // Bottom Right
        { x, y, 0.0f },                  // Top Left
        { x + width, y, 0.0f },          // Top Right
        shaderdefault,                   // Shader
        camera,                          // Camera
    });
    UnbindTexture();
}

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

void SaveScreenshot(const char *filename, int x, int y, int width, int height) {
    printf("Saving screenshot to -> %s\n", filename);
    unsigned char *pixels = malloc(width * height * 4); // RGBA
    if (!pixels) return;
    int adjustedY = window.screen_height - y - height;
    glReadPixels(x, adjustedY, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    unsigned char *flippedPixels = malloc(width * height * 4);
    if (!flippedPixels) {
        free(pixels);
        return;
    }
    for (int row = 0; row < height; row++) {
        memcpy(flippedPixels + row * width * 4, pixels + (height - 1 - row) * width * 4, width * 4);
    }
    stbi_write_jpg(filename, width, height, 4, flippedPixels, width * 4);
    free(pixels);
    free(flippedPixels);
}
