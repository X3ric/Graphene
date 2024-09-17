#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>
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
    stbtt_fontinfo fontInfo;   // Font information
    unsigned char* fontBuffer; // Font data buffer
    unsigned char* atlasData;  // Atlas texture data
    GLuint textureID;          // OpenGL texture ID
    int atlasWidth;            // Dimensions of the atlas Width
    int atlasHeight;           // Dimensions of the atlas Height
    Glyph glyphs[MAX_GLYPHS];  // Glyph data for ASCII characters 32-127
    float fontSize;            // Font size for which glyphs were generated
    bool nearest;              // Nearest filter
} Font;

Font GenAtlas(Font font) {
    if (font.fontSize <= 1) font.fontSize = 32.0f;
    if (!font.fontBuffer) return font;
    font.atlasWidth = 1024;
    font.atlasHeight = 1024;
    font.atlasData = (unsigned char*)calloc(font.atlasWidth * font.atlasHeight * 4, sizeof(unsigned char)); // RGBA
    if (!font.atlasData) {
        free(font.fontBuffer);
        return font;
    }
    stbtt_pack_context packContext;
    if (!stbtt_PackBegin(&packContext, font.atlasData, font.atlasWidth, font.atlasHeight, 0, 1, NULL)) {
        free(font.atlasData);
        free(font.fontBuffer);
        return font;
    }
    stbtt_PackSetOversampling(&packContext, 4, 4);
    stbtt_packedchar packedChars[MAX_GLYPHS];
    if (!stbtt_PackFontRange(&packContext, font.fontBuffer, 0, font.fontSize, 32, MAX_GLYPHS, packedChars)) {
        stbtt_PackEnd(&packContext);
        free(font.atlasData);
        free(font.fontBuffer);
        return font;
    }
    for (int i = font.atlasWidth * font.atlasHeight - 1; i >= 0; --i) {
        unsigned char alpha = font.atlasData[i]; // Get the alpha value from the single-channel data
        font.atlasData[i * 4 + 0] = 255;         // Red channel (set to white)
        font.atlasData[i * 4 + 1] = 255;         // Green channel (set to white)
        font.atlasData[i * 4 + 2] = 255;         // Blue channel (set to white)
        font.atlasData[i * 4 + 3] = alpha;       // Alpha channel
    }
    for (int i = 0; i < MAX_GLYPHS; ++i) {
        font.glyphs[i].x0 = packedChars[i].x0;
        font.glyphs[i].y0 = packedChars[i].y0;
        font.glyphs[i].x1 = packedChars[i].x1;
        font.glyphs[i].y1 = packedChars[i].y1;
        font.glyphs[i].xoff = packedChars[i].xoff;
        font.glyphs[i].yoff = packedChars[i].yoff;
        font.glyphs[i].xadvance = packedChars[i].xadvance;
        font.glyphs[i].u0 = packedChars[i].x0 / (float)font.atlasWidth;
        font.glyphs[i].v0 = packedChars[i].y0 / (float)font.atlasHeight;
        font.glyphs[i].u1 = packedChars[i].x1 / (float)font.atlasWidth;
        font.glyphs[i].v1 = packedChars[i].y1 / (float)font.atlasHeight;
    }
    stbtt_PackEnd(&packContext);
    glGenTextures(1, &font.textureID);
    glBindTexture(GL_TEXTURE_2D, font.textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font.atlasWidth, font.atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, font.atlasData);
    glTexOpt(font.nearest ? GL_NEAREST : GL_LINEAR, GL_CLAMP_TO_EDGE);
    stbi_write_jpg("/tmp/atlas.jpg", font.atlasWidth, font.atlasHeight, 1, font.atlasData, font.atlasWidth);
    free(font.atlasData);
    return font;
}

Font LoadFont(const char* fontPath) {
    Font font = {0};
    long size;
    FILE* fp = fopen(fontPath, "rb");
    if (!fp) return font;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    font.fontBuffer = (unsigned char*)malloc(size);
    if (font.fontBuffer) {
        fread(font.fontBuffer, size, 1, fp);
        if (!stbtt_InitFont(&font.fontInfo, font.fontBuffer, stbtt_GetFontOffsetForIndex(font.fontBuffer, 0))) {
            free(font.fontBuffer);
            fclose(fp);
            return font;
        }
    }
    fclose(fp);
    font = GenAtlas(font);
    return font;
}

typedef struct {
    int width;
    int height;
} TextSize;

TextSize GetTextSize(Font font, float fontSize, const char* text) {
    if (!font.fontBuffer) return (TextSize){0, 0};
    TextSize size = {0, 0};
    float scale = stbtt_ScaleForPixelHeight(&font.fontInfo, fontSize);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font.fontInfo, &ascent, &descent, &lineGap);
    int lineHeight = (int)((ascent - descent + lineGap) * scale);
    int currentLineWidth = 0;
    int maxLineWidth = 0;
    for (size_t i = 0; text[i] != '\0'; ++i) {
        if (text[i] == '\n') {
            if (currentLineWidth > maxLineWidth) {
                maxLineWidth = currentLineWidth;
            }
            currentLineWidth = 0;
            size.height += lineHeight;
            continue;
        }
        int advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(&font.fontInfo, text[i], &advanceWidth, &leftSideBearing);
        currentLineWidth += (int)(advanceWidth * scale);
    }
    if (currentLineWidth > maxLineWidth) {
        maxLineWidth = currentLineWidth;
    }
    size.width = maxLineWidth;
    size.height += lineHeight;
    return size;
}


