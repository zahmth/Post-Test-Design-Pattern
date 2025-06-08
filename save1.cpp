#include "raylib.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm> // for std::shuffle

struct PuzzlePiece {
    Rectangle rect;       // Destination on screen
    Rectangle sourceRect; // Source from the image
    Vector2 offset;
    bool isDragging;
    int targetRow, targetCol; // Correct answer position
};

class PuzzleFactory {
public:
    static PuzzlePiece Create(int row, int col, int cellSize, int startX, int startY) {
        PuzzlePiece piece;
        piece.rect = {
            (float)(startX + col * (cellSize + 10)),
            (float)(startY + row * (cellSize + 10)),
            (float)cellSize,
            (float)cellSize
        };
        piece.sourceRect = { 0 }; // Will set later based on image
        piece.offset = {0, 0};
        piece.isDragging = false;
        piece.targetRow = row;
        piece.targetCol = col;
        return piece;
    }
};

bool CheckWin(const std::vector<PuzzlePiece>& pieces, int gridSize, int gridOffsetX, int gridOffsetY, int cellSize) {
    for (const auto& piece : pieces) {
        int col = (piece.rect.x - gridOffsetX + cellSize / 2) / cellSize;
        int row = (piece.rect.y - gridOffsetY + cellSize / 2) / cellSize;

        if (col != piece.targetCol || row != piece.targetRow) {
            return false;
        }
    }
    return true;
}

enum GameState {
    MENU,
    GAME
};

int main() {
    Font font = GetFontDefault();

    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Puzzle Grid with Win Detection");

    SetTargetFPS(60);
    srand((unsigned)time(0));

    const int gridSize = 3;
    const int cellSize = 100;
    const int gridOffsetX = 450;
    const int gridOffsetY = 100;

    const int pieceStartX = 50;
    const int pieceStartY = 100;

    // Load image texture
    Texture2D puzzleImage = LoadTexture("puzzle.png");
    Texture2D boardImage = LoadTexture("board.png");

    // Calculate each piece's source rect
    int imageCellSizeX = puzzleImage.width / gridSize;
    int imageCellSizeY = puzzleImage.height / gridSize;

    // Create all pieces
    std::vector<PuzzlePiece> pieces;
    for (int row = 0; row < gridSize; row++) {
        for (int col = 0; col < gridSize; col++) {
            PuzzlePiece piece = PuzzleFactory::Create(row, col, cellSize, pieceStartX, pieceStartY);
            piece.sourceRect = {
                (float)(col * imageCellSizeX),
                (float)(row * imageCellSizeY),
                (float)imageCellSizeX,
                (float)imageCellSizeY
            };
            pieces.push_back(piece);
        }
    }

    // Randomize the initial positions of the puzzle pieces on the left
    std::random_shuffle(pieces.begin(), pieces.end());

    // Adjust positions after shuffle
    for (int i = 0; i < pieces.size(); i++) {
        int row = i / gridSize;
        int col = i % gridSize;
        pieces[i].rect.x = pieceStartX + col * (cellSize + 10);
        pieces[i].rect.y = pieceStartY + row * (cellSize + 10);
    }

    PuzzlePiece* draggingPiece = nullptr;
    bool isWin = false;

    GameState currentState = MENU;
    Rectangle startButton = { screenWidth/2 - 100, screenHeight/2 - 30, 200, 50 };
    Rectangle exitButton = { screenWidth/2 - 100, screenHeight/2 + 40, 200, 50 };

while (!WindowShouldClose()) {
    Vector2 mouse = GetMousePosition();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (currentState == MENU) {
        DrawText("PUZZLE GAME", screenWidth/2 - MeasureText("PUZZLE GAME", 40)/2, 100, 40, DARKGRAY);

        // Warna hover
        Color startColor = CheckCollisionPointRec(mouse, startButton) ?  SKYBLUE : BLUE;
        Color exitColor = CheckCollisionPointRec(mouse, exitButton) ? PINK : RED;

        DrawRectangleRec(startButton, startColor);
        DrawText("START GAME", startButton.x + 30, startButton.y + 15, 20, WHITE);

        DrawRectangleRec(exitButton, exitColor);
        DrawText("EXIT", exitButton.x + 80, exitButton.y + 15, 20, WHITE);


        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, startButton)) {
                currentState = GAME;
            }
            if (CheckCollisionPointRec(mouse, exitButton)) {
                break;
            }
        }

    } else if (currentState == GAME) {
        Vector2 mouse = GetMousePosition();

        // Handle Drag Start
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (auto& piece : pieces) {
                if (CheckCollisionPointRec(mouse, piece.rect)) {
                    piece.isDragging = true;
                    piece.offset = { mouse.x - piece.rect.x, mouse.y - piece.rect.y };
                    draggingPiece = &piece;
                    break;
                }
            }
        }

        // Handle Drag Move
        if (draggingPiece && draggingPiece->isDragging) {
            draggingPiece->rect.x = mouse.x - draggingPiece->offset.x;
            draggingPiece->rect.y = mouse.y - draggingPiece->offset.y;
        }

        // Handle Drag End
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && draggingPiece) {
            int col = (draggingPiece->rect.x - gridOffsetX + cellSize / 2) / cellSize;
            int row = (draggingPiece->rect.y - gridOffsetY + cellSize / 2) / cellSize;

            // Snap into grid if within bounds
            if (row >= 0 && row < gridSize && col >= 0 && col < gridSize) {
                draggingPiece->rect.x = gridOffsetX + col * cellSize;
                draggingPiece->rect.y = gridOffsetY + row * cellSize;
            }

            draggingPiece->isDragging = false;
            draggingPiece = nullptr;
        }

        // Win detection
        isWin = CheckWin(pieces, gridSize, gridOffsetX, gridOffsetY, cellSize);

        // Drawing
        ClearBackground(RAYWHITE);

        DrawText("Susun puzzle ke grid di kanan!", 80, 20, 40, DARKGRAY);

        if (isWin) {
            DrawText("YOU WIN!", 100, 220, 40, GREEN);
        }

        // Gambar papan grid sebagai satu gambar
        DrawTexture(boardImage, gridOffsetX, gridOffsetY, WHITE);


        // Draw pieces (no borders)
        for (auto& piece : pieces) {
            DrawTexturePro(
                puzzleImage,
                piece.sourceRect,
                piece.rect,
                {0,0},
                0.0f,
                WHITE
            );
        }

    }

    EndDrawing();
}

    UnloadTexture(puzzleImage);
    UnloadTexture(boardImage);
    CloseWindow();
    return 0;
}