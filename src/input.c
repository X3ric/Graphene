#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int KeyChar(const char* character) {
    if (!character) {
        return GLFW_KEY_UNKNOWN;
    }
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
            //case '-': return GLFW_KEY_MINUS;
            case '.': return GLFW_KEY_PERIOD;
            case '/': return GLFW_KEY_SLASH;
            case ';': return GLFW_KEY_SEMICOLON;
            case '=': return GLFW_KEY_EQUAL;
            case '[': return GLFW_KEY_LEFT_BRACKET;
            case '\\': return GLFW_KEY_BACKSLASH;
            case ']': return GLFW_KEY_RIGHT_BRACKET;
            case '`': return GLFW_KEY_GRAVE_ACCENT;
            case '-': return 47;
            case '+': return 93;
            //case '-': return GLFW_KEY_KP_SUBTRACT;
            //case '+': return GLFW_KEY_KP_ADD;
            case '*': return GLFW_KEY_KP_MULTIPLY;
            default: return GLFW_KEY_UNKNOWN;
        }
    }
    // Keypad
        if (strcmp(character, "KpDecimal") == 0) return GLFW_KEY_KP_DECIMAL;
        if (strcmp(character, "KpDivide") == 0) return GLFW_KEY_KP_DIVIDE;
        if (strcmp(character, "KpMultiply") == 0) return GLFW_KEY_KP_MULTIPLY;
        if (strcmp(character, "KpSubtract") == 0) return GLFW_KEY_KP_SUBTRACT;
        if (strcmp(character, "KpAdd") == 0) return GLFW_KEY_KP_ADD;
        if (strcmp(character, "KpEnter") == 0) return GLFW_KEY_KP_ENTER;
        if (strcmp(character, "KpEqual") == 0) return GLFW_KEY_KP_EQUAL;
        if (strcmp(character, "Kp0") == 0) return GLFW_KEY_KP_0;
        if (strcmp(character, "Kp1") == 0) return GLFW_KEY_KP_1;
        if (strcmp(character, "Kp2") == 0) return GLFW_KEY_KP_2;
        if (strcmp(character, "Kp3") == 0) return GLFW_KEY_KP_3;
        if (strcmp(character, "Kp4") == 0) return GLFW_KEY_KP_4;
        if (strcmp(character, "Kp5") == 0) return GLFW_KEY_KP_5;
        if (strcmp(character, "Kp6") == 0) return GLFW_KEY_KP_6;
        if (strcmp(character, "Kp7") == 0) return GLFW_KEY_KP_7;
        if (strcmp(character, "Kp8") == 0) return GLFW_KEY_KP_8;
        if (strcmp(character, "Kp9") == 0) return GLFW_KEY_KP_9;
    // Alphanumeric keys
        if (strcmp(character, "Space") == 0) return GLFW_KEY_SPACE;
        if (strcmp(character, "Apostrophe") == 0) return GLFW_KEY_APOSTROPHE;
        if (strcmp(character, "Comma") == 0) return GLFW_KEY_COMMA;
        if (strcmp(character, "Add") == 0) return GLFW_KEY_KP_ADD;
        if (strcmp(character, "Minus") == 0) return GLFW_KEY_MINUS;
        if (strcmp(character, "Period") == 0) return GLFW_KEY_PERIOD;
        if (strcmp(character, "Slash") == 0) return GLFW_KEY_SLASH;
        if (strcmp(character, "Semicolon") == 0) return GLFW_KEY_SEMICOLON;
        if (strcmp(character, "Equal") == 0) return GLFW_KEY_EQUAL;
        if (strcmp(character, "LeftBracket") == 0) return GLFW_KEY_LEFT_BRACKET;
        if (strcmp(character, "Brackslash") == 0) return GLFW_KEY_BACKSLASH;
        if (strcmp(character, "RightBracket") == 0) return GLFW_KEY_RIGHT_BRACKET;
        if (strcmp(character, "Grave") == 0) return GLFW_KEY_GRAVE_ACCENT;
        if (strcmp(character, "Esc") == 0) return GLFW_KEY_ESCAPE;
        if (strcmp(character, "Enter") == 0) return GLFW_KEY_ENTER;
        if (strcmp(character, "Tab") == 0) return GLFW_KEY_TAB;
        if (strcmp(character, "Backspace") == 0) return GLFW_KEY_BACKSPACE;
        if (strcmp(character, "Insert") == 0) return GLFW_KEY_INSERT;
        if (strcmp(character, "Delete") == 0) return GLFW_KEY_DELETE;
        if (strcmp(character, "Right") == 0) return GLFW_KEY_RIGHT;
        if (strcmp(character, "Left") == 0) return GLFW_KEY_LEFT;
        if (strcmp(character, "Down") == 0) return GLFW_KEY_DOWN;
        if (strcmp(character, "Up") == 0) return GLFW_KEY_UP;
        if (strcmp(character, "PageUp") == 0) return GLFW_KEY_PAGE_UP;
        if (strcmp(character, "PageDown") == 0) return GLFW_KEY_PAGE_DOWN;
        if (strcmp(character, "Home") == 0) return GLFW_KEY_HOME;
        if (strcmp(character, "End") == 0) return GLFW_KEY_END;
        if (strcmp(character, "CapsLock") == 0) return GLFW_KEY_CAPS_LOCK;
        if (strcmp(character, "ScrollLock") == 0) return GLFW_KEY_SCROLL_LOCK;
        if (strcmp(character, "NumLock") == 0) return GLFW_KEY_NUM_LOCK;
        if (strcmp(character, "PrintScreen") == 0) return GLFW_KEY_PRINT_SCREEN;
        if (strcmp(character, "Pause") == 0) return GLFW_KEY_PAUSE;
    // Function keys
        if (strncmp(character, "F", 1) == 0 && strlen(character) > 1) {
            int functionKeyNumber = atoi(character + 1);
            if (functionKeyNumber >= 1 && functionKeyNumber <= 25) {
                return GLFW_KEY_F1 + (functionKeyNumber - 1);
            }
        }
    // Modifier keys
        if (strcmp(character, "LeftShift") == 0) return GLFW_KEY_LEFT_SHIFT;
        if (strcmp(character, "RightShift") == 0) return GLFW_KEY_RIGHT_SHIFT;
        if (strcmp(character, "LeftControl") == 0) return GLFW_KEY_LEFT_CONTROL;
        if (strcmp(character, "RightControl") == 0) return GLFW_KEY_RIGHT_CONTROL;
        if (strcmp(character, "LeftAlt") == 0) return GLFW_KEY_LEFT_ALT;
        if (strcmp(character, "RightAlt") == 0) return GLFW_KEY_RIGHT_ALT;
        if (strcmp(character, "LeftSuper") == 0) return GLFW_KEY_LEFT_SUPER;
        if (strcmp(character, "RightSuper") == 0) return GLFW_KEY_RIGHT_SUPER;
    // Menu key
        if (strcmp(character, "Menu") == 0) return GLFW_KEY_MENU;
    // Mouse Keys
        if (strcmp(character, "Mouse1") == 0) return GLFW_MOUSE_BUTTON_1;
        if (strcmp(character, "Mouse2") == 0) return GLFW_MOUSE_BUTTON_2;
        if (strcmp(character, "Mouse3") == 0) return GLFW_MOUSE_BUTTON_3;
        if (strcmp(character, "Mouse4") == 0) return GLFW_MOUSE_BUTTON_4;
        if (strcmp(character, "Mouse5") == 0) return GLFW_MOUSE_BUTTON_5;
        if (strcmp(character, "Mouse6") == 0) return GLFW_MOUSE_BUTTON_6;
        if (strcmp(character, "Mouse7") == 0) return GLFW_MOUSE_BUTTON_7;
        if (strcmp(character, "Mouse8") == 0) return GLFW_MOUSE_BUTTON_8;
    // Gamepad Keys
        if (strcmp(character, "GamepadA") == 0) return GLFW_GAMEPAD_BUTTON_A;
        if (strcmp(character, "GamepadB") == 0) return GLFW_GAMEPAD_BUTTON_B;
        if (strcmp(character, "GamepadX") == 0) return GLFW_GAMEPAD_BUTTON_X;
        if (strcmp(character, "GamepadY") == 0) return GLFW_GAMEPAD_BUTTON_Y;
        if (strcmp(character, "LeftBumper") == 0) return GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
        if (strcmp(character, "RightBumper") == 0) return GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
        if (strcmp(character, "Back") == 0) return GLFW_GAMEPAD_BUTTON_BACK;
        if (strcmp(character, "Start") == 0) return GLFW_GAMEPAD_BUTTON_START;
        if (strcmp(character, "Guide") == 0) return GLFW_GAMEPAD_BUTTON_GUIDE;
        if (strcmp(character, "LeftThumb") == 0) return GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
        if (strcmp(character, "RightThumb") == 0) return GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
        if (strcmp(character, "L3") == 0) return GLFW_GAMEPAD_BUTTON_LEFT_THUMB; // Alternative label for LeftThumb
        if (strcmp(character, "R3") == 0) return GLFW_GAMEPAD_BUTTON_RIGHT_THUMB; // Alternative label for RightThumb
        if (strcmp(character, "DpadUp") == 0) return GLFW_GAMEPAD_BUTTON_DPAD_UP;
        if (strcmp(character, "DpadRight") == 0) return GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
        if (strcmp(character, "DpadDown") == 0) return GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
        if (strcmp(character, "DpadLeft") == 0) return GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
        if (strcmp(character, "Cross") == 0) return GLFW_GAMEPAD_BUTTON_CROSS; // Alternative label for A
        if (strcmp(character, "Circle") == 0) return GLFW_GAMEPAD_BUTTON_CIRCLE; // Alternative label for B
        if (strcmp(character, "Square") == 0) return GLFW_GAMEPAD_BUTTON_SQUARE; // Alternative label for X
        if (strcmp(character, "Triangle") == 0) return GLFW_GAMEPAD_BUTTON_TRIANGLE; // Alternative label for Y
    // If no match found
    return GLFW_KEY_UNKNOWN;
}

