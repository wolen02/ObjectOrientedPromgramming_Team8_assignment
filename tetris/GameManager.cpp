#define WIN32_LEAN_AND_MAEN
#define NOMINMAX
#include <windows.h>

#include "GameManager.h"
#include<iostream>
#include <ctime>     // [수정] time()
#include <cstdlib>   // [수정] srand, rand, system
#include <cstdio>    // [수정] printf
#include <iterator>  // [수정] size(stage_data)
// 콘솔에서 키보드가 눌린 상태인지 확인하는 _kbhit()함수 포함하는 헤더
#include<conio.h>
#include"Stage.h"
#include"CropBlock.h"

#define EXT_KEY			0xffffffe0	//확장키 인식값 
#define KEY_LEFT		0x4b
#define KEY_RIGHT		0x4d
#define KEY_UP			0x48
#define KEY_DOWN		0x50
using namespace std;

GameManager::GameManager()
	: score(0), level(0), is_gameover(0), lines(0), ab_x(5), ab_y(1)   // [수정] 헤더 선언 순서대로
{
	// 난수 설정
	srand((unsigned)time(NULL));

	// [수정] board = Board(); 삭제. 멤버라서 이미 생성자에서 초기화됨

	// Stage 정보 저장.  [수정] 4번째 인자 = 돌 개수 (2 + 단계)
	for (int i = 0; i < (int)size(stage_data); i++) {
		stage_data[i] = Stage((40 - 10 * i), (20 - 3 * i), 20);
	}
}

GameManager::~GameManager()
{
}

// [수정] 블록 좌표만 옮긴다
void GameManager::shiftBlock(int dx, int dy)
{
	if (dx < 0) curBlock->moveLeft();
	if (dx > 0) curBlock->moveRight();
	if (dy > 0) curBlock->moveDown();
	if (dy < 0) curBlock->moveUp();
}

// [수정] 핵심 수정: 보드에 놓인 블록을 먼저 지워야 자기 자신과 충돌하지 않는다
bool GameManager::moveBlock(int dx, int dy)
{
	board.removeBlock(*curBlock);      // 1. 지우고
	shiftBlock(dx, dy);                // 2. 옮기고
	bool ok = board.canPlace(*curBlock);   // 3. 검사
	if (!ok) shiftBlock(-dx, -dy);     //    안 되면 되돌리고
	board.place(*curBlock);            // 4. 다시 놓기
	return ok;
}

bool GameManager::rotateBlock()
{
	board.removeBlock(*curBlock);
	curBlock->rotateCW();
	bool ok = board.canPlace(*curBlock);
	if (!ok) curBlock->rotateCCW();
	board.place(*curBlock);
	return ok;
}

// [수정] 블록이 더 못 내려갈 때만 부른다. 게임오버면 true
bool GameManager::landBlock()
{
	// 화면 위에 걸친 채로 멈췄으면 게임오버
	if (curBlock->isAboveTop())
		return true;

	// 클리어한 line 확인 (착지한 뒤에만 검사해야 떨어지는 블록이 지워지지 않음)
	int cleared = board.checkFullLine();
	lines += cleared;
	score += cleared * 100;   // [수정] 점수 (원하는 규칙으로 바꾸기)

	if (stage_data[level].getClear_line() <= lines)	//클리어 스테이지
	{
		if (level < (int)size(stage_data) - 1) {   // [수정] stage_data[4] 넘어가지 않게
			level++;
			board.spawnStones(2 + level);   // [수정] 새 단계 돌. 돌 개수는 임시로 2 + 단계
		}
		lines = 0;
	}

	// 다음 블록 꺼내기
	curBlock = move(nextBlock);
	nextBlock = make_unique<CropBlock>(rand() % 7);

	// 새 블록을 놓을 자리가 없으면 게임오버
	if (!board.canPlace(*curBlock))
		return true;
	board.place(*curBlock);
	return false;
}

// [수정] cls 대신 커서를 맨 위로 옮겨서 덮어쓰기 (깜빡임 감소) + 오른쪽 패널도 다시 그림
void GameManager::redraw()
{
	gotoxy(0, 0);
	board.showBoard();
	show_next_block(*nextBlock);
	show_gamestat();
}

// [수정] 게임오버 후 다시 시작할 때 초기화
void GameManager::resetGame()
{
	board.initBoard();
	score = 0;
	lines = 0;
	is_gameover = 0;
}

