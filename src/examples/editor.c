#include "../window.h"
#include "modules/ui.c"

Font font;
Shader shaderfontcursor;

#define MAX_TEXT_LENGTH 1024
#define MAX_LINES 1024

typedef struct {
    char* text;
    int length;
} Line;

Line lines[MAX_LINES] = {0};
int numLines = 1, cursorLine = 0, cursorCol = 0, scrollY = 0;
int windowWidth = 1920, windowHeight = 1080;

static double lastKeyPressTime[GLFW_KEY_LAST + 1] = {0};
static bool keyStates[GLFW_KEY_LAST + 1] = {false};

int maxVisibleLines;
int lineHeight;
double fontSize;

void InitializeLine(int index) {
    if (index >= MAX_LINES) return;
    lines[index].text = malloc(MAX_TEXT_LENGTH);
    if (!lines[index].text) {
        fprintf(stderr, "Failed to allocate memory for line %d\n", index);
        exit(EXIT_FAILURE);
    }
    lines[index].text[0] = '\0';
    lines[index].length = 0;
}

void FreeLines() {
    for (int i = 0; i < numLines; i++) {
        free(lines[i].text);
    }
}

void InsertChar(char c) {
    if (lines[cursorLine].length < MAX_TEXT_LENGTH - 1) {
        memmove(&lines[cursorLine].text[cursorCol + 1], &lines[cursorLine].text[cursorCol], lines[cursorLine].length - cursorCol + 1);
        lines[cursorLine].text[cursorCol++] = c;
        lines[cursorLine].length++;
    }
}

void DeleteChar() {
    if (cursorCol > 0) {
        memmove(&lines[cursorLine].text[cursorCol - 1], &lines[cursorLine].text[cursorCol], lines[cursorLine].length - cursorCol + 1);
        lines[cursorLine].length--;
        cursorCol--;
    } else if (cursorLine > 0) {
        strcat(lines[cursorLine - 1].text, lines[cursorLine].text);
        lines[cursorLine - 1].length += lines[cursorLine].length;
        free(lines[cursorLine].text);
        memmove(&lines[cursorLine], &lines[cursorLine + 1], (numLines - cursorLine - 1) * sizeof(Line));
        numLines--;
        cursorLine--;
        cursorCol = lines[cursorLine].length;
    }
}

void DeleteCharAfterCursor() {
    if (cursorCol < lines[cursorLine].length) {
        memmove(&lines[cursorLine].text[cursorCol], &lines[cursorLine].text[cursorCol + 1], lines[cursorLine].length - cursorCol);
        lines[cursorLine].length--;
    } else if (cursorLine < numLines - 1) {
        strcat(lines[cursorLine].text, lines[cursorLine + 1].text);
        lines[cursorLine].length += lines[cursorLine + 1].length;
        free(lines[cursorLine + 1].text);
        memmove(&lines[cursorLine + 1], &lines[cursorLine + 2], (numLines - cursorLine - 2) * sizeof(Line));
        numLines--;
    }
}

void InsertNewline() {
    if (numLines >= MAX_LINES) return;
    memmove(&lines[cursorLine + 1], &lines[cursorLine], (numLines - cursorLine) * sizeof(Line));
    numLines++;
    InitializeLine(cursorLine + 1);
    strcpy(lines[cursorLine + 1].text, &lines[cursorLine].text[cursorCol]);
    lines[cursorLine + 1].length = lines[cursorLine].length - cursorCol;
    lines[cursorLine].text[cursorCol] = '\0';
    lines[cursorLine].length = cursorCol;
    cursorLine++;
    cursorCol = 0;
}

void AdjustScrollToCursor() {
    int cursorY = cursorLine * lineHeight;
    int viewStartY = scrollY;
    int viewEndY = scrollY + window.screen_height - lineHeight;
    if (cursorY < viewStartY) {
        scrollY = fmax(0, cursorY - lineHeight);
    } else if (cursorY > viewEndY) {
        scrollY = fmin((numLines - maxVisibleLines + 1) * lineHeight, cursorY - window.screen_height + 1 * lineHeight);
    }
}

void ScrollCallbackMod(GLFWwindow* window, double xoffset, double yoffset) {
    int scrollAmount = (int)(yoffset);
    int newScrollY = scrollY - scrollAmount * lineHeight;
    int maxScroll = (numLines - maxVisibleLines + 1) * lineHeight;
    scrollY = fmax(0, fmin(maxScroll, newScrollY));
}

void CharCallbackMod(GLFWwindow* glfw_window, unsigned int codepoint) {
    if (isprint(codepoint)) InsertChar((char)codepoint);
}

double repeatInterval;

void HandleKeyRepeat(int key, double currentTime) {
    if (!keyStates[key] || (currentTime - lastKeyPressTime[key] > repeatInterval)) {
        keyStates[key] = true;
        lastKeyPressTime[key] = currentTime;
    }
}

int selectionStartLine = -1, selectionStartCol = -1;  // Start of selection
int selectionEndLine = -1, selectionEndCol = -1;      // End of selection
bool isSelecting = false;

