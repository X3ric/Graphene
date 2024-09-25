#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

typedef struct {
    float x0, y0, x1, y1;  // Coordinates of the glyph in the atlas (in pixels)
    float xoff, yoff;      // Left/top offsets
    float xadvance;        // Advance width
    float u0, v0;          // Texture coordinates for the top-left corner of the glyph
    float u1, v1;          // Texture coordinates for the bottom-right corner of the glyph
} Glyph;

#define MAX_GLYPHS 256

typedef struct {
    FT_Library library;        // FreeType library instance
    FT_Face face;              // Font face
    unsigned char* atlasData;  // Atlas texture data
    GLuint textureID;          // OpenGL texture ID
    int oversampling;          // Dimensions of the oversampling
    int atlasWidth;            // Dimensions of the atlas Width
    int atlasHeight;           // Dimensions of the atlas Height
    Glyph glyphs[MAX_GLYPHS];  // Glyph data for ASCII characters 32-127
    float fontSize;            // Font size for which glyphs were generated
    bool nearest;              // Nearest filter
    bool subpixel;              // Subpixel
} Font;

#define ATLAS_FONT_SIZE 128.0

typedef struct FontCacheNode {
    float fontSize;
    Font font;
    struct FontCacheNode* next;
} FontCacheNode;

FontCacheNode* fontCache = NULL;

int CalculateAtlasSize(int numGlyphs, float fontSize, int oversampling) {
    float estimatedAreaPerGlyph = (fontSize * fontSize) * oversampling * oversampling;
    float totalArea = estimatedAreaPerGlyph * numGlyphs;
    int atlasDimension = (int)sqrt(totalArea);
    int powerOfTwo = 1;
    while (powerOfTwo < atlasDimension) {
        powerOfTwo *= 2;
    }
    return powerOfTwo;
}

Font GenAtlas(Font font) {
    if (font.fontSize <= 1) font.fontSize = ATLAS_FONT_SIZE;
    if (font.oversampling <= 1) font.oversampling = 4;
    if (!font.face) return font;
    FT_Error error = FT_Set_Pixel_Sizes(font.face, 0, font.fontSize);
    if (error) {
        return font;
    }
    if (font.subpixel) FT_Library_SetLcdFilter(font.library, FT_LCD_FILTER_DEFAULT);
    int estimatedAtlasSize = CalculateAtlasSize(MAX_GLYPHS, font.fontSize, font.oversampling);
    font.atlasWidth = estimatedAtlasSize;
    font.atlasHeight = estimatedAtlasSize;
    font.atlasData = (unsigned char*)calloc(font.atlasWidth * font.atlasHeight * 4, sizeof(unsigned char));
    if (!font.atlasData) {
        FT_Done_Face(font.face);
        FT_Done_FreeType(font.library);
        return font;
    }
    stbrp_context packContext;
    stbrp_node* nodes = (stbrp_node*)malloc(sizeof(stbrp_node) * font.atlasWidth);
    stbrp_init_target(&packContext, font.atlasWidth, font.atlasHeight, nodes, font.atlasWidth);
    stbrp_rect* rects = (stbrp_rect*)malloc(sizeof(stbrp_rect) * MAX_GLYPHS);
    for (int i = 0; i < MAX_GLYPHS; ++i) {
        int codepoint = 32 + i;
        if (font.subpixel) {
            error = FT_Load_Char(font.face, codepoint, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LCD);
        } else {
            error = FT_Load_Char(font.face, codepoint, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT);
        }
        if (error) {
            rects[i].w = rects[i].h = 0;
            rects[i].id = i;
            continue;
        }
        FT_GlyphSlot slot = font.face->glyph;
        FT_Bitmap* bitmap = &slot->bitmap;
        rects[i].id = i;
        rects[i].w = font.subpixel ? (bitmap->width / 3) + 2 : bitmap->width + 2;
        rects[i].h = bitmap->rows + 2;
    }
    stbrp_pack_rects(&packContext, rects, MAX_GLYPHS);
    for (int i = 0; i < MAX_GLYPHS; ++i) {
        if (!rects[i].was_packed) continue;
        int codepoint = 32 + i;
        if (font.subpixel) {
            error = FT_Load_Char(font.face, codepoint, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LCD);
        } else {
            error = FT_Load_Char(font.face, codepoint, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT);
        }
        if (error) continue;
        FT_GlyphSlot slot = font.face->glyph;
        FT_Bitmap* bitmap = &slot->bitmap;
        int x = rects[i].x;
        int y = rects[i].y;
       if (font.subpixel) {
            for (int row = 0; row < bitmap->rows; ++row) {
                for (int col = 0; col < bitmap->width; col += 3) {
                    int srcIndex = row * bitmap->pitch + col;
                    int dstIndex = ((y + row) * font.atlasWidth + (x + col / 3)) * 4;
                    font.atlasData[dstIndex + 0] = bitmap->buffer[srcIndex + 0];
                    font.atlasData[dstIndex + 1] = bitmap->buffer[srcIndex + 1];
                    font.atlasData[dstIndex + 2] = bitmap->buffer[srcIndex + 2];
                    font.atlasData[dstIndex + 3] = bitmap->buffer[srcIndex + 3];
                }
            }
        } else {
            for (int row = 0; row < bitmap->rows; ++row) {
                for (int col = 0; col < bitmap->width; ++col) {
                    int srcIndex = row * bitmap->pitch + col;
                    int dstIndex = ((y + row) * font.atlasWidth + (x + col)) * 4;
                    font.atlasData[dstIndex + 0] = 255;
                    font.atlasData[dstIndex + 1] = 255;
                    font.atlasData[dstIndex + 2] = 255;
                    font.atlasData[dstIndex + 3] = bitmap->buffer[srcIndex];    
                }
            }
        }
        Glyph* glyph = &font.glyphs[i];
        glyph->x0 = x;
        glyph->y0 = y;
        glyph->x1 = x + (font.subpixel ? bitmap->width / 3 : bitmap->width);
        glyph->y1 = y + bitmap->rows;
        glyph->xoff = slot->bitmap_left;
        glyph->yoff = slot->bitmap_top;
        glyph->xadvance = slot->advance.x >> 6;
        glyph->u0 = (float)glyph->x0 / (float)font.atlasWidth;
        glyph->v0 = (float)glyph->y0 / (float)font.atlasHeight;
        glyph->u1 = (float)glyph->x1 / (float)font.atlasWidth;
        glyph->v1 = (float)glyph->y1 / (float)font.atlasHeight;
    }
    glGenTextures(1, &font.textureID);
    glBindTexture(GL_TEXTURE_2D, font.textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font.atlasWidth, font.atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, font.atlasData);
    glTexOpt(font.nearest ? GL_NEAREST : GL_LINEAR, GL_CLAMP_TO_EDGE);
    //stbi_write_jpg("/tmp/atlas.jpg", font.atlasWidth, font.atlasHeight, 1, font.atlasData, font.atlasWidth);
    free(font.atlasData);
    font.atlasData = NULL;
    free(nodes);
    free(rects);
    return font;
}

