#include "raylib.h"
#include "BestTimeManager.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <cstdio>

struct PuzzlePiece {
    Rectangle rect;
    Rectangle sourceRect;
    Vector2 offset;
    bool isDragging;
    int targetRow, targetCol;
    float rotation = 0.0f;
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

        if (col != piece.targetCol || row != piece.targetRow || ((int)piece.rotation % 360) != 0) {
            return false;
        }
    }
    return true;
}

enum GameState {
    MENU,
    PUZZLE1,
    PUZZLE2,
    WIN
};

int main() {
    InitWindow(1000, 750, "Puzzle Piece");
    SetTargetFPS(60);
    srand((unsigned)time(0));

    Font font = GetFontDefault();
    Texture2D puzzle1Image = LoadTexture("puzzle.png");
    Texture2D puzzle2Image = LoadTexture("puzzle2.png");
    Texture2D boardImage3x3 = LoadTexture("board.png");
    Texture2D boardImage4x4 = LoadTexture("board4x4.png");

    const int cellSize = 100;
    const int pieceStartX = 50;
    const int pieceStartY = 100;
    const int gridOffsetX = 600;
    const int gridOffsetY = 100;

    int currentGridSize = 3;
    Texture2D* currentImage = &puzzle1Image;
    Texture2D* currentBoardImage = &boardImage3x3;

    std::vector<PuzzlePiece> pieces;
    auto resetPuzzle = [&](int gridSize) {
        pieces.clear();
        int imageCellSizeX = currentImage->width / gridSize;
        int imageCellSizeY = currentImage->height / gridSize;

        for (int row = 0; row < gridSize; row++) {
            for (int col = 0; col < gridSize; col++) {
                PuzzlePiece piece = PuzzleFactory::Create(row, col, cellSize, pieceStartX, pieceStartY);
                piece.sourceRect = {
                    (float)(col * imageCellSizeX),
                    (float)(row * imageCellSizeY),
                    (float)imageCellSizeX,
                    (float)imageCellSizeY
                };
                piece.rotation = 0.0f;
                pieces.push_back(piece);
            }
        }

        std::random_shuffle(pieces.begin(), pieces.end());
        for (int i = 0; i < pieces.size(); i++) {
            int row = i / gridSize;
            int col = i % gridSize;
            pieces[i].rect.x = pieceStartX + col * (cellSize + 10);
            pieces[i].rect.y = pieceStartY + row * (cellSize + 10);
        }
    };

    GameState currentState = MENU;
    PuzzlePiece* draggingPiece = nullptr;

    Rectangle startButton = { 400, 300, 200, 50 };
    Rectangle exitButton = { 400, 370, 200, 50 };
    Rectangle retryButton = { 300, 300, 200, 50 };
    Rectangle exitButtonWin = { 300, 370, 200, 50 };

    bool isWin = false;
    float gameTimer = 0.0f;
    bool timerRunning = false;

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentState == MENU) {
            DrawText("PUZZLE GAME", 350, 100, 40, DARKGRAY);
            DrawRectangleRec(startButton, CheckCollisionPointRec(mouse, startButton) ? SKYBLUE : BLUE);
            DrawText("START GAME", 430, 315, 20, WHITE);
            DrawRectangleRec(exitButton, CheckCollisionPointRec(mouse, exitButton) ? PINK : RED);
            DrawText("EXIT", 470, 385, 20, WHITE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, startButton)) {
                    currentState = PUZZLE1;
                    currentGridSize = 3;
                    currentImage = &puzzle1Image;
                    currentBoardImage = &boardImage3x3;
                    resetPuzzle(currentGridSize);
                    gameTimer = 0.0f;
                    timerRunning = true;
                    isWin = false;
                } else if (CheckCollisionPointRec(mouse, exitButton)) {
                    break;
                }
            }

        } else if (currentState == PUZZLE1 || currentState == PUZZLE2 || currentState == WIN) {
            if (timerRunning && !isWin) gameTimer += GetFrameTime();

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

            if (draggingPiece && draggingPiece->isDragging) {
                draggingPiece->rect.x = mouse.x - draggingPiece->offset.x;
                draggingPiece->rect.y = mouse.y - draggingPiece->offset.y;

                if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                    draggingPiece->rotation += 90;
                    if (draggingPiece->rotation >= 360) draggingPiece->rotation = 0;
                }
            }

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && draggingPiece) {
                int col = (draggingPiece->rect.x - gridOffsetX + cellSize / 2) / cellSize;
                int row = (draggingPiece->rect.y - gridOffsetY + cellSize / 2) / cellSize;

                if (row >= 0 && row < currentGridSize && col >= 0 && col < currentGridSize) {
                    draggingPiece->rect.x = gridOffsetX + col * cellSize;
                    draggingPiece->rect.y = gridOffsetY + row * cellSize;
                }

                draggingPiece->isDragging = false;
                draggingPiece = nullptr;
            }

            DrawText("Susun puzzle ke puzzle board!", 80, 20, 40, DARKGRAY);
            DrawTexture(*currentBoardImage, gridOffsetX, gridOffsetY, WHITE);

            for (auto& piece : pieces) {
                DrawTexturePro(
                    *currentImage,
                    piece.sourceRect,
                    piece.rect,
                    {0, 0},
                    piece.rotation,
                    WHITE
                );
            }

            isWin = CheckWin(pieces, currentGridSize, gridOffsetX, gridOffsetY, cellSize);
            if (isWin) {
                if (currentState == PUZZLE1) {
                    currentState = PUZZLE2;
                    currentGridSize = 4;
                    currentImage = &puzzle2Image;
                    currentBoardImage = &boardImage4x4;
                    resetPuzzle(currentGridSize);
                    isWin = false;
                } else if (currentState == PUZZLE2) {
                    currentState = WIN;
                    timerRunning = false;
                    BestTimeManager::GetInstance().UpdateBestTime(gameTimer);
                }
            }

            char timerText[64];
            snprintf(timerText, sizeof(timerText), "Time: %.2f", gameTimer);
            DrawText(timerText, 20, 700, 20, DARKGRAY);

            float best = BestTimeManager::GetInstance().GetBestTime();
            if (best >= 0) {
                char bestText[64];
                snprintf(bestText, sizeof(bestText), "Best: %.2f", best);
                DrawText(bestText, 20, 730, 20, DARKGRAY);
            }

            if (currentState == WIN) {
                DrawText("YOU WIN!", 280, 200, 40, GREEN);
                DrawRectangleRec(retryButton, CheckCollisionPointRec(mouse, retryButton) ? ORANGE : GOLD);
                DrawText("RETRY", 370, 315, 20, WHITE);
                DrawRectangleRec(exitButtonWin, CheckCollisionPointRec(mouse, exitButtonWin) ? PINK : RED);
                DrawText("EXIT", 380, 385, 20, WHITE);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (CheckCollisionPointRec(mouse, retryButton)) {
                        currentState = PUZZLE1;
                        currentGridSize = 3;
                        currentImage = &puzzle1Image;
                        currentBoardImage = &boardImage3x3;
                        resetPuzzle(currentGridSize);
                        gameTimer = 0.0f;
                        timerRunning = true;
                        isWin = false;
                    } else if (CheckCollisionPointRec(mouse, exitButtonWin)) {
                        CloseWindow();
                        return 0;
                    }
                }
            }
        }

        EndDrawing();
    }

    UnloadTexture(puzzle1Image);
    UnloadTexture(puzzle2Image);
    UnloadTexture(boardImage3x3);
    UnloadTexture(boardImage4x4);
    CloseWindow();
    return 0;
}