// KEYS

int isKeyDown(const char* character) {
    int key = KeyChar(character);
    return glfwGetKey(window.w, key) == GLFW_PRESS;
}

int isKeyUp(const char* character) {
    int key = KeyChar(character);
    return glfwGetKey(window.w, key) == GLFW_RELEASE;
}

bool isKeyPressed(const char* character, double interval) {
    static int lastKey = -1;
    static double lastPressTime = 0;
    static bool initialPress = true;
    int key = KeyChar(character);
    double currentTime = glfwGetTime();
    if (key == GLFW_KEY_UNKNOWN || key > GLFW_KEY_LAST) {
        return false;
    }
    if (glfwGetKey(window.w, key) == GLFW_PRESS) {
        if (key != lastKey) {
            lastKey = key;
            lastPressTime = currentTime;
            initialPress = true;
            return true;
        } else if (currentTime - lastPressTime > interval && !initialPress) {
            lastPressTime = currentTime;
            return true;
        }

        initialPress = false;
    } else {
        if (key == lastKey) {
            lastKey = -1;
            initialPress = false;
        }
    }
    return false;
}

static int lastState[GLFW_KEY_LAST + 1] = {0};
static int toggleState[GLFW_KEY_LAST + 1] = {0};

int isKey(const char* character) {
    int key = KeyChar(character);
    if (key == GLFW_KEY_UNKNOWN || key > GLFW_KEY_LAST) {
        return false;
    }
    if (isKeyDown(character) && lastState[key] == GLFW_RELEASE) {
        toggleState[key] = !toggleState[key];
    }
    lastState[key] = glfwGetKey(window.w, key);
    return toggleState[key];
}