Font LoadFont(const char* fontPath) {
    Font font = {0};
    FT_Error error = FT_Init_FreeType(&font.library);
    if (error) {
        return font;
    }
    error = FT_New_Face(font.library, fontPath, 0, &font.face);
    if (error == FT_Err_Unknown_File_Format) {
        FT_Done_FreeType(font.library);
        return font;
    } else if (error) {
        FT_Done_FreeType(font.library);
        return font;
    }
    if (font.fontSize <= 1) font.fontSize = ATLAS_FONT_SIZE;
    if (font.oversampling <= 1) font.oversampling = 1;
    font = GenAtlas(font);
    return font;
}

Font SetFontSize(Font font, float fontSize) {
    if (fontSize <= 1) fontSize = ATLAS_FONT_SIZE;
    FontCacheNode* node = fontCache;
    while (node) {
        if (node->fontSize == fontSize) {
            return node->font;
        }
        node = node->next;
    }
    font.fontSize = fontSize;
    font = GenAtlas(font);
    FontCacheNode* newNode = (FontCacheNode*)malloc(sizeof(FontCacheNode));
    newNode->fontSize = fontSize;
    newNode->font = font;
    newNode->next = fontCache;
    fontCache = newNode;
    return font;
}

typedef struct {
    int width;
    int height;
} TextSize;

