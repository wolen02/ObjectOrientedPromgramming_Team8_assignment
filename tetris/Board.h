#pragma once
#include <memory> 
#include "Block.h"   // [추가] Block 을 직접 받기 위해
using namespace std;

class Board {
private:
    static const int height = 21;
    static const int width = 14;
    char total_block[height][width];   // 0 빈칸, 1 벽, 2 해충(P), 3 돌(R), 'c' 작물
  
    unique_ptr<Block> stuck[height][width];//돌의 내구도를 기억하기 위함
    void shiftColumnDown(int row, int x);   // [수정] x 열만 row 위 칸들을 한 칸 내린다 (돌 위는 안 내려감)
    void shiftDownFrom(int row);
public:
    Board();
    void initBoard();  // 보드를 처음 상태로 만들어
    void showBoard() const;  // 현재 보드를 화면에 보여준다.


    int checkFullLine();  // 가득 찬 줄을 지우고, 지운 줄의 수를 돌려준다[돌 수정]
    int getCell(int x, int y) const;  // 해당 위치의 값을 가져온다.
    void setCell(int x, int y, int value);  // 해당 위치의 값을 바꿔요[돌 수정]

    // 블록 객체를 직접 사용.
    bool canPlace(const Block& b) const; // 현재 위치에 블록을 놓을 수 있으면 true, 충돌하면 false를 반환한다.
    // [변경] - int-로
    int place(const Block& b); // 바닥에 도착한 블록을 보드에 저장한다.
    //[추가] 기존 블록을 보드에서 삭제한다.
    int removeBlock(const Block& b);//[돌 수정]

    //[돌 추가]
    int spawnStones(int count);
    const Block* stuckBlock(int x, int y)const;
};
















//bool canPlaceBlock(const char shape[4][4], int x, int y) const;  // 블록을 이 위치에 놓을 수 있는지 확인한다
  //void setBlock(const char shape[4][4], int x, int y, char value = 'c');  // 블록을 보드에 저장해요