void RenderShaderText(ShaderObject obj, Color color, float fontSize) {
    if (obj.shader.hotreloading) {
        obj.shader = ShaderHotReload(obj.shader);
    }
    // Projection Matrix
        GLfloat Projection[16], Model[16], View[16];
        CalculateProjections(obj,Model,Projection,View);
    // Depth
        if(obj.is3d) {
            if(obj.cam.fov > 0.0f){
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LEQUAL);
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                glFrontFace(GL_CCW);
            } else {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
                glFrontFace(GL_CCW);
            }
        }
    // Debug
        if (window.debug.wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else if (window.debug.point) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            if(window.debug.pointsize > 0) glPointSize(window.debug.pointsize);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    // Bind VAO
        glBindVertexArray(VAO);
    // Bind VBO and update with new vertex data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, obj.size_vertices, obj.vertices);
    // Bind EBO and update with new index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, obj.size_indices, obj.indices);
    // Use the shader program
        glUseProgram(obj.shader.Program);
    // Set uniforms
        GLumatrix4fv(obj.shader, "projection", Projection);
        GLumatrix4fv(obj.shader, "model", Model);
        GLumatrix4fv(obj.shader, "view", View);
        GLuint1f(obj.shader, "Size", fontSize);
        GLuint4f(obj.shader, "Color", color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
        GLuint1f(obj.shader, "iTime", glfwGetTime());
        GLuint2f(obj.shader, "iResolution", window.screen_width, window.screen_height);
        GLuint2f(obj.shader, "iMouse", mouse.x, mouse.y);
    // Draw using indices
        glDrawElements(GL_TRIANGLES, obj.size_indices / sizeof(GLuint), GL_UNSIGNED_INT, 0);
    // Unbind shader program
        glUseProgram(0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Disable Effects
        if(obj.is3d) {
            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
        }
}

void DrawText(int x, int y, Font font, float fontSize, const char* text, Color color) {
    if (fontSize <= 1.0f) fontSize = 1.0f;
    if (color.a == 0) color.a = 255;
    if (!font.fontBuffer || !font.textureID) return;
    glBindTexture(GL_TEXTURE_2D, font.textureID);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float scale = stbtt_ScaleForPixelHeight(&font.fontInfo, fontSize);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font.fontInfo, &ascent, &descent, &lineGap);
    float ascent_px = ascent * scale;
    float lineHeight = (ascent - descent + lineGap) * scale;
    float xpos = (float)x;
    float ypos = (float)y + ascent_px;
    for (size_t i = 0; text[i] != '\0'; ++i) {
        if (text[i] == '\n') {
            ypos += lineHeight;
            xpos = (float)x;
            continue;
        }
        int codepoint = (unsigned char)text[i];
        if (codepoint < 32 || codepoint >= 32 + MAX_GLYPHS) continue;
        Glyph* g = &font.glyphs[codepoint - 32];
        if (!g) continue;
        int glyphIndex = stbtt_FindGlyphIndex(&font.fontInfo, codepoint);
        if (glyphIndex == 0) continue;
        int advanceWidth, leftSideBearing;
        stbtt_GetGlyphHMetrics(&font.fontInfo, glyphIndex, &advanceWidth, &leftSideBearing);
        int x0, y0, x1, y1;
        stbtt_GetGlyphBitmapBox(&font.fontInfo, glyphIndex, 1.0f, 1.0f, &x0, &y0, &x1, &y1);
        float x0_scaled = x0 * scale;
        float y0_scaled = y0 * scale;
        float x1_scaled = x1 * scale;
        float y1_scaled = y1 * scale;
        float x_start = xpos + leftSideBearing * scale;
        float y_start = ypos;
        float x_pos = x_start + x0_scaled;
        float y_pos = y_start + y0_scaled;
        float w = (x1_scaled - x0_scaled);
        float h = (y1_scaled - y0_scaled);
        float u0 = g->u0;
        float v0 = g->v0;
        float u1 = g->u1;
        float v1 = g->v1;
        GLfloat vertices[] = {
            x_pos,     y_pos + h, 0.0f, u0, v1,  // Top-left
            x_pos + w, y_pos + h, 0.0f, u1, v1,  // Top-right
            x_pos + w, y_pos,     0.0f, u1, v0,  // Bottom-right
            x_pos,     y_pos,     0.0f, u0, v0   // Bottom-left
        };
        GLuint indices[] = {0, 1, 2, 2, 3, 0};
        RenderShaderText((ShaderObject){camera, shaderfont, vertices, indices, sizeof(vertices), sizeof(indices)}, color, fontSize);
        xpos += (advanceWidth * scale);
        if (text[i + 1]) {
            unsigned char nextCodepoint = (unsigned char)text[i + 1];
            if (nextCodepoint >= 32 && nextCodepoint < 32 + MAX_GLYPHS) {
                int nextGlyphIndex = stbtt_FindGlyphIndex(&font.fontInfo, nextCodepoint);
                if (nextGlyphIndex != 0) {
                    int kernAdvance = stbtt_GetGlyphKernAdvance(&font.fontInfo, glyphIndex, nextGlyphIndex);
                    xpos += kernAdvance * scale;
                }
            }
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}
