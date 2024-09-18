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

int GetGlobalTextPosition(int line, int col) {
    int position = 0;
    for (int i = 0; i < line; ++i) {
        position += strlen(lines[i].text) + 1;
    }
    return position + col;
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

void DeleteSelection() {
    if (selectionStartLine == -1 || selectionEndLine == -1) return;
    int startLine = selectionStartLine;
    int startCol = selectionStartCol;
    int endLine = selectionEndLine;
    int endCol = selectionEndCol;
    if (startLine > endLine || (startLine == endLine && startCol > endCol)) {
        int tempLine = startLine;
        int tempCol = startCol;
        startLine = endLine;
        startCol = endCol;
        endLine = tempLine;
        endCol = tempCol;
    }
    if (startLine == endLine) {
        memmove(&lines[startLine].text[startCol], &lines[startLine].text[endCol], lines[startLine].length - endCol + 1);
        lines[startLine].length -= (endCol - startCol);
    } else {
        strncat(lines[startLine].text + startCol, lines[endLine].text + endCol, MAX_TEXT_LENGTH - startCol - 1);
        lines[startLine].length = strlen(lines[startLine].text);
        int linesToRemove = endLine - startLine;
        for (int i = endLine + 1; i < numLines; i++) {
            lines[i - linesToRemove] = lines[i];
        }
        for (int i = numLines - linesToRemove; i < numLines; i++) {
            free(lines[i].text);
            lines[i].text = NULL;
            lines[i].length = 0;
        }

        numLines -= linesToRemove;
    }
    cursorLine = startLine;
    cursorCol = startCol;
    selectionStartLine = selectionStartCol = selectionEndLine = selectionEndCol = -1;
    isSelecting = false;
}

void KeyCallbackMod(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
    static double lastPressTime = 0.0;
    double currentTime = glfwGetTime();
    bool ctrlPressed = (mods & GLFW_MOD_CONTROL) != 0;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (ctrlPressed) {
            repeatInterval = 0.001;
            if (!isSelecting) {
                selectionStartLine = cursorLine;
                selectionStartCol = cursorCol;
                isSelecting = true;
            }
        } else {
            repeatInterval = 0.03;
            selectionStartLine = selectionStartCol = selectionStartLine = selectionEndCol = -1;
            isSelecting = false;
        }
        HandleKeyRepeat(key, currentTime);
        switch (key) {
            case GLFW_KEY_BACKSPACE:
                if (ctrlPressed) {
                    DeleteSelection();
                } else {
                    DeleteChar(); 
                }
                break;
            case GLFW_KEY_DELETE:
                if (ctrlPressed) {
                    DeleteSelection();
                } else {
                    DeleteCharAfterCursor(); 
                }
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
        if (ctrlPressed && isSelecting) {
            selectionEndLine = cursorLine;
            selectionEndCol = cursorCol;
        }
        AdjustScrollToCursor();
    } else if (action == GLFW_RELEASE) {
        keyStates[key] = false;
        if (!ctrlPressed) {
            isSelecting = false;
            selectionStartLine = selectionStartCol = selectionEndLine = selectionEndCol = -1;
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
            RenderShaderText((ShaderObject){camera, shaderfontcursor, vertices, indices, sizeof(vertices), sizeof(indices)}, color, fontSize);
        } else {
            RenderShaderText((ShaderObject){camera, shaderfont, vertices, indices, sizeof(vertices), sizeof(indices)}, color, fontSize);
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

void DrawTextEditor(Font font, float fontSize, Color textColor, int cursorLine, int cursorCol) {
    int lineHeight = GetTextSize(font, fontSize, "gj|").height;
    int numVisibleLines = window.screen_height / lineHeight;
    int startLine = fmax(0, scrollY / lineHeight);
    int endLine = fmin(numLines, startLine + numVisibleLines);
    char textBlock[1024 * 1024];
    int textBlockLen = 0;
    int cursorPosInTextBlock = -1;
    int selectionStart = -1;
    int selectionEnd = -1;
    int selectionStartGlobal = (isSelecting && selectionStartLine != -1 && selectionStartCol != -1) 
                               ? GetGlobalTextPosition(selectionStartLine, selectionStartCol) : -1;
    int selectionEndGlobal = (isSelecting && selectionEndLine != -1 && selectionEndCol != -1) 
                             ? GetGlobalTextPosition(selectionEndLine, selectionEndCol) : -1;
    if (selectionStartGlobal > selectionEndGlobal) {
        int temp = selectionStartGlobal;
        selectionStartGlobal = selectionEndGlobal;
        selectionEndGlobal = temp;
    }
    if (selectionStartGlobal > selectionEndGlobal) {
        int temp = selectionStartGlobal;
        selectionStartGlobal = selectionEndGlobal;
        selectionEndGlobal = temp;
    }
    for (int i = startLine; i < endLine; i++) {
        int lineLength = strlen(lines[i].text);
        if (i == cursorLine) {
            cursorPosInTextBlock = textBlockLen + cursorCol;
        }
        for (int j = 0; j < lineLength; ++j) {
            char currentChar = lines[i].text[j];
            int currentGlobalPos = GetGlobalTextPosition(i, j);
            if (i == cursorLine && j == cursorCol) {
                if (currentChar == ' ' || currentChar == '\t' || currentChar == '\n' || currentChar == '\0') {
                    currentChar = '_';
                }
            }
            textBlock[textBlockLen++] = currentChar;
            if (isSelecting && selectionStartGlobal != -1 && selectionEndGlobal != -1 && 
                currentGlobalPos >= selectionStartGlobal && currentGlobalPos <= selectionEndGlobal) {
                if (selectionStart == -1) selectionStart = textBlockLen - 1;
                selectionEnd = textBlockLen;
            }
        }
        if (cursorLine == i && cursorCol == lineLength) {
            textBlock[textBlockLen++] = '_';
            textBlock[textBlockLen++] = '\n';
        } else {
            textBlock[textBlockLen++] = '\n';
        }
    }
    textBlock[textBlockLen] = '\0';
    if (isSelecting && selectionStartGlobal != -1 && selectionEndGlobal != -1 && selectionStartGlobal == selectionEndGlobal) {
        selectionStart = cursorPosInTextBlock;
        selectionEnd = cursorPosInTextBlock + 1;
    } else if (!isSelecting || selectionStart == -1 || selectionEnd == -1) {
        selectionStart = selectionEnd = -1;
    }
    if (cursorPosInTextBlock != -1) {
        if (selectionStart == -1 || selectionEnd == -1) {
            selectionStart = cursorPosInTextBlock;
            selectionEnd = cursorPosInTextBlock + 1;
        } else if (cursorPosInTextBlock < selectionStart || cursorPosInTextBlock > selectionEnd) {
            selectionStart = cursorPosInTextBlock;
            selectionEnd = cursorPosInTextBlock + 1;
        }
    }
    DrawTextMod(0, 0, font, fontSize, textBlock, textColor, selectionStart, selectionEnd - 1);
}

int main(int argc, char** argv) {
    WindowInit(1920, 1080, "Grafenic - Text Editor");
    font = LoadFont("./res/fonts/JetBrains.ttf");
    font.nearest = false;
    shaderfontcursor = LoadShader("./res/shaders/default.vert", "./res/shaders/fontcursor.frag");
    shaderfontcursor.hotreloading = true;
    shaderdefault.hotreloading = true;
    shaderfont.hotreloading = true;
    InitializeLine(0);
    glfwSetCharCallback(window.w, CharCallbackMod);
    glfwSetKeyCallback(window.w, KeyCallbackMod);
    glfwSetScrollCallback(window.w, ScrollCallbackMod);
    fontSize = 200.0;
    while (!WindowState()) {
        WindowClear();
        DrawTextEditor(font, Scaling(fontSize), WHITE, cursorLine, cursorCol);
        // Modular ui.h functions
            ExitPromt(font);
        WindowProcess();
    }
    FreeLines();
    WindowClose();
    return 0;
}
