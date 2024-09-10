#include "../window.h"
#include "modules/ui.c"

Font font;

void Draw2DAxes(int x, int y, int jid, int width, int height) {
    static const char* axisGroups[][2] = {
        {"LeftX", "LeftY"},
        {"RightX", "RightY"}
    };
    const int circleBorderThickness = Scaling(4);
    const int limitCircleRadius = Scaling(45);
    const int indicatorCircleRadius = Scaling(15);
    const int labelSpacing = Scaling(3);
    bool l3Pressed = IsGamepadButtonDown(jid,"L3");
    bool r3Pressed = IsGamepadButtonDown(jid,"R3");
    bool isAnyButtonPressed = l3Pressed || r3Pressed;
    Color pointerColor = isAnyButtonPressed ? GREEN : GRAY;
    for (size_t group = 0; group < sizeof(axisGroups) / sizeof(axisGroups[0]); group++) {
        int groupX = x  + group * (width + Scaling(50));
        int groupY = y;
        bool l3Pressed = IsGamepadButtonDown(jid, "L3");
        bool r3Pressed = IsGamepadButtonDown(jid, "R3");
        bool isAnyButtonPressed = (group == 0 && l3Pressed) || (group == 1 && r3Pressed);
        Color pointerColor = isAnyButtonPressed ? GREEN : GRAY;
        DrawCircle(groupX + width / 2, groupY + height / 2, limitCircleRadius, LIGHTGRAY);
        DrawCircleBorder(groupX + width / 2, groupY + height / 2, limitCircleRadius, circleBorderThickness, (Color){255,255,255,120});
        float xValue = GetGamepadAxis(jid, axisGroups[group][0]);
        float yValue = GetGamepadAxis(jid, axisGroups[group][1]);
        xValue = xValue < -1.0f ? -1.0f : (xValue > 1.0f ? 1.0f : xValue);
        yValue = yValue < -1.0f ? -1.0f : (yValue > 1.0f ? 1.0f : yValue);
        int centerX = groupX + width / 2;
        int centerY = groupY + height / 2;
        int visualX = centerX + (int)(xValue * (limitCircleRadius - indicatorCircleRadius));
        int visualY = centerY + (int)(yValue * (limitCircleRadius - indicatorCircleRadius));
        DrawLine(centerX, centerY, visualX, visualY, circleBorderThickness, (Color){255,255,255,120});
        DrawCircle(visualX, visualY, indicatorCircleRadius, pointerColor);
        DrawCircleBorder(visualX, visualY, indicatorCircleRadius, Scaling(3), (Color){255,255,255,120});
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "X: %.2f Y: %.2f", xValue, yValue);
        TextSize labelSize = GetTextSize(font, Scaling(20), buffer);
        int textX = groupX + (width - labelSize.width) / 2;
        int textY = groupY + height + labelSpacing;
        DrawText(textX, textY, font, Scaling(20), buffer, WHITE);
    }
}

void DrawTriggers(int x, int y, int jid, int width, int height) {
    static const char* triggerNames[] = {
        "LeftTrigger", "RightTrigger"
    };
    static const char* visualNames[] = {
        "LT", "RT"
    };
    for (size_t t = 0; t < sizeof(triggerNames) / sizeof(triggerNames[0]); t++) {
        float triggerValue = GetGamepadAxis(jid, triggerNames[t]);
        triggerValue = triggerValue < 0.0f ? 0.0f : (triggerValue > 1.0f ? 1.0f : triggerValue);
        int triggerBarHeight = (int)(height * triggerValue);
        char buffer[256];
        char bufferval[256];
        snprintf(buffer, sizeof(buffer), "%s", visualNames[t]);
        snprintf(bufferval, sizeof(bufferval), "%.2f", triggerValue);
        TextSize valSize = GetTextSize(font, Scaling(24), buffer);
        TextSize labelSize = GetTextSize(font, Scaling(24), buffer);
        int visualX = x + t * (width + Scaling(250));
        int visualY = y + Scaling(30);
        int valX = visualX + (width - valSize.width) / 2 - (valSize.width / 2);
        int valY = visualY + Scaling(5);
        int textX = visualX + (width - labelSize.width) / 2;
        int textY = visualY - Scaling(5) - labelSize.height;
        DrawRect(visualX, visualY + (height - triggerBarHeight), width, triggerBarHeight, GRAY);
        DrawRectBorder(visualX, visualY, width, height, Scaling(2), WHITE);
        DrawText(textX, textY, font, Scaling(24), buffer, WHITE);
        DrawText(valX, valY + height, font, Scaling(24), bufferval, WHITE);
    }
}

void DrawGamepadInfo(int x, int y, int jid) {
    if (!IsGamepadConnected(jid)) return;
    char buffer[256];
    const char* joystickName = GetJoystickName(jid);
    // Gamepad Info
    snprintf(buffer, sizeof(buffer), "Gamepad %d (%s)", jid, GetJoystickName(jid));
    TextSize labelSize = GetTextSize(font, Scaling(24), buffer);
    DrawText(x, y, font, Scaling(24), buffer, WHITE);
    snprintf(buffer, sizeof(buffer), "Connected");
    TextSize statusSize = GetTextSize(font, Scaling(24), buffer);
    int statusX = x + labelSize.width + Scaling(20);
    DrawText(statusX, y, font, Scaling(24), buffer, GREEN);
    y += Scaling(15);
    // Draw Input
    DrawTriggers(x + Scaling(275), y, jid, Scaling(25), Scaling(75));
    Draw2DAxes(x + Scaling(249), y + Scaling(215), jid, Scaling(150), Scaling(150));
    // Buttons information
    DrawText(x, y, font, Scaling(22), "  Buttons:", WHITE);
    y += Scaling(25);
    static const char* buttonNames[] = {
        "GamepadA", "GamepadB", "GamepadX", "GamepadY", "LeftBumper", "RightBumper", "Back", "Start",
        "Guide", "LeftThumb", "RightThumb", "DpadUp", "DpadRight",
        "DpadDown", "DpadLeft", "Cross", "Circle", "Square", "Triangle"
    };
    x += Scaling(30);
    for (int b = 0; b < sizeof(buttonNames) / sizeof(buttonNames[0]); b++) {
        snprintf(buffer, sizeof(buffer), "%s: %s", buttonNames[b], IsGamepadButtonDown(jid, buttonNames[b]) ? "Pressed" : "Released");
        
        DrawText(x, y, font, Scaling(20), buffer, WHITE);
        y += Scaling(20);
    }
    // Extra spacing between gamepads
    y += Scaling(30);
}

int main(int argc, char** args) {
    WindowInit(1920, 1080, "Grafenic - Gamepad");
    font = LoadFont("./res/fonts/Monocraft.ttf");
    font.nearest = true;
    LoadJoysticks();
    ClearColor((Color){75, 75, 75, 100});
    while (!WindowState()) {
        WindowClear();
        // Gamepad Visual
            int x = Scaling(10);
            int y = Scaling(10);
            bool anyGamepadFound = false;
            JoystickManager joystickmanager = GetJoysticks();
            for (int i = 0; i < joystickmanager.count; i++) {
                int joystick = joystickmanager.joysticks[i];
                if (IsGamepadConnected(joystick)) {
                    anyGamepadFound = true;
                    DrawGamepadInfo(x, y, joystick);
                }
            }
            if (!anyGamepadFound) {
                DrawText(x, y, font, Scaling(24), "No Gamepad Found", RED);
            }
        // Modular ui.h functions
            ExitPromt(font);  
        WindowProcess();
    }
    WindowClose();
    return 0;
}
