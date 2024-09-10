typedef struct {
    GLuint texture;
    Color color;
    int width, height;
    bool linear;
    bool isBitmap;
} CachedTexture;

typedef struct {
    GLuint texture;
    int width;
    int height;
    char* text;
} TextCacheEntry;

typedef struct {
    TextCacheEntry* entries;
    size_t size;
    size_t capacity;
} TextCache;

// Texture Cache

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


// Text Cache

static TextCache* textCache = NULL;

void InitTextCache(void) {
    if (textCache) return;
    textCache = (TextCache*)malloc(sizeof(TextCache));
    textCache->capacity = 16;
    textCache->size = 0;
    textCache->entries = (TextCacheEntry*)calloc(textCache->capacity, sizeof(TextCacheEntry));
}

void AddToTextCache(GLuint texture, int width, int height, const char* text) {
    if (!textCache) InitTextCache();
    for (size_t i = 0; i < textCache->size; ++i) {
        if (textCache->entries[i].text && strcmp(textCache->entries[i].text, text) == 0) {
            textCache->entries[i].texture = texture;
            textCache->entries[i].width = width;
            textCache->entries[i].height = height;
            return;
        }
    }
    if (textCache->size >= textCache->capacity) {
        size_t new_capacity = textCache->capacity * 2;
        TextCacheEntry* new_entries = (TextCacheEntry*)realloc(textCache->entries, new_capacity * sizeof(TextCacheEntry));
        if (new_entries) {
            textCache->entries = new_entries;
            textCache->capacity = new_capacity;
            for (size_t i = textCache->size; i < new_capacity; ++i) {
                textCache->entries[i].text = NULL;
            }
        }
    }
    size_t index = textCache->size;
    textCache->entries[index].texture = texture;
    textCache->entries[index].width = width;
    textCache->entries[index].height = height;
    textCache->entries[index].text = strdup(text);
    textCache->size++;
}

GLuint GetTextFromCache(const char* text, int* width, int* height) {
    if (!textCache) return 0;
    for (size_t i = 0; i < textCache->size; ++i) {
        if (textCache->entries[i].text && strcmp(textCache->entries[i].text, text) == 0) {
            if (width) *width = textCache->entries[i].width;
            if (height) *height = textCache->entries[i].height;
            return textCache->entries[i].texture;
        }
    }
    return 0;
}

void CleanupTextCache(void) {
    if (!textCache) return;
    for (size_t i = 0; i < textCache->size; ++i) {
        free(textCache->entries[i].text);
    }
    free(textCache->entries);
    free(textCache);
    textCache = NULL;
}