void isKeyReset(const char* character) {
    int key = KeyChar(character);
    if (key != GLFW_KEY_UNKNOWN && key <= GLFW_KEY_LAST) {
        toggleState[key] = 0;
    }
}

char lastPressedChar = '\0';
void CharCallback(GLFWwindow* glfw_window, unsigned int codepoint) {
    if (codepoint < 128) {
        lastPressedChar = (char)codepoint;
    }
}

void KeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
    const char* actionStrings[] = {"RELEASED", "PRESSED", "REPEATED", "RELASED"};
    const char* actionString = action >= GLFW_PRESS && action <= GLFW_REPEAT ? actionStrings[action] : actionStrings[3];
    char modString[64] = {0};
    if (mods == 0) {
        strcpy(modString, "");
    } else {
        if (mods & GLFW_MOD_SHIFT) strcat(modString, "Shift+");
        if (mods & GLFW_MOD_CONTROL) strcat(modString, "Ctrl+");
        if (mods & GLFW_MOD_ALT) strcat(modString, "Alt+");
        if (mods & GLFW_MOD_SUPER) strcat(modString, "Mod+");
    }
    char keyNameBuffer[64] = {0};
    const char* keyName = glfwGetKeyName(key, scancode);
    if (!keyName) {
        return;
    } else {
        strcpy(keyNameBuffer, keyName);
        if (mods & GLFW_MOD_SHIFT) {
            for (int i = 0; keyNameBuffer[i]; ++i) {
                keyNameBuffer[i] = toupper((unsigned char)keyNameBuffer[i]);
            }
        }
    }
    if(window.debug.input){
        printf("%s[%s] %s %d %d %c\n", modString, keyNameBuffer,actionString, scancode, key,lastPressedChar);
    }
}