int GameManager::init()
{
	// 시작하면 start 화면이 보여야함.
	show_logo();

	// 시작하면 조작키를 보여주고  stage를 입력 받음
	input_data();

	// 무한 반복 실행
	while (1)
	{
		// [수정] 매 판 시작마다 초기화 + 돌 배치
		resetGame();
		board.spawnStones(2 + level);   // [수정] 돌 개수는 임시로 2 + 단계 (Stage 에 돌 개수가 생기면 바꾸기)

		// 현재 블록과 다음에 올 블록 미리 생성하기
		curBlock = make_unique<CropBlock>(rand() % 7);
		nextBlock = make_unique<CropBlock>(rand() % 7);
		board.place(*curBlock);   // [수정] 처음 블록도 보드에 올려야 지우기/옮기기가 맞음

		system("cls");
		redraw();

		// 게임 시작
		for (int i = 1; ; i++)
		{
			bool changed = false;   // [수정] 화면을 다시 그릴지
			bool landed = false;    // [수정] 이번에 착지했는지

			// 키보드 입력이 감지되면
			if (_kbhit())
			{
				// 입력된 키 확인하고
				char keytemp = _getch();   // [수정] _getche 는 글자를 화면에 찍어서 _getch 로

				// 확장키로 읽되
				if (keytemp == (char)EXT_KEY)
				{
					keytemp = _getch();

					// 방향키면 주어진 대로 움직이고
					switch (keytemp)
					{
					case KEY_UP:		//회전하기
						changed = rotateBlock();
						break;
					case KEY_LEFT:		//왼쪽으로 이동
						changed = moveBlock(-1, 0);
						break;
					case KEY_RIGHT:		//오른쪽으로 이동
						changed = moveBlock(1, 0);
						break;
					case KEY_DOWN:		//아래로 이동
						changed = moveBlock(0, 1);
						if (!changed) landed = true;   // [수정] 못 내려가면 착지
						break;
					}
				}

				// 입력 키가 스페이스 바면 바닥까지
				if (keytemp == 32)
				{
					while (moveBlock(0, 1)) {}   // [수정] 못 내려갈 때까지
					landed = true;
				}
			}

			// 방향키를 누르지 않았을 때, 블록이 밑으로 내려옴
			if (!landed && i % stage_data[level].getSpeed() == 0)
			{
				if (moveBlock(0, 1)) changed = true;
				else landed = true;   // [수정] 못 내려가면 착지
			}

			// [수정] 착지했을 때만 줄 검사 / 다음 블록 / 게임오버 판정
			if (landed)
			{
				if (landBlock()) {
					redraw();
					show_gameover();
					SetColor(GRAY);
					break;
				}
				changed = true;
			}

			if (changed) redraw();

			gotoxy(77, 23);
			Sleep(15);
		}
	}
	return 0;
}

// 커서를 이동시키는 거임
int GameManager::gotoxy(int x, int y)
{
	// #include windows.h를 통해 표준 입출력, 표준 에러의 handle을 가져올 수 있음
	// handle은 os의 handle table의 값을 가져옴
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.Y = (SHORT)y;
	pos.X = (SHORT)x;
	SetConsoleCursorPosition(hConsole, pos);
	return 0;
}

int GameManager::show_logo()
{
	int i, j;
	gotoxy(13, 3);
	printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	Sleep(100);
	gotoxy(13, 4);
	printf("┃◆◆◆  ◆◆◆  ◆◆◆   ◆◆     ◆   ◆◆◆ ┃");
	Sleep(100);
	gotoxy(13, 5);
	printf("┃  ◆    ◆        ◆     ◆ ◆    ◆   ◆     ┃");
	Sleep(100);
	gotoxy(13, 6);
	printf("┃  ◆    ◆◆◆    ◆     ◆◆     ◆     ◆   ┃");
	Sleep(100);
	gotoxy(13, 7);
	printf("┃  ◆    ◆        ◆     ◆ ◆    ◆       ◆ ┃");
	Sleep(100);
	gotoxy(13, 8);
	printf("┃  ◆    ◆◆◆    ◆     ◆  ◆   ◆   ◆◆◆ ┃");
	Sleep(100);
	gotoxy(13, 9);
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

	gotoxy(28, 20);
	printf("Please Press Any Key~!");

	for (i = 0; i >= 0; i++) {
		if (i % 40 == 0)
		{
			for (j = 0; j < 5; j++)
			{
				gotoxy(18, 14 + j);
				printf("                                                          ");
			}
			CropBlock tempBlock = CropBlock(rand() % 7);
			show_cur_block(tempBlock, 6, 14);
			tempBlock = CropBlock(rand() % 7);
			show_cur_block(tempBlock, 12, 14);
			tempBlock = CropBlock(rand() % 7);
			show_cur_block(tempBlock, 19, 14);
			tempBlock = CropBlock(rand() % 7);
			show_cur_block(tempBlock, 24, 14);
		}
		if (_kbhit())
			break;
		Sleep(30);
	}
	// 키보드로부터 문자를 즉시 입력받는 함수(아무키나 눌렀던 걸 비우는 거구나)
	_getch();
	system("cls");

	return 0;
}

