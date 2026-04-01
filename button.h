#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"

typedef struct Button {
    Rectangle rect;
    Texture2D tex;
    float scale;      // ✨ Current visual scale
    float targetScale; // ✨ 1.0 for normal, 1.2 for hover
} Button;

// ✨ The "Update & Logic" Function
static inline bool GuiButton(Button *btn, Sound clickSound) {
    Vector2 mousePos = GetMousePosition();
    bool hovering = CheckCollisionPointRec(mousePos, btn->rect);
    
    // Smooth Scaling Logic (Lerp-ish)
    btn->targetScale = hovering ? 1.15f : 1.0f;
    btn->scale += (btn->targetScale - btn->scale) * 0.2f; // Smooth transition

    if (hovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        PlaySound(clickSound); // 🔊 CLICK!
        return true;
    }
    return false;
}

// ✨ The "Fancy" Draw Function
static inline void DrawGuiButton(Button btn) {
    // Calculate centered destination based on scale
    float width = btn.rect.width * btn.scale;
    float height = btn.rect.height * btn.scale;
    
    // We adjust the X/Y so it scales from the CENTER of the original rect
    Rectangle drawRect = { 
        btn.rect.x + (btn.rect.width / 2), 
        btn.rect.y + (btn.rect.height / 2), 
        width, 
        height 
    };

    // Origin is half of the NEW width/height to keep it centered
    Vector2 origin = { width / 2, height / 2 };

    Color tint = (btn.targetScale > 1.0f) ? WHITE : Fade(WHITE, 0.8f);
    
    DrawTexturePro(btn.tex, 
                   (Rectangle){0, 0, (float)btn.tex.width, (float)btn.tex.height}, 
                   drawRect, origin, 0.0f, tint);
}

#endif
