#ifndef SCORE_H
#define SCORE_H

#include "raylib.h"
#include <stdio.h>
#include <stddef.h>

// --- FUNCTION PROTOTYPES (The "Head's Up" for the compiler) ---
void LoadScoreFont(void);
void UnloadScoreFont(void);
void DrawHUD(int current);
void DrawScoreboard(Texture2D board, Texture2D *medals, int current, int best);
int LoadBestScore(void);
void SaveBestScore(int score);

// --- IMPLEMENTATION ---

static Font scoreFont;

void LoadScoreFont() {
    scoreFont = LoadFont("score.ttf");
}

void UnloadScoreFont() {
    UnloadFont(scoreFont);
}

int LoadBestScore() {
    int score = 0;
    FILE *file = fopen("highscore.txt", "r");
    if (file != NULL) {
        fscanf(file, "%d", &score);
        fclose(file);
    }
    return score;
}

void SaveBestScore(int score) {
    FILE *file = fopen("highscore.txt", "w");
    if (file != NULL) {
        fprintf(file, "%d", score);
        fclose(file);
    }
}

// ✨ New: Draws the live counter at the top center
void DrawHUD(int current) {
    const char* text = TextFormat("%d", current);
    float fontSize = 40.0f;
    float spacing = 2.0f;

    // Calculate center of screen
    Vector2 textSize = MeasureTextEx(scoreFont, text, fontSize, spacing);
    Vector2 position = { (GetScreenWidth() / 2.0f) - (textSize.x / 2.0f), 20 };

    // Draw shadow for readability 🌑
    DrawTextEx(scoreFont, text, (Vector2){ position.x + 2, position.y + 2 }, fontSize, spacing, BLACK);
    // Draw main text
    DrawTextEx(scoreFont, text, position, fontSize, spacing, WHITE);
}

void DrawScoreboard(Texture2D board, Texture2D *medals, int current, int best) {
    float boardScale = 3.0f;
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    Rectangle dest = {
            sw/2 - (board.width * boardScale)/2,
            sh/2 - (board.height * boardScale)/2,
            board.width * boardScale, board.height * boardScale
    };

    DrawTexturePro(board, (Rectangle){0,0,113,57}, dest, (Vector2){0,0}, 0.0f, WHITE);

    // Medal Logic 🏅
    Texture2D *m = NULL;
    if (current >= 100) m = &medals[3];
    else if (current >= 50) m = &medals[2];
    else if (current >= 25) m = &medals[1];
    else if (current >= 10) m = &medals[0];

    if (m != NULL) {
        DrawTexturePro(*m, (Rectangle){0,0,22,22},
                       (Rectangle){dest.x + (24 * boardScale), dest.y + (32 * boardScale), 22 * boardScale, 22 * boardScale},
                       (Vector2){11 * boardScale, 11 * boardScale}, 0.0f, WHITE);
    }

    // Centered Text on Scoreboard using score.ttf
    const char* curT = TextFormat("%d", current);
    const char* bstT = TextFormat("%d", best);

    Vector2 curSize = MeasureTextEx(scoreFont, curT, 20, 2);
    Vector2 bstSize = MeasureTextEx(scoreFont, bstT, 20, 2);

    DrawTextEx(scoreFont, curT, (Vector2){ dest.x + (91 * boardScale) - (curSize.x/2), dest.y + (19 * boardScale) - 10 }, 20, 2, WHITE);
    DrawTextEx(scoreFont, bstT, (Vector2){ dest.x + (93 * boardScale) - (bstSize.x/2), dest.y + (43 * boardScale) - 10 }, 20, 2, YELLOW);
}

#endif