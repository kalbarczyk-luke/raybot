/*******************************************************************************************
*
*   RAYBOT GAME by kalba - based on raylib [core] examples 
*
*   Welcome to raylib!
*
*   To compile example, just press F5.
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/
#include "raylib.h"
#include "raymath.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"

typedef enum {
    RED_COLOR,
    GREEN_COLOR,
    BLUE_COLOR,
    YELLOW_COLOR
} RandColorEnum;

typedef enum  { 
    HOME=0, 
    GAMEPLAY,
    FREEPLAY, 
    RESULTS
} GameScreen;

typedef struct {
    Vector2 position; 
    Vector2 velocity; 
    int radius; 
} Point;

typedef struct {
    Rectangle rectangle;
    Color color;
    bool pathReady, pathReady2, pathReady3, isPicked, isCollected, wasCollected;
} Box;

typedef struct {
    Rectangle rec; 
    Vector2 vel;
    bool isCarrying;
    int size;
} Player;

typedef struct {
    Rectangle rec;
    Color color;
    bool state;
} Button;

void checkPlayerBounds(Player *bot, int screenWidth, int screenHeight);
void checkBoxBounds(Box *box, int screenWidth, int screenHeight);
void ballin(Point *ball, int screenWidth, int screenHeight, float time);
void initBoxes(Box *box, int iterator);
void shuffleColors(Color *array, size_t n);
void initTargets(Box *target, int iterator, Color *arr);
void DrawMenu(int screenWidth, int screenHeight, Rectangle freeModeButton, Rectangle timeModeButton, Rectangle helpButton, Rectangle resultsButton, Rectangle textBox, Color buttonColor[]);
void DrawMenuUI(int screenWidth, int screenHeight, Button buttons[], Rectangle textBox);
void DrawResults(int screenWidth, int screenHeight, Color buttonColor[], Font font, int textOffset);
void drawBackgroundElements(GameScreen gamemode);
void drawReturnScreen(int screenWidth);
void drawHelpPopUp(Button closeButton);
void writeResult(const char name[], float time);
int recordsCount();
int compareScores_s(void *scores, const void *a, const void *b);
int compareScores(const void *a, const void *b);
Color getRandomColor();
bool colorMatch(Color c1, Color c2);
static float *global_scores;
const char *result_path = "resources/results.dat"; 

#define BRONZE (Color){205, 127, 50, 255}
#define MENU_BUTTON_HOVER (Color){0, 173, 66, 255}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int boxNumber = 10;
    const int maxPoints = 10;
    const int maxCharCount = 9;
    const size_t targetNumber = 4;
    int points = 0, previousPoints = 0, frameCounter = 0, scrollPos = 1;
    float frameTimer = 0.0f, attemptTime = -1.0f;
    bool gameStarted = false, firstSpawn = false, helpPopUp = false;
    bool mouseOnFreeMode = false, mouseOnTimeMode = false, mouseOnHelp = false, mouseOnResults = false;
    bool returnHomeRequested = false;

    char name[maxCharCount + 1];
    int letterCount = 0;
    name[0] = 0;

    Rectangle textBox = { screenWidth/2.0f - 200, 260, 400, 60 };

    Button menuButtons[4] = {
        {
            // freeMode
            .rec   = { screenWidth/2 - 250, screenHeight/2 + 50, 240, 80 },
            .color = DARKGREEN,
            .state = false
        },
        {
            // timeMode
            .rec   = { screenWidth/2 + 10, screenHeight/2 + 50, 240, 80 },
            .color = DARKGREEN,
            .state = false
        },
        {
            // help
            .rec   = { screenWidth/2 - 250, screenHeight/2 + 150, 240, 80 },
            .color = DARKGREEN,
            .state = false
        },
        {
            // results
            .rec   = { screenWidth/2 + 10, screenHeight/2 + 150, 240, 80 },
            .color = DARKGREEN,
            .state = false
        }
    };
    
    Rectangle freeModeButton = { screenWidth/2 - 250, screenHeight/2 + 50, 240, 80 };
    Rectangle timeModeButton = { screenWidth/2 + 10, screenHeight/2 + 50, 240, 80 };
    Rectangle helpButton     = { screenWidth/2 - 250, screenHeight/2 + 150, 240, 80 };
    Rectangle resultsButton  = { screenWidth/2 + 10, screenHeight/2 + 150, 240, 80 };
    
    Button helpCloseButton = {
        .rec = {700, 50, 50, 50},
        .color = MAROON,
        .state = false
    };

    Rectangle resultsBox = { 50, 110, 340, 460};
    // Rectangle helpCloseButton = {700, 50, 50, 50};
    Color buttonColor[5] = {DARKGREEN, DARKGREEN, DARKGREEN, DARKGREEN, DARKGREEN};

    srand(time(NULL));

    Box targets[targetNumber];
    Color usedTargetsColors[targetNumber];
    usedTargetsColors[0] = YELLOW;
    usedTargetsColors[1] = RED;
    usedTargetsColors[2] = GREEN;
    usedTargetsColors[3] = BLUE;
    for (int t = 0; t < targetNumber; t++){
        targets[t].rectangle.x = 700;    
        targets[t].rectangle.y = 50 + 150*t;    
        targets[t].rectangle.width = 100;    
        targets[t].rectangle.height = 100;
        targets[t].color = usedTargetsColors[t];        
    }

    Player bot = {
        bot.size = 50,
        bot.rec.x = screenWidth/2.0f,
        bot.rec.y = screenHeight/2.0f,
        bot.rec.height = bot.size,
        bot.rec.width = bot.size,
        bot.vel.x = 480.0f,
        bot.vel.y = 480.0f,
        bot.isCarrying = false
    };

    Box box[10];
    
    Camera2D camera = { 0 };
    camera.target = (Vector2){ bot.rec.x + 20.0f, bot.rec.y + 20.0f };
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
   
    InitWindow(screenWidth, screenHeight, "RAYBOT");
    InitAudioDevice();
    GameScreen currentScreen = HOME;
    SetTargetFPS(60);

    Texture2D sprite = LoadTexture("resources/raybotYES.png");  
    Rectangle frameRec = { 0.0f, 0.0f, (float)sprite.width/2, (float)sprite.height/5 };
    int currentFrame = 0;
    int currentRow = 0; 
    int boxSpeed = 1;

    Font resultFont = { 0 };
    resultFont = LoadFont("resources/fonts/romulus.png");

    // Sound Effect by u_8e8ungop1x from Pixabay
    Sound popSound = LoadSound("resources/pop.mp3");

    // ===== MAIN LOOP ======
    while (!WindowShouldClose()) {    
        float dt = GetFrameTime();
        frameTimer += dt;

        // === obsluga ekranów ===
        switch (currentScreen){
            case HOME:
                for (int i = 0; i < 4; i++){
                    Button *button = &menuButtons[i];
                    if (CheckCollisionPointRec(GetMousePosition(), button->rec)) {
                        button->state = true;
                        button->color = MENU_BUTTON_HOVER;
                    } else {
                        button->state = false;
                        button->color = DARKGREEN;
                    }
                }
                
                if (!gameStarted && !helpPopUp && menuButtons[1].state && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && strcmp(name, "") != 0) {
                    gameStarted = true;
                    firstSpawn = false;
                    currentScreen = GAMEPLAY;
                } else if (!gameStarted && !helpPopUp && menuButtons[0].state && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    gameStarted = true;
                    firstSpawn = false;
                    currentScreen = FREEPLAY;
                } else if (!gameStarted && menuButtons[2].state && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    helpPopUp = true;
                } else if (!gameStarted && menuButtons[3].state && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    currentScreen = RESULTS;
                }
                
                if (!helpPopUp){
                    int key = GetCharPressed();

                    while (key > 0) {
                        // NOTE: Only allow keys in range [32..125]
                        if ((key >= 32) && (key <= 125) && (letterCount < maxCharCount)) {
                            name[letterCount] = (char)key;
                            name[letterCount+1] = '\0'; // Add null terminator at the end of the string.
                            letterCount++;
                        }
    
                        key = GetCharPressed();  // Check next character in the queue
                    }
    
                    if (IsKeyPressed(KEY_BACKSPACE)) {
                        letterCount--;
                        if (letterCount < 0) letterCount = 0;
                        name[letterCount] = '\0';
                    }    
                }
                
                if (helpPopUp && CheckCollisionPointRec(GetMousePosition(), helpCloseButton.rec)) { 
                    helpCloseButton.color = (Color){ 210, 63, 85, 255 };
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_Q)) {
                        helpPopUp = false;
                    }
                } else {
                    helpCloseButton.color = MAROON;
                }
                break;
            case GAMEPLAY:
                if (IsKeyPressed(KEY_Q)) returnHomeRequested = true;

                if (returnHomeRequested) {
                    if (IsKeyPressed(KEY_Y)){
                        gameStarted = false;
                        points = 0;
                        frameCounter = 0;
                        attemptTime = -1.0f;
                        name[0] = '\0';
                        letterCount = 0;
                        currentScreen = HOME;
                        returnHomeRequested = false;   
                    }
                    else if (IsKeyPressed(KEY_N)) returnHomeRequested = false;
                }
                else {
                    frameCounter++;
                }
                break;
            case FREEPLAY:
                if (IsKeyPressed(KEY_Q)) returnHomeRequested = true;

                if (returnHomeRequested) {
                    if (IsKeyPressed(KEY_Y)){
                        gameStarted = false;
                        points = 0;
                        frameCounter = 0;
                        name[0] = '\0';
                        letterCount = 0;
                        currentScreen = HOME;
                        returnHomeRequested = false;   
                    }
                    else if (IsKeyPressed(KEY_N)) returnHomeRequested = false;
                }
                else {
                    frameCounter++;
                }
                break;
            case RESULTS:
                if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){10, 10, 50, 50})) buttonColor[4] = (Color){0, 173, 66, 255};
                else buttonColor[4] = DARKGREEN;
                if (((CheckCollisionPointRec(GetMousePosition(), (Rectangle){10, 10, 50, 50}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || IsKeyPressed(KEY_Q))){
                    currentScreen = HOME;
                }
                if (CheckCollisionPointRec(GetMousePosition(), resultsBox)) {
                    if (recordsCount() > 11) scrollPos -= (int)(GetMouseWheelMove()*10.0f);
                    if (scrollPos < 1) scrollPos = 1;
                    else if (scrollPos > 369) scrollPos = 369;
                }
                break;
            default: break;
        }

        // === ruch gracza ===
        if (!returnHomeRequested){
            if (IsKeyDown(KEY_D)) {
                bot.rec.x += bot.vel.x * dt;
                currentRow = 3;
            }
            else if (IsKeyDown(KEY_A)) {
                bot.rec.x -= bot.vel.x * dt;
                currentRow = 4;
            } 
            else if (IsKeyDown(KEY_W)) {
                bot.rec.y -= bot.vel.y * dt;
                currentRow = 1;
            } 
            else if (IsKeyDown(KEY_S)) {
                bot.rec.y += bot.vel.y * dt;
                currentRow = 2;        
            }
            else {
                currentRow = 0;
            }
        }

        if (frameTimer >= 0.2f) {
            frameTimer = 0.0f;  
            currentFrame = (currentFrame + 1) % 2;  
        }

        checkPlayerBounds(&bot, screenWidth, screenHeight);

        // TODO: na koniec zablokowac mozliwosc zoomowania
        // camera.zoom += ((float)GetMouseWheelMove()*0.05f);
        // if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        // else if (camera.zoom < 0.1f) camera.zoom = 0.1f;
        
        // === loop-movement boxów ===
        for (int i = 0; i < boxNumber; i++){
            if (!box[i].isPicked && !box[i].isCollected){
                if (!box[i].pathReady2 && box[i].rectangle.x < 200 && box[i].rectangle.y <= 100) {
                    box[i].rectangle.x += boxSpeed;
                    box[i].pathReady = true;
                }
                else if (box[i].pathReady && box[i].rectangle.y < 500) {
                    box[i].rectangle.y += boxSpeed;
                    box[i].pathReady2 = true;
                }
                else if (box[i].pathReady2 && box[i].rectangle.x > -30) {
                    box[i].rectangle.x -= boxSpeed;
                    box[i].pathReady = false;
                    box[i].pathReady3 = true;
                }
                else if (box[i].pathReady3 && box[i].rectangle.x <= -30 && box[i].rectangle.y > 100){
                    box[i].rectangle.y -= 5 * boxSpeed;
                    box[i].pathReady2 = false;   
                }
            }
        }

        if (IsKeyPressed(KEY_R)) {
            camera.zoom = 1.0f;
            camera.rotation = 0.0f;
        }

        // === nowe rundy z nowymi targetami ===
        if (gameStarted) {  
            if (points == 0 && !firstSpawn) { 
                for (int i = 0; i < boxNumber; i++) {
                    initBoxes(&box[i], i);
                }
                firstSpawn = true; 
            }

            if (currentScreen == FREEPLAY){
                if (points != 0 && points % 10 == 0 && points != previousPoints) {
                    previousPoints = points;
                    for (int i = 0; i < boxNumber; i++) {
                        initBoxes(&box[i], i);
                    }
                    shuffleColors(usedTargetsColors, targetNumber);
                    for (size_t i = 0; i < targetNumber; i++) {
                        initTargets(&targets[i], i, usedTargetsColors);
                    }
                }
            }
            else if (currentScreen == GAMEPLAY){
                if ((points != 0 && points % 10 == 0 && points != previousPoints && points < maxPoints) || (attemptTime != -1.0f && IsKeyPressed(KEY_R))) {
                    previousPoints = points;
                    for (int i = 0; i < boxNumber; i++) {
                        initBoxes(&box[i], i);
                    }
                    shuffleColors(usedTargetsColors, targetNumber);
                    for (size_t i = 0; i < targetNumber; i++) {
                        initTargets(&targets[i], i, usedTargetsColors);
                    }
                }                
            }
        }

        // === kolizja bot-box, przenoszenie boxów ===
        for (int i = 0; i < boxNumber; i++) {
            checkBoxBounds(&box[i], screenWidth, screenHeight);

            if (CheckCollisionRecs(bot.rec, box[i].rectangle) && IsKeyDown(KEY_SPACE) && !bot.isCarrying) {
                bot.isCarrying = true;
                box[i].isPicked = true;
            }

            if (box[i].isPicked) {
                box[i].rectangle.x = bot.rec.x + 49;
                box[i].rectangle.y = bot.rec.y + 15;
                DrawCircle(bot.rec.x + 0.5f * bot.rec.width, bot.rec.y, 60.0f, Fade(box[i].color, 0.2f));
            }

            if (!IsKeyDown(KEY_SPACE) && box[i].isPicked) {
                box[i].isPicked = false;
                bot.isCarrying = false;
            }

            for (int j = 0; j < targetNumber; j++){
                if (CheckCollisionRecs(box[i].rectangle, targets[j].rectangle) && colorMatch(box[i].color, targets[j].color) && !box[i].wasCollected) {
                    PlaySound(popSound);
                    box[i].isCollected = true;
                    box[i].isPicked = false;
                    bot.isCarrying = false;
                    box[i].wasCollected = true;
                    points++;
                }
            }
        }
        
        BeginDrawing();

            ClearBackground((Color){145, 145, 145, 255});

            switch (currentScreen){
                case HOME:
                    DrawMenuUI(screenWidth, screenHeight, menuButtons, textBox);
                    // DrawMenu(screenWidth, screenHeight, freeModeButton, timeModeButton, helpButton, resultsButton, textBox, buttonColor);
                    if (name[0] == 0) DrawText("Input your name!", (int)textBox.x + 30, (int)textBox.y + 12, 40, Fade(DARKGRAY, 0.4f)); 
                    else DrawText(name, (int)textBox.x + 5, (int)textBox.y + 12, 40, DARKGREEN);
                    if (helpPopUp) drawHelpPopUp(helpCloseButton);
                    break;
                case GAMEPLAY:
                    BeginMode2D(camera);
                    drawBackgroundElements(currentScreen);
                    DrawText(name, 160, 560, 32, WHITE);
                    // === Rysowanie targetów ===
                    for (int i = 0; i < targetNumber; i++) {
                        DrawRectangleRec(targets[i].rectangle, Fade(GRAY, 0.9f));
                        DrawRectangleLinesEx(targets[i].rectangle, 5.0f, targets[i].color);
                    }
                    // === Rysowanie boxów ===
                    for (int i = 0; i < boxNumber; i++){
                        if (box[i].rectangle.x > -200) DrawRectangleRec(box[i].rectangle, box[i].color);
                    }
                    // === Rysowanie gracza ===
                    frameRec.x = (float)(currentFrame * frameRec.width);
                    frameRec.y = (float)(currentRow * frameRec.height);
                    // DrawRectangle(bot.rec.x, bot.rec.y, bot.size, bot.size, MAGENTA);
                    DrawTextureRec(sprite, frameRec, (Vector2){bot.rec.x, bot.rec.y}, WHITE);
                    EndMode2D();
                    // === Wyswietlanie wyniku i czasu gry ===
                    if (points < maxPoints){
                        DrawText(TextFormat("Score: %i, Time: %02.02f", points, (float)frameCounter/60), 20, 20, 32, WHITE);
                    } else if (attemptTime == -1.0f){
                        attemptTime = (float)frameCounter/60;
                        writeResult(name, attemptTime);
                    }
                    if (attemptTime != -1.0f){
                        bot.rec.x = screenWidth/2.0f;
                        bot.rec.y = screenHeight/2.0f;
                        DrawText(TextFormat("Score: %i, Time: %02.02f", points, attemptTime), 20, 20, 32, WHITE);
                        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.8f));
                        DrawRectangle(screenWidth/2 - 260, screenHeight/2 - 50, 495, 70, (Color){0, 173, 66, 255});
                        DrawRectangleLinesEx((Rectangle){screenWidth/2 - 260, screenHeight/2 - 50, 495, 70}, 3.0f, DARKGREEN);
                        DrawText(TextFormat("Your time: %02.02f s!", attemptTime), screenWidth/2 - 240, screenHeight/2 - 38, 50, WHITE);
                        DrawText("Press Q to return to main menu.", screenWidth/2 - 220, screenHeight/2 + 25, 24, WHITE);
                        DrawText("Press R to play again.", screenWidth/2 - 220, screenHeight/2 + 55, 24, WHITE);
                        if (!returnHomeRequested && IsKeyPressed(KEY_R)){
                            points = 0;
                            frameCounter = 0;
                            attemptTime = -1.0f;
                        }
                    }

                    if (returnHomeRequested) {
                        boxSpeed = 0;
                        drawReturnScreen(screenWidth);
                    }
                    else boxSpeed = 1;

                    DrawFPS(screenWidth - 30, 5);
                    break;
                case FREEPLAY:
                    BeginMode2D(camera);
                    drawBackgroundElements(currentScreen);
                    DrawText(name, 160, 560, 32, WHITE);
                    // === Rysowanie targetów ===
                    for (int i = 0; i < targetNumber; i++) {
                        DrawRectangleRec(targets[i].rectangle, Fade(GRAY, 0.9f));
                        DrawRectangleLinesEx(targets[i].rectangle, 5.0f, targets[i].color);
                    }
                    // === Rysowanie boxów ===
                    for (int i = 0; i < boxNumber; i++){
                        if (box[i].rectangle.x > -200) DrawRectangleRec(box[i].rectangle, box[i].color);
                    }
                    // === Rysowanie gracza ===
                    frameRec.x = (float)(currentFrame * frameRec.width);
                    frameRec.y = (float)(currentRow * frameRec.height);
                    // DrawRectangle(bot.rec.x, bot.rec.y, bot.size, bot.size, MAGENTA);
                    DrawTextureRec(sprite, frameRec, (Vector2){bot.rec.x, bot.rec.y}, WHITE);
                    EndMode2D();
                    // === Wyswietlanie wyniku i czasu gry ===
                    DrawText(TextFormat("Score: %i", points), 20, 20, 32, WHITE);
                    if (returnHomeRequested) {
                        boxSpeed = 0;
                        drawReturnScreen(screenWidth);
                    }
                    else boxSpeed = 1;
                    DrawFPS(screenWidth - 30, 5);
                    break;
                case RESULTS:
                    DrawResults(screenWidth, screenHeight, buttonColor, resultFont, (scrollPos-1)/10);
                    // TODO: adaptive scroll 
                    DrawRectangle(screenWidth/2-40, 140 + scrollPos, 30, 30, LIGHTGRAY);
                    break;
                default: break;
            }

        EndDrawing();   
    }
    UnloadTexture(sprite);
    UnloadSound(popSound);
    UnloadFont(resultFont);
    CloseAudioDevice();
    CloseWindow();                  

    return 0;
}

void checkPlayerBounds(Player *bot, int screenWidth, int screenHeight) {
    if (bot->rec.x >= screenWidth - bot->size/2) bot->rec.x = screenWidth - bot->size/2;
    else if (bot->rec.x <= bot->size/2) bot->rec.x = bot->size/2;
    else if (bot->rec.y >= screenHeight - bot->size/2) bot->rec.y = screenHeight - bot->size/2;
    else if (bot->rec.y <= bot->size/2) bot->rec.y = bot->size/2;    
}

void checkBoxBounds(Box *box, int screenWidth, int screenHeight) {
    if (box->rectangle.x >= screenWidth - box->rectangle.width/2) box->rectangle.x = screenWidth - box->rectangle.width/2;
    else if (box->rectangle.y >= screenHeight - box->rectangle.width/2) box->rectangle.y = screenHeight - box->rectangle.width/2;
    else if (box->rectangle.y <= box->rectangle.width/2) box->rectangle.y = box->rectangle.width/2;    
}

void ballin(Point *ball, int screenWidth, int screenHeight, float time) {
    if (ball->position.x >= screenWidth - ball->radius || ball->position.x <= ball->radius ) {
            ball->velocity.x *= -1;
        }
    else if (ball->position.y >= screenHeight - ball->radius || ball->position.y <= ball->radius ) {
        ball->velocity.y *= -1;
    }

    ball->position.x += ball->velocity.x * time;
    ball->position.y += ball->velocity.y * time;
}

void initBoxes(Box *box, int iterator) {
    box->rectangle.x = -50 - 100*iterator;
    box->rectangle.y = 100;
    box->rectangle.width = 30;
    box->rectangle.height = 30;
    box->color = getRandomColor();
    box->pathReady = box->pathReady2 = box->pathReady3 = box->isPicked = box->isCollected = box->wasCollected = false;
}

void initTargets(Box *target, int iterator, Color *arr) {
    target->color = arr[iterator];      
}

Color getRandomColor() {
    RandColorEnum randomColor = rand() % 4; 

    switch (randomColor) {
        case RED_COLOR: return RED;
        case GREEN_COLOR: return GREEN;
        case BLUE_COLOR: return BLUE;
        case YELLOW_COLOR: return YELLOW;
        default: return WHITE; 
    }
}

bool colorMatch(Color c1, Color c2) {
    return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a);
}

void shuffleColors(Color *array, size_t n) {
    if (n > 1) {
        for (size_t i = 0; i < n - 1; i++) {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          Color t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void DrawMenu(int screenWidth, int screenHeight, Rectangle freeModeButton, Rectangle timeModeButton, Rectangle helpButton, Rectangle resultsButton, Rectangle textBox, Color buttonColor[]){
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){76, 230, 142, 255});
    DrawText("RAYBOT", 165, screenHeight/2 - 200, 120, DARKGREEN);
    DrawRectangleRec(freeModeButton, buttonColor[0]);
    DrawText("Free Play", screenWidth/2 - 230, screenHeight/2 + 70, 40, WHITE);
    DrawRectangleRec(timeModeButton, buttonColor[1]);
    DrawText("Time Play", screenWidth/2 + 35, screenHeight/2 + 70, 40, WHITE);
    DrawRectangleRec(helpButton, buttonColor[2]);
    DrawText("Help", screenWidth/2 - 170, screenHeight/2 + 170, 40, WHITE);
    DrawRectangleRec(resultsButton, buttonColor[3]);
    DrawText("Results", screenWidth/2 + 55, screenHeight/2 + 170, 40, WHITE);
    DrawRectangleRec(textBox, LIGHTGRAY);
    DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);
}

void DrawMenuUI(int screenWidth, int screenHeight, Button buttons[], Rectangle textBox) {
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){76, 230, 142, 255});
    DrawText("RAYBOT", 165, screenHeight/2 - 200, 120, DARKGREEN);
    DrawRectangleRec(buttons[0].rec, buttons[0].color);
    DrawText("Free Play", screenWidth/2 - 230, screenHeight/2 + 70, 40, WHITE);
    DrawRectangleRec(buttons[1].rec, buttons[1].color);
    DrawText("Time Play", screenWidth/2 + 35, screenHeight/2 + 70, 40, WHITE);
    DrawRectangleRec(buttons[2].rec, buttons[2].color);
    DrawText("Help", screenWidth/2 - 170, screenHeight/2 + 170, 40, WHITE);
    DrawRectangleRec(buttons[3].rec, buttons[3].color);
    DrawText("Results", screenWidth/2 + 55, screenHeight/2 + 170, 40, WHITE);
    DrawRectangleRec(textBox, LIGHTGRAY);
    DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);
}

void DrawResults(int screenWidth, int screenHeight, Color buttonColor[], Font font, int textOffset) {
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){76, 230, 142, 255});
    DrawRectangle(10, 10, 50, 50, Fade(buttonColor[4], 0.9f));
    DrawRectangle(25, 37, 20, 13, Fade(WHITE, 0.9f));
    DrawTriangle((Vector2){50,35}, (Vector2){35,20}, (Vector2){20,35}, Fade(WHITE, 0.9f));

    DrawRectangle(20, 100, screenWidth - 2*20, screenHeight - 100 - 20, WHITE);
    DrawLineEx((Vector2){screenWidth/2, 110}, (Vector2){screenWidth/2, screenHeight - 40}, 3.0f, GRAY);
    DrawRectangle(screenWidth/2-40, 110, 30, 30, LIGHTGRAY);
    DrawTriangle((Vector2){screenWidth/2 - 15,135}, (Vector2){screenWidth/2 - 25,115}, (Vector2){screenWidth/2 - 35,135}, Fade(WHITE, 0.9f));
    DrawRectangle(screenWidth/2-40, 540, 30, 30, LIGHTGRAY);
    DrawTriangle((Vector2){screenWidth/2 - 15,545}, (Vector2){screenWidth/2 - 35,545}, (Vector2){screenWidth/2 - 25,565}, Fade(WHITE, 0.9f));
    DrawRectangle(screenWidth/2-40, 140, 30, 400, Fade(LIGHTGRAY, 0.5f));
    DrawText("HOME", 12, 70, 18, DARKGREEN);
    DrawText("Time Play Results", 210, 25, 46, DARKGREEN);
    DrawTextEx(font, "TOP RESULTS", (Vector2) {440, 120}, 40, 5.0f, MAROON);

    // file results
    Vector2 offset =  {0, 0};
    FILE *file = fopen(result_path, "r");
    if (!file) {
        DrawTextEx(font, "No results found", (Vector2){50, 120}, 32, 1, RED);
        return;
    }

    char results[100][24] = {0};
    float scores[100] = {0.0f};
    
    Color medals[5] = {GOLD, LIGHTGRAY, BRONZE, BLACK, BLACK};
    int indices[100]; 
    int totalResults = 0, resultID;
    
    while (totalResults < 100 && fgets(results[totalResults], 24, file) != NULL) { 
        // results[totalResults][strcspn(results[totalResults], "\n")] = 0;  // Usunięcie `\n`
        indices[totalResults] = totalResults;
        totalResults++;
    }
    fclose(file);
    if (totalResults == 0) {
        DrawTextEx(font, "No results yet!", (Vector2){50, 120}, 40, 1, MAROON);
        DrawTextEx(font, "It's empty here...", (Vector2) {440, 180}, 24, 2.0f, BLACK);
        return;
    }
    
    if (totalResults < 12) resultID = 0;
    else resultID = textOffset;
    
    for (int i = resultID; i < 11+resultID; i++) {
        DrawTextEx(font, results[i], (Vector2){50, 120 + offset.y}, 32, 1, BLACK);
        offset.y += 40;
    }

    for (int i = 0; i < totalResults; i++){
        char *token = strtok(results[i], "-");
        if (token) {
            token = strtok(NULL, "-");  
            if (token) {
                scores[i] = strtof(token, NULL);
            } else {
                scores[i] = 99999.0f; 
            }
        } else {
            scores[i] = 99999.0f; 
        }
    }

    #ifdef __linux__ 
        global_scores = scores;
        qsort(indices, totalResults, sizeof(int), compareScores);
    #elif _WIN32
        qsort_s(indices, totalResults, sizeof(int), compareScores_s, scores);
    #else

    #endif
    
    for (int i = 0; i < 5 && i < totalResults; i++) {
        int sortedIndex = indices[i];
        DrawTextEx(font, TextFormat("#%i ",i+1), (Vector2) {440, 180+40*i}, 32, 1, medals[i]);  
        DrawTextEx(font, TextFormat("%s- %.2f",results[sortedIndex], scores[sortedIndex]), (Vector2){490, 180 + 40 * i}, 32, 1, BLACK);
    }
}

int compareScores_s(void *scores, const void *a, const void *b) {
    int idxA = *(const int *)a;
    int idxB = *(const int *)b;
    float *scoreArray = (float *)scores;
    
    return (scoreArray[idxA] > scoreArray[idxB]) - (scoreArray[idxA] < scoreArray[idxB]);
}

int compareScores(const void *a, const void *b) {
    int idxA = *(const int *)a;
    int idxB = *(const int *)b;
    // float *scoreArray = (float *)scores;

    return (global_scores[idxA] > global_scores[idxB]) - (global_scores[idxA] < global_scores[idxB]);
}

int recordsCount() {
    int lines = 0;
    char ch;
    FILE *file = fopen(result_path, "r");
    if (file != NULL) {
        while ((ch = fgetc(file)) != EOF) { 
            if (ch == '\n') { 
                lines++;
            }
        }
    }
    fclose(file);
    return lines;
}

void drawBackgroundElements(GameScreen gamemode) {
    for (int i = 20; i < 1000; i += 60){
        DrawLine(i,0,i,1000, Fade(BLACK, 0.5f));
        DrawLine(0,i,1000,i, Fade(BLACK, 0.5f));
    }
    DrawRectangle(30, 550, 330, 50, Fade(DARKGRAY, 0.7f));
    DrawText("Player: ", 40, 560, 32,WHITE);
    if (gamemode == FREEPLAY){
        DrawRectangle(30, 30, 220, 50, Fade(DARKGRAY, 0.7f));
    } else if (gamemode == GAMEPLAY){
        DrawRectangle(30, 30, 350, 50, Fade(DARKGRAY, 0.7f));
    }
}

void drawReturnScreen(int screenWidth) {
    DrawRectangle(0, 0, screenWidth, 200, Fade(DARKGRAY, 0.2f));
    DrawRectangle(0, 400, screenWidth, 200, Fade(DARKGRAY, 0.2f));
    DrawRectangle(0, 200, screenWidth, 200, BLACK);
    DrawText("Are you sure you want to exit game? [Y/N]", 40, 280, 30, WHITE);
}

void drawHelpPopUp(Button closeButton){
    DrawRectangle(50, 50, 700, 500, Fade(DARKGRAY, 0.95f));
    DrawRectangleRec(closeButton.rec, Fade(closeButton.color, 0.9f));
    DrawLineEx((Vector2) {710, 60}, (Vector2) {740, 90}, 3.0f, Fade(WHITE, 0.9f));
    DrawLineEx((Vector2) {740, 60}, (Vector2) {710, 90}, 3.0f, Fade(WHITE, 0.9f));
    DrawText("RAYBOT - HELP", 240, 60, 40, Fade(WHITE, 0.9f));
    DrawText("Raybot! Collect and sort the boxes.", 60, 140, 32, Fade(WHITE, 0.9f)); 
    DrawText("Controls: ", 60, 200, 32, Fade(WHITE, 0.9f)); 
    DrawText("WASD - raybot movement", 60, 240, 32, Fade(WHITE, 0.9f)); 
    DrawText("SPACE - pick up a box", 60, 280, 32, Fade(WHITE, 0.9f));     
    DrawText("Q - exit", 60, 320, 32, Fade(WHITE, 0.9f));  
    DrawText("Gamemodes: ", 60, 380, 32, Fade(WHITE, 0.9f));  
    DrawText("Free play: relax with no time limit!", 60, 420, 32, Fade(WHITE, 0.9f));  
    DrawText("Time play: collect 10 boxes as", 60, 460, 32, Fade(WHITE, 0.9f));  
    DrawText("fast as possible!", 227, 490, 32, Fade(WHITE, 0.9f));  
}

void writeResult(const char name[], float time) {
    FILE *fptr;
    
    // Otwieranie pliku w trybie dopisywania ("a")
    fptr = fopen(result_path, "a");
    if (fptr != NULL) {
        fprintf(fptr, "%s - %.2f\n", name, time);
    }

    fclose(fptr);
}
