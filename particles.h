#ifndef PARTICLES_H
#define PARTICLES_H

#include "raylib.h"

#define MAX_SNOW 100

typedef struct SnowParticle {
    Vector2 position;
    float speed;
    float size;
} SnowParticle;

static SnowParticle snow[MAX_SNOW];

void InitSnow() {
    for (int i = 0; i < MAX_SNOW; i++) {
        snow[i].position = (Vector2){ GetRandomValue(0, GetScreenWidth()), GetRandomValue(0, GetScreenHeight()) };
        snow[i].speed = GetRandomValue(50, 150) / 100.0f;
        snow[i].size = GetRandomValue(1, 3);
    }
}

void UpdateSnow() {
    for (int i = 0; i < MAX_SNOW; i++) {
        snow[i].position.y += snow[i].speed;
        snow[i].position.x += GetRandomValue(-1, 1) * 0.2f; // subtle drift 🌬️
        
        if (snow[i].position.y > GetScreenHeight()) {
            snow[i].position.y = -10;
            snow[i].position.x = GetRandomValue(0, GetScreenWidth());
        }
    }
}

void DrawSnow() {
    for (int i = 0; i < MAX_SNOW; i++) {
        DrawCircleV(snow[i].position, snow[i].size, Fade(WHITE, 0.6f));
    }
}

#endif