TextSize GetTextSize(Font font, float fontSize, const char* text) {
    if (!font.face) return (TextSize){0, 0};
    TextSize size = {0, 0};
    FT_Error error = FT_Set_Pixel_Sizes(font.face, 0, fontSize);
    if (error) {
        return size;
    }
    FT_Face face = font.face;
    FT_GlyphSlot slot = face->glyph;
    int ascent = face->size->metrics.ascender >> 6;
    int descent = face->size->metrics.descender >> 6;
    int lineGap = (face->size->metrics.height - (face->size->metrics.ascender - face->size->metrics.descender)) >> 6;
    int lineHeight = ascent - descent + lineGap;
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
        int codepoint = (unsigned char)text[i];
        error = FT_Load_Char(face, codepoint, FT_LOAD_DEFAULT);
        if (error) {
            continue;
        }
        currentLineWidth += slot->advance.x >> 6;
        if (text[i + 1]) {
            int nextCodepoint = (unsigned char)text[i + 1];
            FT_Vector delta;
            FT_Get_Kerning(face, codepoint, nextCodepoint, FT_KERNING_DEFAULT, &delta);
            currentLineWidth += delta.x >> 6;
        }
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
    if (!font.face || !font.textureID) return;
    float scale = fontSize / font.fontSize;
    font = SetFontSize(font, font.fontSize);
    glBindTexture(GL_TEXTURE_2D, font.textureID);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    FT_Face face = font.face;
    int lineHeight = (face->size->metrics.height >> 6) * scale;
    int ascent = (face->size->metrics.ascender >> 6) * scale;
    int descent = (face->size->metrics.descender >> 6) * scale;
    float xpos = (float)x;
    float ypos = (float)y + (120.0f  * scale);
    FT_UInt previous = 0;
    for (size_t i = 0; text[i] != '\0'; ++i) {
        if (text[i] == '\n') {
            ypos += lineHeight;
            xpos = (float)x;
            previous = 0;
            continue;
        }
        FT_UInt codepoint = (unsigned char)text[i];
        if (codepoint < 32 || codepoint >= 32 + MAX_GLYPHS) continue;
        Glyph* glyph = &font.glyphs[codepoint - 32];
        if (FT_HAS_KERNING(face) && previous) {
            FT_Vector delta;
            FT_Get_Kerning(face, previous, codepoint, FT_KERNING_DEFAULT, &delta);
            xpos += (delta.x >> 6) * scale;
        }
        previous = codepoint;
        float x_start = xpos + glyph->xoff * scale;
        float y_start = ypos - glyph->yoff * scale;
        float w = (glyph->x1 - glyph->x0) * scale;
        float h = (glyph->y1 - glyph->y0) * scale;
        GLfloat vertices[] = {
            x_start,     y_start + h, 0.0f, glyph->u0, glyph->v1,
            x_start + w, y_start + h, 0.0f, glyph->u1, glyph->v1,
            x_start + w, y_start,     0.0f, glyph->u1, glyph->v0,
            x_start,     y_start,     0.0f, glyph->u0, glyph->v0
        };
        GLuint indices[] = {0, 1, 2, 2, 3, 0};
        RenderShaderText((ShaderObject){camera, shaderfont, vertices, indices, sizeof(vertices), sizeof(indices)}, color, fontSize);
        xpos += glyph->xadvance * scale;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void DrawTextEditor(int x, int y, Font font, float fontSize, const char* text, Color color, int cursorStart, int cursorEnd, Shader shaderfont, Shader shaderfontcursor) {
    if (fontSize <= 1.0f) fontSize = 1.0f;
    if (color.a == 0) color.a = 255;
    if (!font.face || !font.textureID) return;
    float scale = fontSize / font.fontSize;
    font = SetFontSize(font, font.fontSize);
    glBindTexture(GL_TEXTURE_2D, font.textureID);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    FT_Face face = font.face;
    int lineHeight = (face->size->metrics.height >> 6) * scale;
    int ascent = (face->size->metrics.ascender >> 6) * scale;
    int descent = (face->size->metrics.descender >> 6) * scale;
    float xpos = (float)x;
    float ypos = (float)y + (120.0f  * scale);
    FT_UInt previous = 0;
    for (size_t i = 0; text[i] != '\0'; ++i) {
        if (text[i] == '\n') {
            ypos += lineHeight;
            xpos = (float)x;
            previous = 0;
            continue;
        }
        FT_UInt codepoint = (unsigned char)text[i];
        if (codepoint < 32 || codepoint >= 32 + MAX_GLYPHS) continue;
        Glyph* glyph = &font.glyphs[codepoint - 32];
        if (FT_HAS_KERNING(face) && previous) {
            FT_Vector delta;
            FT_Get_Kerning(face, previous, codepoint, FT_KERNING_DEFAULT, &delta);
            xpos += (delta.x >> 6) * scale;
        }
        previous = codepoint;
        float x_start = xpos + glyph->xoff * scale;
        float y_start = ypos - glyph->yoff * scale;
        float w = (glyph->x1 - glyph->x0) * scale;
        float h = (glyph->y1 - glyph->y0) * scale;
        GLfloat vertices[] = {
            x_start,     y_start + h, 0.0f, glyph->u0, glyph->v1,
            x_start + w, y_start + h, 0.0f, glyph->u1, glyph->v1,
            x_start + w, y_start,     0.0f, glyph->u1, glyph->v0,
            x_start,     y_start,     0.0f, glyph->u0, glyph->v0
        };
        GLuint indices[] = {0, 1, 2, 2, 3, 0};
        bool isSelected = (i >= cursorStart && i <= cursorEnd);
        if (isSelected) {
            RenderShaderText((ShaderObject){camera, shaderfontcursor, vertices, indices, sizeof(vertices), sizeof(indices)}, color, fontSize);
        } else {
            RenderShaderText((ShaderObject){camera, shaderfont, vertices, indices, sizeof(vertices), sizeof(indices)}, color, fontSize);
        }
        xpos += glyph->xadvance * scale;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void FreeFontCache() {
    FontCacheNode* node = fontCache;
    while (node) {
        FontCacheNode* next = node->next;
        if (node->font.textureID) {
            glDeleteTextures(1, &node->font.textureID);
            node->font.textureID = 0;
        }
        if (node->font.atlasData) {
            free(node->font.atlasData);
            node->font.atlasData = NULL;
        }
        free(node);
        node = next;
    }
    fontCache = NULL;
}
