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

int main() {
    Font font = GetFontDefault();

    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Puzzle Grid with Win Detection");

    SetTargetFPS(60);
    srand((unsigned)time(0));

    const int gridSize = 3;
    const int cellSize = 100;
    const int gridOffsetX = 500;
    const int gridOffsetY = 100;

    const int pieceStartX = 50;
    const int pieceStartY = 100;

    // Load image texture
    Texture2D puzzleImage = LoadTexture("puzzle1.jpg");

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

    while (!WindowShouldClose()) {
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
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Susun puzzle ke grid di kanan!", 10, 10, 20, DARKGRAY);

        if (isWin) {
            DrawText("YOU WIN!", screenWidth / 2 - 60, 50, 40, GREEN);
        }

        // Grid Board (kanan) with white gaps
        for (int row = 0; row < gridSize; row++) {
            for (int col = 0; col < gridSize; col++) {
                DrawRectangle(
                    gridOffsetX + col * cellSize,
                    gridOffsetY + row * cellSize,
                    cellSize, cellSize,
                    LIGHTGRAY
                );
            }
        }

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

        EndDrawing();
    }

    UnloadTexture(puzzleImage);
    CloseWindow();
    return 0;
}