// MOUSE

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

Mouse mouse;

Mouse MouseInit() {
    glfwGetCursorPos(window.w, &mouse.x, &mouse.y);
    mouse.scroll.scrolling = (mouse.scroll.x != mouse.scroll.lastx) || (mouse.scroll.y != mouse.scroll.lasty);
    mouse.scroll.lastx = mouse.scroll.x;
    mouse.scroll.lasty = mouse.scroll.y;
    bool moved = (mouse.x != mouse.lastx) || (mouse.y != mouse.lasty);
    return (Mouse){mouse.x, mouse.y, mouse.x, mouse.y, mouse.scroll, moved};
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    mouse.scroll.x += xoffset;
    mouse.scroll.y += yoffset;
}

void SetCursorPos(float x, float y) {
    glfwSetCursorPos(window.w,x,y);
}

static int mouseLastState[GLFW_MOUSE_BUTTON_LAST + 1] = {0};
static int mouseToggleState[GLFW_MOUSE_BUTTON_LAST + 1] = {0};

int isMouseButtonDown(int button) {
    return glfwGetMouseButton(window.w, button) == GLFW_PRESS;
}

int isMouseButtonUp(int button) {
    return glfwGetMouseButton(window.w, button) == GLFW_RELEASE;
}

int isMouseButton(const int button) {
    int currentState = glfwGetMouseButton(window.w, button);
    if (currentState == GLFW_PRESS && mouseLastState[button] == GLFW_RELEASE) {
        mouseToggleState[button] = !mouseToggleState[button];
    }
    mouseLastState[button] = currentState;
    return mouseToggleState[button];
}

void isMouseButtonReset(const int button) {
    if (button <= GLFW_MOUSE_BUTTON_LAST) {
        mouseToggleState[button] = 0;
    }
}

// GAMEPAD & JOYSTICK

#define MAX_JOYSTICKS (GLFW_JOYSTICK_LAST + 1)

