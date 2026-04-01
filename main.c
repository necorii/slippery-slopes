#include "raylib.h"
#include "constants.h"
#include "particles.h"
#include "score.h" // Implementation is inside your .h
#include "button.h"
#include "menu.h"
#include <stddef.h>

typedef struct Penguin {
    float posX, posY;
    int facing;
} Penguin;

typedef struct Platform {
    Rectangle rect;
} Platform;

// --- GLOBAL RESTART HELPER (The Reset Slay) ---
void ResetGame(Penguin *p, Platform *plats, int *score, int *lastID, bool *gOver, Camera2D *cam, float *vY) {
    *score = 0;
    *lastID = -1;
    *gOver = false;
    *vY = 0;
    
    // Initial Spawn Platform
    plats[0].rect = (Rectangle){ (SCREEN_WIDTH / 2.0f) - 60, SCREEN_HEIGHT - 200, 120, 15 };
    
    // Procedural generation for the rest
    for (int i = 1; i < MAX_PLATFORMS; i++) {
        float nX = plats[i-1].rect.x + GetRandomValue(-80, 80);
        if (nX < 20) nX = 20; 
        if (nX > SCREEN_WIDTH - 140) nX = SCREEN_WIDTH - 140;
        plats[i].rect = (Rectangle){ nX, SCREEN_HEIGHT - 200 - (i * 100), 120, 15 };
    }

    p->posX = plats[0].rect.x + 40;
    p->posY = plats[0].rect.y - 22;
    p->facing = 1;
    
    // Reset Camera to player
    cam->target = (Vector2){ p->posX + 19, p->posY + 12 };
}

