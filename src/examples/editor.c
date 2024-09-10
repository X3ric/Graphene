#include "../window.h"
#include "modules/ui.c"

Font font;
Shader custom;

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

int maxLineNumWidth;
int maxLineNumHeight;
int maxVisibleLines;
int fontSize;

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

static double cursorBlinkTime = 0.0;
static const double CURSOR_BLINK_INTERVAL = 0.65;

void AdjustScrollToCursor() {
    int lineHeight = maxLineNumHeight;
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
    int lineHeight = maxLineNumHeight;
    int scrollAmount = (int)(yoffset);
    int newScrollY = scrollY - scrollAmount * lineHeight;
    int maxScroll = (numLines - maxVisibleLines + 1) * lineHeight;
    scrollY = fmax(0, fmin(maxScroll, newScrollY));
}

void DrawTextEditor(Font font, float fontSize, Color textColor, Color lineNumberColor, int cursorLine, int cursorCol) {
    int lineHeight = GetTextSize(font, fontSize, "A").height;
    int startLine = fmax(0, scrollY / lineHeight);
    maxLineNumWidth = GetTextSize(font, fontSize, "9999").width;
    maxLineNumHeight = lineHeight;
    maxVisibleLines = window.screen_height / lineHeight;
    int endLine = fmin(numLines, startLine + maxVisibleLines + 1);
    double currentTime = glfwGetTime();
    bool showCursor = fmod(currentTime, CURSOR_BLINK_INTERVAL) < (CURSOR_BLINK_INTERVAL / 2.0);
    for (int i = startLine; i < endLine; i++) {
        int y = (i - startLine) * lineHeight - (scrollY % lineHeight);
        char lineNum[10];
        snprintf(lineNum, sizeof(lineNum), "%d", i + 1);
        int lineNumberY = y + (lineHeight - GetTextSize(font, fontSize, lineNum).height) / 2;
        DrawText(0, lineNumberY, font, fontSize, lineNum, lineNumberColor);
        int textX = maxLineNumWidth;
        int textY = y + (lineHeight - GetTextSize(font, fontSize, lines[i].text).height) / 2;
        DrawText(textX, textY, font, fontSize, lines[i].text, textColor);
        if (i == cursorLine && showCursor) {
            int cursorX = textX;
            for (int j = 0; j < cursorCol; j++) {
                int glyphIndex = lines[i].text[j] - 32;
                if (glyphIndex >= 0 && glyphIndex < MAX_GLYPHS) {
                    Glyph glyph = font.glyphs[glyphIndex];
                    cursorX += (int)(glyph.xadvance * fontSize / font.fontSize);
                }
            }
            int cursorWidth = maxLineNumWidth / 4;
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            Rect((RectObject){
                { cursorX, y, 0.0f },
                { cursorX + cursorWidth, y, 0.0f },
                { cursorX, y + lineHeight, 0.0f },
                { cursorX + cursorWidth, y + lineHeight, 0.0f },
                custom,
                camera,
            });
            glDisable(GL_BLEND);
        }
    }
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

#include <stdio.h>
#include <GLFW/glfw3.h>

int KeyChar(const char* character) {
    if (!character) return GLFW_KEY_UNKNOWN;

    // Example conversion of printable characters to GLFW key codes
    if (strlen(character) == 1) {
        char ch = character[0];
        if (ch >= 'A' && ch <= 'Z') {
            return GLFW_KEY_A + (ch - 'A');
        }
        if (ch >= 'a' && ch <= 'z') {
            return GLFW_KEY_A + (ch - 'a');
        }
        if (ch >= '0' && ch <= '9') {
            return GLFW_KEY_0 + (ch - '0');
        }
        switch(ch) {
            case ' ': return GLFW_KEY_SPACE;
            case '\'': return GLFW_KEY_APOSTROPHE;
            case ',': return GLFW_KEY_COMMA;
            case '.': return GLFW_KEY_PERIOD;
            case '/': return GLFW_KEY_SLASH;
            case ';': return GLFW_KEY_SEMICOLON;
            case '=': return GLFW_KEY_EQUAL;
            case '[': return GLFW_KEY_LEFT_BRACKET;
            case '\\': return GLFW_KEY_BACKSLASH;
            case ']': return GLFW_KEY_RIGHT_BRACKET;
            case '`': return GLFW_KEY_GRAVE_ACCENT;
            case '-': return GLFW_KEY_MINUS;
            default: return GLFW_KEY_UNKNOWN;
        }
    }
    return GLFW_KEY_UNKNOWN;
}

void KeyCallbackMod(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
    static double lastPressTime = 0.0;
    double currentTime = glfwGetTime();
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        bool ctrlPressed = (mods & GLFW_MOD_CONTROL) != 0;
        if (ctrlPressed) {
            repeatInterval = 0.001;
        } else {
            repeatInterval = 0.03;
        }   
        HandleKeyRepeat(key, currentTime);
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
            case GLFW_KEY_L: 
                cursorCol = fmin(lines[cursorLine].length, cursorCol + 1);
                break;
            case GLFW_KEY_J: 
                cursorLine = fmin(numLines - 1, cursorLine + 1);
                cursorCol = fmin(cursorCol, lines[cursorLine].length);
                break;
            case GLFW_KEY_K: 
                cursorLine = fmax(0, cursorLine - 1);
                cursorCol = fmin(cursorCol, lines[cursorLine].length);
                break;
            case GLFW_KEY_H: 
                cursorCol = fmax(0, cursorCol - 1);
                break;
            default:
                // Handle other keys if needed
                break;
        }
        if (ctrlPressed && key == GLFW_KEY_L) {
            for (int i = 0; i < numLines; i++) {
                free(lines[i].text);
                lines[i].text = malloc(MAX_TEXT_LENGTH);
                lines[i].text[0] = '\0';
                lines[i].length = 0;
            }
            numLines = 1;
            cursorLine = 0;
            cursorCol = 0;
            scrollY = 0;
        }
        AdjustScrollToCursor();
        const char* keyName = glfwGetKeyName(key, 0);
        if (keyName) {
            int charCode = KeyChar(keyName);
            if (charCode != GLFW_KEY_UNKNOWN) {
                printf("Key pressed: %s (Keycode: %d)\n", keyName, charCode);
            }
        }
    } else if (action == GLFW_RELEASE) {
        keyStates[key] = false;
    }
}

int main(int argc, char** argv) {
    WindowInit(windowWidth, windowHeight, "Grafenic - Text Editor");
    font = LoadFont("./res/fonts/JetBrains.ttf");
    font.nearest = false;
    custom = LoadShader("./res/shaders/default.vert", "./res/shaders/cursor.frag");
    custom.hotreloading = true;
    shaderdefault.hotreloading = true;
    InitializeLine(0);
    glfwSetCharCallback(window.w, CharCallbackMod);
    glfwSetKeyCallback(window.w, KeyCallbackMod);
    glfwSetScrollCallback(window.w, ScrollCallbackMod);
    while (!WindowState()) {
        WindowClear();
        fontSize = 32.0;
        DrawTextEditor(font, Scaling(fontSize), WHITE, GRAY, cursorLine, cursorCol);
        // Modular ui.h functions
            ExitPromt(font);
        WindowProcess();
    }
    FreeLines();
    WindowClose();
    return 0;
}
