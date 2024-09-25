
void DrawTextRows(Font font,int section,int rows, const char* textContent) {
    int y = window.screen_height / 12;
    int sectionWidth = window.screen_width / rows;
    int fontsize = Scaling(40);
    TextSize textSize = GetTextSize(font, fontsize, textContent);
    int textX = section * sectionWidth + (sectionWidth - textSize.width) / 2;
    int textY = window.screen_height - y/2 - textSize.height/2;
    DrawText(textX, textY , font, fontsize, textContent, WHITE);
} 

void DrawTextColumn(Font font, int section, int totalSections, const char* textContent) {
    int sectionHeight = window.screen_height / totalSections;
    int fontsize = Scaling(35);
    TextSize textSize = GetTextSize(font, fontsize, textContent);
    int textX = 10;
    int textY = section * sectionHeight + (textSize.height);
    DrawText(textX, textY, font, fontsize, textContent, WHITE);
} 

void DrawPopUp(const char* title, Font font, int fontsize, int width, int height) { 
    int x = (window.screen_width / 2) - (width / 2);
    int y = (window.screen_height / 2) - (height / 2);
    DrawRect(x, y, width, height, (Color){50, 50, 50, 100});
    DrawRectBorder(x, y, width, height, Scaling(5), (Color){5, 5, 5, 245});
    TextSize text = GetTextSize(font, fontsize, title);
    int textX = x + (width / 2) - (text.width / 2);
    int textY = y + (height / 2) - (text.height / 2);
    DrawText(textX, textY, font, fontsize, title, WHITE);
}

void Fps(int x, int y, Font font, int size) {
    static double lastTime = 0.0;
    static char fpsText[16];
    double currentTime = window.time;
    snprintf(fpsText, sizeof(fpsText), "FPS: %.0f", window.fps);
    DrawText(x, y, font, size, fpsText, WHITE);
}

void ExitPromt(Font font) {
    if (isKey("Esc")) {
        DrawPopUp("Quit? y/n",font,Scaling(16),window.screen_width/18, window.screen_height/35);
        if (isKeyDown("Y")) {
            WindowStateSet(true);
        }
        if (isKeyDown("N")) {
            isKeyReset("Esc");
        }
    }
} 