int main() {
    // --- 0. INITIALIZATION ---
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Slippery Slopes");
    InitAudioDevice();
    LoadScoreFont();
    InitSnow();
    SetTargetFPS(60);

    // --- 1. LOAD ASSETS ---
    Texture2D idleTex = LoadTexture("idle.png");
    Texture2D runTex = LoadTexture("run.png");
    Texture2D slabTex = LoadTexture("slab.png");
    Texture2D bgTex = LoadTexture("glacier_bg.png");
    Texture2D scoreBoard = LoadTexture("score.png");
    Texture2D medals[4] = { LoadTexture("bronze.png"), LoadTexture("silver.png"), LoadTexture("gold.png"), LoadTexture("platinum.png") };
    Texture2D leftTex = LoadTexture("left.png");
    Texture2D rightTex = LoadTexture("right.png");
    Texture2D pauseTex = LoadTexture("pause.png");
    Texture2D muteTex = LoadTexture("mute.png");
    Texture2D soundTex = LoadTexture("sound.png");
    Texture2D resetTex = LoadTexture("reset.png");
    Texture2D playTex = LoadTexture("play.png");
    Texture2D leaderTex = LoadTexture("leaderboard.png");
    Font menuFont = LoadFont("menu.ttf");

    Sound jumpSound = LoadSound("jump.wav");
    Sound clickSound = LoadSound("click.wav");
    Sound transitionSound = LoadSound("transition.wav");

    // --- 2. STATE & UI SETUP ---
    GameState currentState = MENU;
    float transitionTimer = 0.0f;
    float menuScroll = 0.0f;
    bool isPaused = false, isMuted = false, gameOver = false, moving = false;
    int platformsTouched = 0, bestPlatforms = LoadBestScore(), lastPlatformID = -1;

    // Menu Buttons
    float mBtnW = 80.0f, mBtnH = 48.0f, mGap = 20.0f;
    float startX = (SCREEN_WIDTH / 2.0f) - ((mBtnW * 2 + mGap) / 2.0f);
    Button playBtn = { { startX, SCREEN_HEIGHT/2 + 60, mBtnW, mBtnH }, playTex, 1.0f, 1.0f };
    Button leaderBtn = { { startX + mBtnW + mGap, SCREEN_HEIGHT/2 + 60, mBtnW, mBtnH }, leaderTex, 1.0f, 1.0f };

    // In-Game HUD
    Button pauseBtn = { { SCREEN_WIDTH - 50, 20, 32, 32 }, pauseTex, 1.0f, 1.0f };
    Button muteBtn  = { { SCREEN_WIDTH - 90, 20, 32, 32 }, soundTex, 1.0f, 1.0f };
    Button topResetBtn = { { SCREEN_WIDTH - 130, 20, 32, 32 }, resetTex, 1.0f, 1.0f };
    
    // Death Screen Restart Button (Centered for Touch)
    Button largeResetBtn = { { (SCREEN_WIDTH/2) - 40, (SCREEN_HEIGHT/2) + 65, 80, 80 }, resetTex, 1.0f, 1.0f };

    // Touch Controls
    float moveSize = 90.0f;
    Rectangle leftDest = { 20, (float)SCREEN_HEIGHT - moveSize - 20, moveSize, moveSize };
    Rectangle rightDest = { (float)SCREEN_WIDTH - moveSize - 20, (float)SCREEN_HEIGHT - moveSize - 20, moveSize, moveSize };

    // --- 3. WORLD STATE ---
    Platform platforms[MAX_PLATFORMS];
    Penguin player;
    float velY = 0.0f;
    Camera2D camera = { .offset = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT * 0.7f }, .zoom = 2.5f };
    
    ResetGame(&player, platforms, &platformsTouched, &lastPlatformID, &gameOver, &camera, &velY);

    // --- 4. MAIN LOOP ---
    while(!WindowShouldClose()) {
        switch(currentState) {
            case MENU:
                menuScroll += 1.5f;
                if (GuiButton(&playBtn, clickSound)) {
                    currentState = TRANSITION;
                    transitionTimer = 0.0f;
                    ResetGame(&player, platforms, &platformsTouched, &lastPlatformID, &gameOver, &camera, &velY);
                    PlaySound(transitionSound);
                }
                break;

            case TRANSITION:
                for(int i = 0; i < 15; i++) UpdateSnow();
                transitionTimer += GetFrameTime();
                if (transitionTimer > 1.5f) currentState = PLAYING;
                break;

            case PLAYING:
                UpdateSnow();
                
                // --- HUD INTERACTION ---
                if (!gameOver) {
                    if (GuiButton(&pauseBtn, clickSound)) isPaused = !isPaused;
                    if (GuiButton(&muteBtn, clickSound)) { 
                        isMuted = !isMuted; 
                        muteBtn.tex = isMuted ? muteTex : soundTex; 
                        SetMasterVolume(isMuted ? 0.0f : 1.0f); 
                    }
                    if (GuiButton(&topResetBtn, clickSound) || IsKeyPressed(KEY_R)) {
                        ResetGame(&player, platforms, &platformsTouched, &lastPlatformID, &gameOver, &camera, &velY);
                    }
                }

                if (!gameOver && !isPaused) {
                    moving = false;
                    // Keyboard + Mobile Touch Logic
                    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), leftDest))) { 
                        player.posX -= PENGUIN_SPEED; player.facing = -1; moving = true; 
                    }
                    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) || (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), rightDest))) { 
                        player.posX += PENGUIN_SPEED; player.facing = 1; moving = true; 
                    }

                    velY += GRAVITY; player.posY += velY;

                    // Platform Collision & Recycling
                    for (int i = 0; i < MAX_PLATFORMS; i++) {
                        if (velY > 0 && CheckCollisionRecs((Rectangle){player.posX+10, player.posY+20, 18, 4}, platforms[i].rect)) {
                            player.posY = platforms[i].rect.y - 22; velY = -12.0f; PlaySound(jumpSound);
                            if (lastPlatformID != i) { platformsTouched++; lastPlatformID = i; }
                        }
                        if (platforms[i].rect.y > camera.target.y + 400) {
                            platforms[i].rect.y -= (MAX_PLATFORMS * 100);
                            platforms[i].rect.x = GetRandomValue(20, SCREEN_WIDTH - 140);
                        }
                    }

                    // Fall Death Logic
                    if (player.posY > camera.target.y + 350) {
                        gameOver = true; 
                        if (platformsTouched > bestPlatforms) { 
                            bestPlatforms = platformsTouched; 
                            SaveBestScore(bestPlatforms); 
                        }
                    }
                    
                    // Camera Follow
                    if (player.posY + 12 < camera.target.y) camera.target.y = player.posY + 12;
                    camera.target.x = player.posX + 19;
                }
                break;
        }

        // --- 5. DRAWING (LAYERED VIBES) ---
        BeginDrawing();
        ClearBackground(BLACK);
        
        if (currentState == MENU || currentState == TRANSITION) {
            DrawParallaxBG(bgTex, menuScroll); 
            DrawMenuTitle(menuFont, SCREEN_WIDTH);
            DrawGuiButton(playBtn); 
            DrawGuiButton(leaderBtn);
            if (currentState == TRANSITION) {
                DrawSnow(); 
                float a = (transitionTimer < 0.75f) ? (transitionTimer/0.75f) : (1.0f - (transitionTimer-0.75f)/0.75f);
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(RAYWHITE, a));
            }
        } else {
            // World Rendering
            DrawTexturePro(bgTex, (Rectangle){0,0,bgTex.width,bgTex.height}, (Rectangle){0,0,SCREEN_WIDTH,SCREEN_HEIGHT}, (Vector2){0,0}, 0.0f, WHITE);
            DrawSnow();
            
            BeginMode2D(camera);
            for (int i = 0; i < MAX_PLATFORMS; i++) 
                DrawTexturePro(slabTex, (Rectangle){0,0,606,66}, platforms[i].rect, (Vector2){0,0}, 0.0f, WHITE);
            
            Texture2D pTex = moving ? runTex : idleTex;
            DrawTextureRec(pTex, (Rectangle){0,0,38.0f*player.facing,24.0f}, (Vector2){player.posX, player.posY}, WHITE);
            EndMode2D();

            if (!gameOver) {
                DrawHUD(platformsTouched); 
                DrawGuiButton(pauseBtn); 
                DrawGuiButton(muteBtn); 
                DrawGuiButton(topResetBtn);
                // Draw Touch Arrows
                DrawTexturePro(leftTex, (Rectangle){0,0,130,147}, leftDest, (Vector2){0,0}, 0.0f, Fade(WHITE, 0.4f));
                DrawTexturePro(rightTex, (Rectangle){0,0,130,152}, rightDest, (Vector2){0,0}, 0.0f, Fade(WHITE, 0.4f));
            } else {
                // DRAW ORDER: Scoreboard, then Button on top 🏆🔘
                DrawScoreboard(scoreBoard, medals, platformsTouched, bestPlatforms);
                
                if (GuiButton(&largeResetBtn, clickSound)) {
                    ResetGame(&player, platforms, &platformsTouched, &lastPlatformID, &gameOver, &camera, &velY);
                }
                DrawGuiButton(largeResetBtn); 
            }
        }
        EndDrawing();
    }

    // --- 6. CLEANUP (The Clean Exorcism) ---
    UnloadFont(menuFont); 
    UnloadSound(jumpSound); UnloadSound(clickSound); UnloadSound(transitionSound);
    CloseAudioDevice();
    UnloadTexture(idleTex); UnloadTexture(runTex); UnloadTexture(slabTex); UnloadTexture(bgTex);
    UnloadTexture(playTex); UnloadTexture(leaderTex); UnloadTexture(pauseTex); UnloadTexture(muteTex);
    UnloadTexture(soundTex); UnloadTexture(resetTex); UnloadTexture(scoreBoard); 
    UnloadTexture(leftTex); UnloadTexture(rightTex);
    for(int i=0; i<4; i++) UnloadTexture(medals[i]);
    UnloadScoreFont(); 
    CloseWindow();
    return 0;
}