typedef struct {
    int joysticks[MAX_JOYSTICKS];
    int count;
} JoystickManager;

static JoystickManager joystickManager;

JoystickManager GetJoysticks(void){
    return joystickManager;
}

static void joystick_callback(int jid, int event) {
    if (event == GLFW_CONNECTED) {
        if (joystickManager.count < MAX_JOYSTICKS) {
            joystickManager.joysticks[joystickManager.count++] = jid;
        }
    } else if (event == GLFW_DISCONNECTED) {
        for (int i = 0; i < joystickManager.count; i++) {
            if (joystickManager.joysticks[i] == jid) {
                joystickManager.joysticks[i] = joystickManager.joysticks[--joystickManager.count];
                break;
            }
        }
    }
}

void LoadJoysticks(void) {
    joystickManager.count = 0;
    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; jid++) {
        if (glfwJoystickPresent(jid)) {
            joystickManager.joysticks[joystickManager.count++] = jid;
        }
    }
    glfwSetJoystickCallback(joystick_callback);
}

const char* GetJoystickName(int jid) {
    const char* name = glfwGetJoystickName(jid);
    return name ? name : "Unknown";
}

bool IsGamepadConnected(int gamepadId) {
    return glfwJoystickPresent(gamepadId) && glfwJoystickIsGamepad(gamepadId);
}

int IsGamepadButtonDown(int gamepadId, const char* buttonName) {
    if (!IsGamepadConnected(gamepadId)) return 0;
    GLFWgamepadstate state;
    if (!glfwGetGamepadState(gamepadId, &state)) return 0;
    int button = KeyChar(buttonName);
    return button != -1 ? state.buttons[button] == GLFW_PRESS : 0;
}

int IsGamepadButtonUp(int gamepadId, const char* buttonName) {
    if (!IsGamepadConnected(gamepadId)) return 0;
    GLFWgamepadstate state;
    if (!glfwGetGamepadState(gamepadId, &state)) return 0;
    int button = KeyChar(buttonName);
    return button != -1 ? state.buttons[button] == GLFW_RELEASE : 0;
}

int IsGamepadButton(const char* character) {
    int key = KeyChar(character);
    if (key == GLFW_KEY_UNKNOWN || key > GLFW_KEY_LAST) {
        return false;
    }
    if (isKeyDown(character) && lastState[key] == GLFW_RELEASE) {
        toggleState[key] = !toggleState[key];
    }
    lastState[key] = glfwGetKey(window.w, key);
    return toggleState[key];
}

void ResetGamepadButton(const char* character) {
    int key = KeyChar(character);
    if (key != GLFW_KEY_UNKNOWN && key <= GLFW_KEY_LAST) {
        toggleState[key] = 0;
    }
}

int GetGamepadAxisValue(const char* axisName) {
    if (!axisName) return -1;
    if (strcmp(axisName, "LeftX") == 0) return GLFW_GAMEPAD_AXIS_LEFT_X;
    if (strcmp(axisName, "LeftY") == 0) return GLFW_GAMEPAD_AXIS_LEFT_Y;
    if (strcmp(axisName, "RightX") == 0) return GLFW_GAMEPAD_AXIS_RIGHT_X;
    if (strcmp(axisName, "RightY") == 0) return GLFW_GAMEPAD_AXIS_RIGHT_Y;
    if (strcmp(axisName, "LeftTrigger") == 0) return GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
    if (strcmp(axisName, "RightTrigger") == 0) return GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
    return -1;
}

float GetGamepadAxis(int gamepadId, const char* axisName) {
    if (!IsGamepadConnected(gamepadId)) return 0.0f;
    GLFWgamepadstate state;
    if (!glfwGetGamepadState(gamepadId, &state)) return 0.0f;
    int axis = GetGamepadAxisValue(axisName);
    if (axis == -1) return 0.0f;
    return state.axes[axis];
}
