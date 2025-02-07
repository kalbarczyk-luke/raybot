/*******************************************************************************************
*
*   raylib [core] example - Basic 3d example
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

typedef enum {
    RED_COLOR,
    GREEN_COLOR,
    BLUE_COLOR,
    YELLOW_COLOR
} RandColorEnum;

typedef struct Point{
    Vector2 position; 
    Vector2 velocity; 
    int radius; 
} Point;

typedef struct Box{
    Rectangle rectangle;
    Color color;
    bool pathReady, pathReady2, pathReady3;
} Box;

typedef struct Player{
    Rectangle rec; 
    Vector2 vel;
    int size;
} Player;

void checkPlayerBounds(Player *bot, int screenWidth, int screenHeight);
Color getRandomColor();

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int boxNumber = 10;

    srand(time(NULL));
    
    Point ball;
    ball.position.x = 20.0f;
    ball.position.y = 20.0f;
    ball.velocity.x = 120.0f;
    ball.velocity.y = 120.0f;
    ball.radius = 12;

    InitWindow(screenWidth, screenHeight, "RAYBOT");

    // Rectangle player = { 400, 280, 40, 40 };
    Player bot;
    bot.size = 30;
    bot.rec.x = screenWidth/2.0f;
    bot.rec.y = screenHeight/2.0f;
    bot.rec.height = bot.size;
    bot.rec.width = bot.size;
    bot.vel.x = 480.0f;
    bot.vel.y = 480.0f;

    Box box[10];
    for (int i = 0; i < boxNumber; i++){
        box[i].rectangle.x = -50 - 100*i;
        box[i].rectangle.y = 100;
        box[i].rectangle.width = 30;
        box[i].rectangle.height = 30;
        box[i].color = getRandomColor();
        box[i].pathReady = box[i].pathReady2 = box[i].pathReady3 = false;
    }
    
    Camera2D camera = { 0 };
    camera.target = (Vector2){ bot.rec.x + 20.0f, bot.rec.y + 20.0f };
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    Rectangle tlo[100] = { 0 };
    for (int i = 0; i<100; i++){
        tlo[i].x = (i*100)*pow(-1,i);
        tlo[i].y = GetRandomValue(220, 260); 
        tlo[i].width = 40; 
        tlo[i].height = 100; 
    }
   
    SetTargetFPS(60);                 // Set our game to run at 60 frames-per-second

    while (!WindowShouldClose()) {    // Detect window close button or ESC key
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_D)) bot.rec.x += bot.vel.x * dt;
        else if (IsKeyDown(KEY_A)) bot.rec.x -= bot.vel.x * dt;
        else if (IsKeyDown(KEY_W)) bot.rec.y -= bot.vel.y * dt;
        else if (IsKeyDown(KEY_S)) bot.rec.y += bot.vel.y * dt;
    
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);
        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.1f) camera.zoom = 0.1f;
        
        for (int i = 0; i < boxNumber; i++){
            if (!box[i].pathReady2 && box[i].rectangle.x < 200 && box[i].rectangle.y <= 100) {
                box[i].rectangle.x += 1;
                box[i].pathReady = true;
            }
            else if (box[i].pathReady && box[i].rectangle.y < 500) {
                box[i].rectangle.y += 1;
                box[i].pathReady2 = true;
            }
            else if (box[i].pathReady2 && box[i].rectangle.x > -30) {
                box[i].rectangle.x -= 1;
                box[i].pathReady = false;
                box[i].pathReady3 = true;
            }
            else if (box[i].pathReady3 && box[i].rectangle.x <= -30 && box[i].rectangle.y > 100){
                box[i].rectangle.y -= 1;
                box[i].pathReady2 = false;   
            }
        }
        if (IsKeyPressed(KEY_R)) {
            camera.zoom = 1.0f;
            camera.rotation = 0.0f;
        }
        
        if (ball.position.x >= screenWidth - ball.radius || ball.position.x <= ball.radius ) {
            ball.velocity.x *= -1;
        }
        else if (ball.position.y >= screenHeight - ball.radius || ball.position.y <= ball.radius ) {
            ball.velocity.y *= -1;
        }

        for (int i = 0; i < boxNumber; i++){
            if (CheckCollisionRecs(bot.rec, box[i].rectangle)) DrawText("AUC!", 20, 20, 20, BLACK);
        }
        
        checkPlayerBounds(&bot, screenWidth, screenHeight);

        ball.position.x += ball.velocity.x * dt;
        ball.position.y += ball.velocity.y * dt;
        
        BeginDrawing();

            ClearBackground((Color){145, 145, 145, 255});
            BeginMode2D(camera);
                DrawRectangle(700, 50, 100, 100, Fade(GRAY, 0.9f));
                DrawRectangleLinesEx((Rectangle) {700, 50, 100, 100}, 5.0f, YELLOW);
                DrawRectangle(700, 200, 100, 100, Fade(GRAY, 0.7f));
                DrawRectangleLinesEx((Rectangle) {700, 200, 100, 100}, 5.0f, RED);
                DrawRectangle(700, 350, 100, 100, Fade(GRAY, 0.7f));
                DrawRectangleLinesEx((Rectangle) {700, 350, 100, 100}, 5.0f, GREEN);
                DrawRectangle(700, 500, 100, 100, Fade(GRAY, 0.7f));
                DrawRectangleLinesEx((Rectangle) {700, 500, 100, 100}, 5.0f, BLUE);
                // for (int i = 0; i<20; i++){
                //     DrawRectangleRec(tlo[i], LIGHTGRAY);
                // }
                // DrawRectangle(-6000, 320, 13000, 8000, szary);
                for (int i = 0; i < boxNumber; i++){
                    if (box[i].rectangle.x > -200) DrawRectangleRec(box[i].rectangle, box[i].color);
                }
                DrawRectangle(bot.rec.x, bot.rec.y, bot.size, bot.size, MAGENTA);
            EndMode2D();
            DrawText("Raybot!", screenWidth/2-50, 10, 36, (Color){76, 230, 142, 255});
            DrawCircle(ball.position.x, ball.position.y, ball.radius, ORANGE);

        EndDrawing();   
    }

    CloseWindow();                  // Close window and OpenGL context

    return 0;
}

void checkPlayerBounds(Player *bot, int screenWidth, int screenHeight){
    if (bot->rec.x >= screenWidth - bot->size/2) bot->rec.x = screenWidth - bot->size/2;
    else if (bot->rec.x <= bot->size/2) bot->rec.x = bot->size/2;
    else if (bot->rec.y >= screenHeight - bot->size/2) bot->rec.y = screenHeight - bot->size/2;
    else if (bot->rec.y <= bot->size/2) bot->rec.y = bot->size/2;    
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