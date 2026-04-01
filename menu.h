#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "button.h"

typedef enum GameState { MENU, TRANSITION, PLAYING } GameState;

// ✨ Horizontal Parallax: Moving Left to Right
static inline void DrawParallaxBG(Texture2D bg, float scroll) {
    float s = (float)((int)scroll % bg.width);
    DrawTextureEx(bg, (Vector2){ s, 0 }, 0, 1.0f, WHITE);
    DrawTextureEx(bg, (Vector2){ s - bg.width, 0 }, 0, 1.0f, WHITE);
}

static inline void DrawMenuTitle(Font font, int screenW) {
    const char* line1 = "SLIPPERY";
    const char* line2 = "SLOPES";
    float fontSize = 80.0f; // ✨ Made it a bit bigger since it's stacked!
    float spacing = 2.0f;
    float lineGap = 10.0f;  // ✨ Gap between the two words

    // 1. Measure both lines
    Vector2 size1 = MeasureTextEx(font, line1, fontSize, spacing);
    Vector2 size2 = MeasureTextEx(font, line2, fontSize, spacing);

    // 2. Calculate Positions
    float x1 = (screenW / 2.0f) - (size1.x / 2.0f);
    float x2 = (screenW / 2.0f) - (size2.x / 2.0f);
    float y1 = 120.0f;
    float y2 = y1 + size1.y + lineGap; // Start line 2 after line 1 ends

    // 3. Render Shadows
    DrawTextEx(font, line1, (Vector2){ x1 + 4, y1 + 4 }, fontSize, spacing, BLACK);
    DrawTextEx(font, line2, (Vector2){ x2 + 4, y2 + 4 }, fontSize, spacing, BLACK);

    // 4. Render Main Text
    DrawTextEx(font, line1, (Vector2){ x1, y1 }, fontSize, spacing, RAYWHITE);
    DrawTextEx(font, line2, (Vector2){ x2, y2 }, fontSize, spacing, RAYWHITE);
}

#endif