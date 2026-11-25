#include "Board.h"
#include "StoneBlock.h"
#include <vector>
#include <cstdlib>
#include <iostream>
using namespace std;

Board::Board() { initBoard(); }

void Board::initBoard() {
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++){
            total_block[y][x] = (x == 0 || x == width - 1 || y == height - 1) ? 1 : 0;
            stuck[y][x].reset();//[돌]
        }
}

void Board::showBoard() const {
    cout << "\n                  TETRIS GAME\n";
    for (int y = 0; y < height; y++) {
        cout << "                  ";
        for (int x = 0; x < width; x++) {
            int cell = total_block[y][x];
            if (cell == 0) cout << "  ";
            else if (stuck[y][x])cout << stuck[y][x]->symbol();//[돌]
            else if (cell == 2) cout << "P ";
            else if (cell == 3) cout << "R ";
            else cout << "■";
        }
        cout << '\n';
    }
    cout << '\n';
}

//[돌 변경]돌이 버티면 작물만 수확, 다 깨지면 원래처럼 줄 삭제.
int Board::checkFullLine() {
    int lines = 0;
    for (int y = height - 2; y >= 0; y--) {
        bool full = true;
        for (int x = 1; x < width - 1; x++)
            if (total_block[y][x] == 0) { full = false; break; }
        if (!full) continue;

        lines++;
        bool stoneSurvived = false;
        for (int x = 1; x < width - 1; x++) {
            if (!stuck[y][x]) continue;
            if (stuck[y][x]->LineCompleted()) {   // 깨짐
                stuck[y][x].reset();
                total_block[y][x] = 0;
            }
            else stoneSurvived = true;              // 금만 감
        }

        if (stoneSurvived) {
            // [수정] 작물을 수확한 칸에는 위 블록들이 내려온다. 돌이 있는 열만 그대로
            for (int x = 1; x < width - 1; x++)
                if (!stuck[y][x]) shiftColumnDown(y, x);
            y++;   // [수정] 내려온 줄을 다시 검사
        }
        else {
            shiftDownFrom(y);
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
    if (x > 0 && x < width - 1 && y >= 0 && y < height - 1 && ((value >= 0 && value <= 3) || value == 'c')) {
       
        if (value == 3)stuck[y][x] = make_unique<StoneBlock>(x, y);//[돌 추가] 
        else stuck[y][x].reset();
        total_block[y][x] = (char)value;
    }
}

//변경 - 블록을 놓을 수 있는지 확인
bool Board::canPlace(const Block& b) const {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {

            if (!b.isFilled(i, j))
                continue;

            int x = b.x() + j;
            int y = b.y() + i;

            if (x < 1 || x >= width - 1 || y >= height - 1)
                return false;

            if (y >= 0 && total_block[y][x] != 0)
                return false;
        }
    }
    return true;
}

// [변경] 블록을 보드에 저장
int Board::place(const Block& b) { // 변경

    if (!canPlace(b)) return -1; // 변경

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int x = b.x() + j;
            int y = b.y() + i;

            if (b.isFilled(i, j) && y >= 0) {
                total_block[y][x] = b.cellValue();
            }
        }
    }

    return 0; // 추가
}
// [추가] 기존 블록을 보드에서 삭제
int Board::removeBlock(const Block& b) {

    // 블록을 삭제할 수 있는지 확인
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {

            if (!b.isFilled(i, j))
                continue;

            int x = b.x() + j;
            int y = b.y() + i;

            if (x < 1 || x >= width - 1 || y >= height - 1)
                return -1;

            if (y >= 0 && total_block[y][x] != b.cellValue())
                return -1;
        }
    }

    // 기존 블록 삭제
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {

            int x = b.x() + j;
            int y = b.y() + i;

            if (b.isFilled(i, j) && y >= 0) {
                total_block[y][x] = 0;
            }
        }
    }

    return 0;
}

void Board::shiftColumnDown(int row, int x) {   // [수정] 한 열만 내리는 부분을 따로 뺐다
    int stoneRow = -1;                         // 이 열에서 row 위쪽의 가장 가까운 돌 (없으면 -1)
    for (int r = row - 1; r >= 0; r--)
        if (stuck[r][x]) { stoneRow = r; break; }

    for (int r = row; r > stoneRow + 1; r--)   // 돌 아래 칸들만 한 칸씩 내린다
        total_block[r][x] = total_block[r - 1][x];
    total_block[stoneRow + 1][x] = 0;          // 돌 바로 아래(돌이 없으면 맨 윗줄)는 빈칸
}

void Board::shiftDownFrom(int row) {
    for (int x = 1; x < width - 1; x++)
        shiftColumnDown(row, x);
}

int Board::spawnStones(int count) {//보드 절반 아래에 count개수만큼 랜덤으로 돌 생성 나중에 스테이지에서 count값 설정해서 조절
    vector<pair<int, int>> empty;
    for (int y = height / 2; y < height - 1; y++)
        for (int x = 1; x < width - 1; x++)
            if (total_block[y][x] == 0) empty.push_back({ x, y });

    int placed = 0;
    while (placed < count && !empty.empty()) {
        int k = rand() % (int)empty.size();
        int x = empty[k].first, y = empty[k].second;
        empty[k] = empty.back();
        empty.pop_back();

        stuck[y][x] = std::make_unique<StoneBlock>(x, y);
        total_block[y][x] = stuck[y][x]->cellValue();   // 3
        placed++;
    }
    return placed;
}

const Block* Board::stuckBlock(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return nullptr;
    return stuck[y][x].get();
}

//bool Board::canPlaceBlock(const char shape[4][4], int x, int y) const {
//    for (int i = 0; i < 4; i++)
//        for (int j = 0; j < 4; j++) {
//            if (shape[i][j] == 0) continue;
//            int xx = x + j, yy = y + i;
//            if (xx < 1 || xx >= width - 1 || yy >= height - 1) return false;
//            if (yy >= 0 && total_block[yy][xx] != 0) return false;
//        }
//    return true;
//}

//void Board::setBlock(const char shape[4][4], int x, int y, char value) {
//    if (!canPlaceBlock(shape, x, y)) return;
//    for (int i = 0; i < 4; i++)
//        for (int j = 0; j < 4; j++)
//            if (shape[i][j] != 0 && y + i >= 0)
//                total_block[y + i][x + j] = value;   // 블록 종류 번호를 저장 (작물 'c', 돌 3 등)
//}
//// [추가] Block 과 연결하는 부분
//static void blockToShape(const Block& b, char shape[4][4]) {
//    for (int r = 0; r < 4; r++)
//        for (int c = 0; c < 4; c++)
//            shape[r][c] = b.isFilled(r, c) ? 1 : 0;
//}

//bool Board::canPlace(const Block& b) const {
//    char shape[4][4];
//    blockToShape(b, shape);
//    return canPlaceBlock(shape, b.x(), b.y());
//}

//void Board::place(const Block& b) {
//    char shape[4][4];
//    blockToShape(b, shape);
//    setBlock(shape, b.x(), b.y(), b.cellValue());
//}