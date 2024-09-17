// Texture Cache

typedef struct {
    GLuint texture;
    Color color;
    int width, height;
    bool linear;
    bool isBitmap;
} CachedTexture;

static CachedTexture* textureCache = NULL;
static size_t cacheSize = 0;

GLuint CreateTextureFromBitmap(const unsigned char* bitmapData, int width, int height, bool linear) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmapData);
    glTexOpt(linear ? GL_LINEAR : GL_NEAREST, GL_CLAMP_TO_EDGE);
    return textureID;
}

GLuint CreateTextureFromColor(Color color, bool linear) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    unsigned char pixels[] = { color.r, color.g, color.b, color.a };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexOpt(linear ? GL_LINEAR : GL_NEAREST, GL_CLAMP_TO_EDGE);
    return textureID;
}

GLuint GetCachedTexture(Color color, bool linear, bool isBitmap, const unsigned char* bitmapData, int width, int height) {
    for (size_t i = 0; i < cacheSize; ++i) {
        if (textureCache[i].isBitmap == isBitmap &&
            textureCache[i].width == width &&
            textureCache[i].height == height &&
            textureCache[i].linear == linear &&
            (!isBitmap && textureCache[i].color.r == color.r &&
             textureCache[i].color.g == color.g &&
             textureCache[i].color.b == color.b &&
             textureCache[i].color.a == color.a)) {
            return textureCache[i].texture;
        }
    }
    GLuint textureID;
    if (isBitmap) {
        textureID = CreateTextureFromBitmap(bitmapData, width, height, linear);
    } else {
        textureID = CreateTextureFromColor(color, linear);
    }
    textureCache = realloc(textureCache, (cacheSize + 1) * sizeof(CachedTexture));
    textureCache[cacheSize].texture = textureID;
    textureCache[cacheSize].color = color;
    textureCache[cacheSize].width = width;
    textureCache[cacheSize].height = height;
    textureCache[cacheSize].linear = linear;
    textureCache[cacheSize].isBitmap = isBitmap;
    cacheSize++;
    return textureID;
}

void CleanUpTextureCache() {
    for (size_t i = 0; i < cacheSize; ++i) {
        glDeleteTextures(1, &textureCache[i].texture);
    }
    free(textureCache);
    textureCache = NULL;
    cacheSize = 0;
}
