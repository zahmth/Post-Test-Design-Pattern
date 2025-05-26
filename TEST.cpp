#include "raylib.h"

int main() {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Simple Raylib Example");

    Vector2 circlePos = { (float)screenWidth / 2, (float)screenHeight / 2 };
    float circleRadius = 30.0f;
    float speed = 200.0f;

    SetTargetFPS(60);  // Set the game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        float deltaTime = GetFrameTime();

        if (IsKeyDown(KEY_RIGHT)) circlePos.x += speed * deltaTime;
        if (IsKeyDown(KEY_LEFT))  circlePos.x -= speed * deltaTime;
        if (IsKeyDown(KEY_DOWN))  circlePos.y += speed * deltaTime;
        if (IsKeyDown(KEY_UP))    circlePos.y -= speed * deltaTime;

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Move the circle with arrow keys!", 10, 10, 20, DARKGRAY);
        DrawCircleV(circlePos, circleRadius, MAROON);

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();  // Close window and OpenGL context

    return 0;
}
