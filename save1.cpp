#include "raylib.h"
#include <vector>
#include <ctime>
#include <cstdlib>

struct PuzzlePiece {
    Rectangle rect;
    Vector2 offset;
    bool isDragging;
    Color color;
    int targetRow, targetCol;
    bool isPlaced = false;
};

class PuzzleFactory {
public:
    static PuzzlePiece Create(int row, int col, int cellSize, int startX, int startY) {
        PuzzlePiece piece;
        piece.rect = {
            (float)(startX + col * (cellSize + 10)), // rapikan ke samping
            (float)(startY + row * (cellSize + 10)), // rapikan ke bawah
            (float)cellSize,
            (float)cellSize
        };
        piece.offset = {0, 0};
        piece.isDragging = false;
        piece.color = Color{ 
            (unsigned char)GetRandomValue(100, 255), 
            (unsigned char)GetRandomValue(100, 255), 
            (unsigned char)GetRandomValue(100, 255), 
            255 
        };
        piece.targetRow = row;
        piece.targetCol = col;
        return piece;
    }
};

int main() {
    Font font = GetFontDefault();  // gunakan font default

    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Puzzle Grid with Drag and Drop");

    SetTargetFPS(60);
    srand((unsigned)time(0));

    const int gridSize = 3;
    const int cellSize = 100;
    const int gridOffsetX = 500;
    const int gridOffsetY = 100;

    const int pieceStartX = 50;
    const int pieceStartY = 100;

    std::vector<PuzzlePiece> pieces;
    for (int row = 0; row < gridSize; row++) {
        for (int col = 0; col < gridSize; col++) {
            pieces.push_back(PuzzleFactory::Create(row, col, cellSize, pieceStartX, pieceStartY));
        }
    }

    PuzzlePiece* draggingPiece = nullptr;

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        // Handle Drag Start
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (auto& piece : pieces) {
                if (!piece.isPlaced && CheckCollisionPointRec(mouse, piece.rect)) {
                    piece.isDragging = true;
                    piece.offset = { mouse.x - piece.rect.x, mouse.y - piece.rect.y };
                    draggingPiece = &piece;
                    break;
                }
            }
        }                

         // Handle Drag Move
         if (draggingPiece && draggingPiece->isDragging && !draggingPiece->isPlaced) {
            draggingPiece->rect.x = mouse.x - draggingPiece->offset.x;
            draggingPiece->rect.y = mouse.y - draggingPiece->offset.y;
        }        

        // Handle Drag End

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && draggingPiece) {
            // Cek posisi sekarang
            int col = (draggingPiece->rect.x - gridOffsetX + cellSize / 2) / cellSize;
            int row = (draggingPiece->rect.y - gridOffsetY + cellSize / 2) / cellSize;
        
            if (row == draggingPiece->targetRow && col == draggingPiece->targetCol) {
                draggingPiece->rect.x = gridOffsetX + col * cellSize;
                draggingPiece->rect.y = gridOffsetY + row * cellSize;
                draggingPiece->isPlaced = true;
            }
        
            // Hentikan dragging apapun kondisinya
            draggingPiece->isDragging = false;
            draggingPiece = nullptr;
        }            

        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Susun puzzle ke grid di kanan!", 10, 10, 20, DARKGRAY);

        // Grid Board
        for (int row = 0; row < gridSize; row++) {
            for (int col = 0; col < gridSize; col++) {
                DrawRectangleLines(
                    gridOffsetX + col * cellSize,
                    gridOffsetY + row * cellSize,
                    cellSize, cellSize,
                    LIGHTGRAY
                );
            }
        }

        for (int row = 0; row < gridSize; row++) {
            for (int col = 0; col < gridSize; col++) {
                int cellIndex = row * gridSize + col;
                int x = gridOffsetX + col * cellSize + 10;
                int y = gridOffsetY + row * cellSize + 10;
                DrawTextEx(font, TextFormat("%d", cellIndex), { (float)x, (float)y }, 20, 1, GRAY);
            }
        }        

        // Pieces
        for (auto& piece : pieces) {
            DrawRectangleRec(piece.rect, piece.color);
            Vector2 textPos = { piece.rect.x + 5, piece.rect.y + 5 };
             int id = piece.targetRow * gridSize + piece.targetCol;
             DrawTextEx(font, TextFormat("%d", id), textPos, 20, 1, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}