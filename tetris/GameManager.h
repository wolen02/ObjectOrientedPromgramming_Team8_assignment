#pragma once
#include"Board.h"
#include"Stage.h"
#include"Block.h"     // [수정] unique_ptr<Block> 를 쓰기 위해
#include<memory>

class GameManager
{
private:
	// 우리는 4단계로 나눌거니까, 각 단계의 stage 정보를 저장할 공간 마련
	Stage stage_data[4];
	// [수정] 선언 순서 = 생성자 초기화 순서 (경고 C5038 방지)
	int score;
	int level;
	int is_gameover;
	int lines;
	int ab_x;
	int ab_y;
	// 보드는 불러오면 알아서 초기화 되게 보드에서 만들기. 나는 그냥 사용만.
	Board board;
	// [수정] CropBlock -> Block. 부모 포인터로 들고 있어야 나중에 다른 블록도 그대로 쓸 수 있음 (다형성)
	std::unique_ptr<Block> curBlock;
	std::unique_ptr<Block> nextBlock;

	enum {
		BLACK,      /*  0 : 까망 */
		DARK_BLUE,    /*  1 : 어두운 파랑 */
		DARK_GREEN,    /*  2 : 어두운 초록 */
		DARK_SKY_BLUE,  /*  3 : 어두운 하늘 */
		DARK_RED,    /*  4 : 어두운 빨강 */
		DARK_VOILET,  /*  5 : 어두운 보라 */
		DARK_YELLOW,  /*  6 : 어두운 노랑 */
		GRAY,      /*  7 : 회색 */
		DARK_GRAY,    /*  8 : 어두운 회색 */
		BLUE,      /*  9 : 파랑 */
		GREEN,      /* 10 : 초록 */
		SKY_BLUE,    /* 11 : 하늘 */
		RED,      /* 12 : 빨강 */
		VOILET,      /* 13 : 보라 */
		YELLOW,      /* 14 : 노랑 */
		WHITE,      /* 15 : 하양 */
	};

	// [수정] 새로 추가한 private 함수들
	void shiftBlock(int dx, int dy);   // 블록 좌표만 옮김 (보드는 안 건드림)
	bool moveBlock(int dx, int dy);    // 지우기 -> 옮기기 -> 검사 -> 다시 놓기. 옮겼으면 true
	bool rotateBlock();                // 회전 버전
	bool landBlock();                  // 착지 처리 (줄 수확, 레벨업, 다음 블록). 게임오버면 true
	void redraw();                     // 보드 + 오른쪽 패널 다시 그리기
	void resetGame();                  // 게임오버 후 다시 시작할 때 초기화

public:
	GameManager();
	~GameManager();

	// 게임 시작시 실행화면 출력
	int init();
	int gotoxy(int x, int y);
	int show_logo();
	void SetColor(int color);

	int show_next_block(Block& b);
	// [수정] merge_block, check_full_line 삭제 (정의가 없고 Board::place / Board::checkFullLine 이 대신 함)
	// [수정] block_start 삭제 (Block 이 시작 위치를 알고 있음: START_X, START_Y)
	int show_gameover();
	int show_gamestat();
	int input_data();
	int show_cur_block(Block& b, int x, int y);
};
