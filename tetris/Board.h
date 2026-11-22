#pragma once
#include "Block.h"   // [추가] Block 을 직접 받기 위해

class Board {
private:
    static const int height = 21;
    static const int width = 14;
    char total_block[height][width];   // 0 빈칸, 1 벽, 2 해충(P), 3 돌(R), 'c' 작물
public:
    Board();
    void initBoard();  // 보드를 처음 상태로 만들어
    void showBoard() const;  // 현재 보드를 화면에 보여준다.
    bool canPlaceBlock(const char shape[4][4], int x, int y) const;  // 블록을 이 위치에 놓을 수 있는지 확인한다
    void setBlock(const char shape[4][4], int x, int y, char value = 'c');  // 블록을 보드에 저장해요
    int checkFullLine();  // 가득 찬 줄을 지우고, 지운 줄의 수를 돌려준다
    int getCell(int x, int y) const;  // 해당 위치의 값을 가져온다.
    void setCell(int x, int y, int value);  // 해당 위치의 값을 바꿔요

    // [추가] 
    bool canPlace(const Block& b) const;
    void place(const Block& b);
};