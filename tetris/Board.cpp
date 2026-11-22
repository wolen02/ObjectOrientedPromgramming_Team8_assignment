#include "Board.h"
#include <iostream>
using namespace std;

Board::Board() { initBoard(); }

void Board::initBoard() {
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            total_block[y][x] = (x == 0 || x == width - 1 || y == height - 1) ? 1 : 0;
}

void Board::showBoard() const {
    cout << "\n                  TETRIS GAME\n";
    for (int y = 0; y < height; y++) {
        cout << "                  ";
        for (int x = 0; x < width; x++) {
            int cell = total_block[y][x];
            if (cell == 0) cout << "  ";
            else if (cell == 2) cout << "P ";
            else if (cell == 3) cout << "R ";
            else cout << "■";
        }
        cout << '\n';
    }
    cout << '\n';
}

bool Board::canPlaceBlock(const char shape[4][4], int x, int y) const {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            if (shape[i][j] == 0) continue;
            int xx = x + j, yy = y + i;
            if (xx < 1 || xx >= width - 1 || yy >= height - 1) return false;
            if (yy >= 0 && total_block[yy][xx] != 0) return false;
        }
    return true;
}

void Board::setBlock(const char shape[4][4], int x, int y, char value) {
    if (!canPlaceBlock(shape, x, y)) return;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (shape[i][j] != 0 && y + i >= 0)
                total_block[y + i][x + j] = value;   // 블록 종류 번호를 저장 (작물 'c', 돌 3 등)
}

int Board::checkFullLine() {
    int lines = 0;
    for (int y = height - 2; y >= 0; y--) {
        bool full = true;
        for (int x = 1; x < width - 1; x++)
            if (total_block[y][x] == 0) { full = false; break; }
        if (full) {
            for (int row = y; row > 0; row--)
                for (int x = 1; x < width - 1; x++)
                    total_block[row][x] = total_block[row - 1][x];
            for (int x = 1; x < width - 1; x++) total_block[0][x] = 0;
            lines++;
            y++;
        }
    }
    return lines;
}

int Board::getCell(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return 1;
    return total_block[y][x];
}
void Board::setCell(int x, int y, int value) {
    if (x > 0 && x < width - 1 && y >= 0 && y < height - 1 && ((value >= 0 && value <= 3) || value == 'c'))
        total_block[y][x] = (char)value;
}

// [추가] Block 과 연결하는 부분
static void blockToShape(const Block& b, char shape[4][4]) {
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            shape[r][c] = b.isFilled(r, c) ? 1 : 0;
}

bool Board::canPlace(const Block& b) const {
    char shape[4][4];
    blockToShape(b, shape);
    return canPlaceBlock(shape, b.x(), b.y());
}

void Board::place(const Block& b) {
    char shape[4][4];
    blockToShape(b, shape);
    setBlock(shape, b.x(), b.y(), b.cellValue());
}