void KeyCallbackMod(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
    static double lastPressTime = 0.0;
    double currentTime = glfwGetTime();
    bool ctrlPressed = (mods & GLFW_MOD_CONTROL) != 0;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (ctrlPressed) {
            repeatInterval = 0.001;
        } else {
            repeatInterval = 0.03;
        }
        HandleKeyRepeat(key, currentTime);
        if (ctrlPressed) {
            if (selectionStartLine == -1 && selectionStartCol == -1) {
                selectionStartLine = cursorLine;
                selectionStartCol = cursorCol;
            }
        } else {
            selectionStartLine = selectionStartCol = selectionEndLine = selectionEndCol = -1;
        }
        switch (key) {
            case GLFW_KEY_BACKSPACE: 
                DeleteChar(); 
                break;
            case GLFW_KEY_DELETE: 
                DeleteCharAfterCursor(); 
                break;
            case GLFW_KEY_ENTER: 
                InsertNewline(); 
                break;
            case GLFW_KEY_LEFT: 
                cursorCol = fmax(0, cursorCol - 1); 
                break;
            case GLFW_KEY_RIGHT: 
                cursorCol = fmin(lines[cursorLine].length, cursorCol + 1); 
                break;
            case GLFW_KEY_UP: 
                cursorLine = fmax(0, cursorLine - 1); 
                cursorCol = fmin(cursorCol, lines[cursorLine].length); 
                break;
            case GLFW_KEY_DOWN: 
                cursorLine = fmin(numLines - 1, cursorLine + 1); 
                cursorCol = fmin(cursorCol, lines[cursorLine].length); 
                break;
            case 47: 
                if (ctrlPressed) fontSize = Lerp(fontSize, fontSize - 1.0f, Easing(window.deltatime, "CubicOut"));  
                break;
            case 93: 
                if (ctrlPressed) fontSize = Lerp(fontSize, fontSize + 1.0f, Easing(window.deltatime, "CubicOut")); 
                break;
            case GLFW_KEY_HOME: 
                cursorCol = 0; 
                break;
            case GLFW_KEY_END: 
                cursorCol = lines[cursorLine].length;
                break;
            case GLFW_KEY_PAGE_UP: 
                cursorLine = fmax(0, cursorLine - maxVisibleLines); 
                cursorCol = fmin(cursorCol, lines[cursorLine].length); 
                break;
            case GLFW_KEY_PAGE_DOWN: 
                cursorLine = fmin(numLines - 1, cursorLine + maxVisibleLines); 
                cursorCol = fmin(cursorCol, lines[cursorLine].length); 
                break;
            default:
                break;
        }
        if (ctrlPressed) {
            selectionEndLine = cursorLine;
            selectionEndCol = cursorCol;
        }
        AdjustScrollToCursor();
        const char* keyName = glfwGetKeyName(key, 0);
        if (keyName) {
            int charCode = KeyChar(keyName);
            if (charCode != GLFW_KEY_UNKNOWN) {
                // printf("Key pressed: %s (Keycode: %d)\n", keyName, charCode);
            }
        }
    } else if (action == GLFW_RELEASE) {
        keyStates[key] = false;
        if (!ctrlPressed) {
            isSelecting = false;
        }
    }
}

void DrawTextMod(int x, int y, Font font, float fontSize, const char* text, Color color, int cursorStart, int cursorEnd) {
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
        bool isSelected = (i >= cursorStart && i <= cursorEnd);
        if (isSelected) {
            RenderShaderText((ShaderObject){camera, shaderfontcursor, vertices, indices, sizeof(vertices), sizeof(indices)}, color);
        } else {
            RenderShaderText((ShaderObject){camera, shaderfont, vertices, indices, sizeof(vertices), sizeof(indices)}, color);
        }
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

void DrawTextEditor(Font font, float fontSize, Color textColor, Color lineNumberColor, int cursorLine, int cursorCol) {
    int lineHeight = GetTextSize(font, fontSize, "gj|").height;
    int numVisibleLines = window.screen_height / lineHeight;
    int startLine = fmax(0, scrollY / lineHeight);
    int endLine = fmin(numLines, startLine + numVisibleLines);
    char textBlock[1024 * 1024];
    int textBlockLen = 0;
    int cursorPosInTextBlock = -1;
    int selectionStart = -1;
    int selectionEnd = -1;
    for (int i = startLine; i < endLine; i++) {
        if (i == cursorLine) {
            cursorPosInTextBlock = textBlockLen + cursorCol;
        }
        if (selectionStart == -1) selectionStart = cursorPosInTextBlock;
        if (selectionEnd == -1) selectionEnd = cursorPosInTextBlock;
        textBlockLen += snprintf(textBlock + textBlockLen, sizeof(textBlock) - textBlockLen, "%s\n", lines[i].text);
    }
    textBlock[textBlockLen] = '\0';
    if (cursorPosInTextBlock >= 0) {
        if (cursorPosInTextBlock < selectionStart) selectionStart = cursorPosInTextBlock;
        if (cursorPosInTextBlock > selectionEnd) selectionEnd = cursorPosInTextBlock;
    }
    DrawTextMod(0, 0, font, fontSize, textBlock, textColor, selectionStart, selectionEnd);
}

int main(int argc, char** argv) {
    WindowInit(windowWidth, windowHeight, "Grafenic - Text Editor");
    font = LoadFont("./res/fonts/JetBrains.ttf");
    font.nearest = false;
    shaderfontcursor = LoadShader("./res/shaders/default.vert", "./res/shaders/fontcursor.frag");
    shaderfontcursor.hotreloading = true;
    shaderdefault.hotreloading = true;
    InitializeLine(0);
    glfwSetCharCallback(window.w, CharCallbackMod);
    glfwSetKeyCallback(window.w, KeyCallbackMod);
    glfwSetScrollCallback(window.w, ScrollCallbackMod);
    fontSize = 100.0;
    while (!WindowState()) {
        WindowClear();
        DrawTextEditor(font, Scaling(fontSize), WHITE, GRAY, cursorLine, cursorCol);
        // Modular ui.h functions
            ExitPromt(font);
        WindowProcess();
    }
    FreeLines();
    WindowClose();
    return 0;
}
