#pragma once
#include"Board.h"
#include"Stage.h"
#include"CropBlock.h"
#include<memory>

class GameManager
{
private:
	// 우리는 4단계로 나눌거니까, 각 단계의 stage 정보를 저장할 공간 마련
	Stage stage_data[4];
	int score;
	int level;
	int is_gameover;
	int lines;
	int ab_x;
	int ab_y;
	// 보드는 불러오면 알아서 초기화 되게 보드에서 만들기. 나는 그냥 사용만.
	Board board;
	std::unique_ptr<CropBlock> curBlock;
	std::unique_ptr<CropBlock> nextBlock;

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
public:
	GameManager();
	~GameManager();
	
	// 게임 시작시 실행화면 출력
	int init(); // 변수 초기화 => 생성자로 하면 될 듯
	int gotoxy(int x, int y);
	int show_logo();
	void SetColor(int color);

	int show_next_block(Block& b);
	int merge_block(int shape, int angle, int x, int y);	//블럭이 바닥에 닿았을때 진행중인 블럭과 쌓아진 블럭을 합침
	int block_start(int shape, int* angle, int* x, int* y);	//블럭이 처음 나올때 위치와 모양을 알려줌
	int show_gameover();
	int show_gamestat();
	int input_data();
	int check_full_line();
	int show_cur_block(Block& b, int x, int y);
};

