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
#include "math.h"

typedef struct Point{
    Vector2 position; 
    Vector2 velocity; 
    int radius; 
} Point;


int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const Color szary = {54, 54, 54, 255};

    Point ball;
    ball.position.x = 20.0f;
    ball.position.y = 20.0f;
    ball.velocity.x = 120.0f;
    ball.velocity.y = 120.0f;
    ball.radius = 12;

    InitWindow(screenWidth, screenHeight, "raylib testy");

    Rectangle player = { 400, 280, 40, 40 };
    
    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.x + 20.0f, player.y + 20.0f };
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
        if (IsKeyDown(KEY_D)) player.x += 2;
        else if (IsKeyDown(KEY_A)) player.x -= 2;

        camera.target = (Vector2){ player.x + 20, player.y + 20 };

        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

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

        ball.position.x += ball.velocity.x * dt;
        ball.position.y += ball.velocity.y * dt;
        
        BeginDrawing();

            ClearBackground((Color){135, 199, 255});
            BeginMode2D(camera);
                for (int i = 0; i<20; i++){
                    DrawRectangleRec(tlo[i], LIGHTGRAY);
                }
                DrawRectangle(-6000, 320, 13000, 8000, szary);
                DrawRectangleRec(player, MAGENTA);
            EndMode2D();
            DrawText("sztyfted!", 10, 10, 20, MAGENTA);
            DrawCircle(ball.position.x, ball.position.y, ball.radius, ORANGE);

        EndDrawing();   
    }

    CloseWindow();                  // Close window and OpenGL context

    return 0;
}