int GameManager::show_cur_block(Block& b, int x, int y)
{
	int i, j;

	// [수정] switch(1) 은 항상 파랑이었음 -> 블록이 자기 색을 알려줌 (다형성)
	SetColor(b.color());

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if ((j + y) < 0)
				continue;
			// [수정] isFilled(행, 열) 이라서 (j, i) 순서. i 가 가로, j 가 세로
			if (b.isFilled(j, i))
			{
				gotoxy((i + x) * 2 + ab_x, j + y + ab_y);
				printf("%s", b.symbol());   // [수정] 블록 종류별 문자
			}
		}
	}
	SetColor(BLACK);
	gotoxy(77, 23);
	return 0;
}

// 오른쪽 위에 다음에 올 블록 표시
int GameManager::show_next_block(Block& b)
{
	int i, j;
	SetColor((level + 1) % 6 + 1);
	for (i = 1; i < 7; i++)
	{
		gotoxy(47, i);
		for (j = 0; j < 6; j++)
		{
			if (i == 1 || i == 6 || j == 0 || j == 5)
			{
				printf("■");
			}
			else {
				printf("  ");
			}
		}
	}
	// 15->20
	show_cur_block(b, 22, 1);
	return 0;
}

int GameManager::show_gameover()
{
	SetColor(RED);
	gotoxy(15, 8);
	printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	gotoxy(15, 9);
	printf("┃**************************┃");
	gotoxy(15, 10);
	printf("┃*        GAME OVER       *┃");
	gotoxy(15, 11);
	printf("┃**************************┃");
	gotoxy(15, 12);
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
	Sleep(1000);
	while (_kbhit()) _getch();   // [수정] fflush(stdin) 은 정의되지 않은 동작 -> 남은 키 비우기

	_getch();
	system("cls");

	return 0;
}

int GameManager::show_gamestat()
{
	SetColor(GRAY);
	// [수정] static printed_text 는 0 그대로라 의미 없었음 -> 항상 출력 (cls 후에도 다시 보이게)
	gotoxy(47, 7);
	printf("STAGE");

	gotoxy(47, 9);
	printf("SCORE");

	gotoxy(47, 12);
	printf("LINES");

	gotoxy(47, 8);
	printf("%10d", level + 1);
	gotoxy(47, 10);
	printf("%10d", score);
	gotoxy(47, 13);
	printf("%10d", stage_data[level].getClear_line() - lines);
	return 0;
}

int GameManager::input_data()
{
	int i = 0;
	SetColor(GRAY);
	gotoxy(10, 7);
	printf("┏━━━━━━━━━<GAME KEY>━━━━━━━━━┓");
	Sleep(10);
	gotoxy(10, 8);
	printf("┃ UP   : Rotate Block        ┃");
	Sleep(10);
	gotoxy(10, 9);
	printf("┃ DOWN : Move One-Step Down  ┃");
	Sleep(10);
	gotoxy(10, 10);
	printf("┃ SPACE: Move Bottom Down    ┃");
	Sleep(10);
	gotoxy(10, 11);
	printf("┃ LEFT : Move Left           ┃");
	Sleep(10);
	gotoxy(10, 12);
	printf("┃ RIGHT: Move Right          ┃");
	Sleep(10);
	gotoxy(10, 13);
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");


	while (i < 1 || i > 4)   // [수정] 8 -> 4 (stage_data 가 4개라서)
	{
		gotoxy(10, 3);
		printf("Select Start level[1-4]:       \b\b\b\b\b\b\b");
		if (scanf_s("%d", &i) != 1) {   // [수정] 숫자 아닌 걸 넣으면 무한루프 방지
			i = 0;
			while (getchar() != '\n') {}
		}
	}


	level = i - 1;
	system("cls");
	return 0;
}


void GameManager::SetColor(int color)
{
	static HANDLE std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(std_output_handle, (WORD)color);
}
