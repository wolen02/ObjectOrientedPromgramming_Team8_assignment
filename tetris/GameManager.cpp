#include "GameManager.h"
#include<iostream>
#include <windows.h>
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
	:level(0), lines(0), ab_x(5), ab_y(1),is_gameover(0), score(0)
{
	// 난수 설정
	srand((unsigned)time(NULL));

	board = Board();

	// Stage 정보 저장.
	for (int i = 0; i < size(stage_data); i++) {
		stage_data[i] = Stage((40-10*i), (20 - 3 * i), 20);
	}

}

GameManager::~GameManager()
{
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
		// 비어있는 보드 보여주기
		system("cls");
		board.showBoard();

		// 현재 블록과 다음에 올 블록 미리 생성하기
		curBlock = make_unique<CropBlock>(rand()%7);
		nextBlock = make_unique<CropBlock>(rand()%7);

		// 오른쪽 위에 다음에 올 블록 표시
		show_next_block(*nextBlock);
		
		
		// 현재 스테이지 정보, 점수, 클리어까지 남은 라인의 수를 콘솔화면에 줄력
		show_gamestat();


		// 게임 시작
		int i;
		for (i = 1;1;i++)
		{
			// 키보드 입력이 감지되면
			if (_kbhit())
			{
				// 입력된 키 확인하고
				char keytemp = _getche();

				// 확장키로 읽되
				if (keytemp == EXT_KEY)
				{
					keytemp = _getche();
					
					// 방향키면 주어진 대로 움직이고
					switch (keytemp)
					{
					case KEY_UP:		//회전하기
						
						curBlock->rotateCW();
						if (board.canPlace(*curBlock))
						{
							// 이전 상태 지우고 다시 회전
							curBlock->rotateCCW();
							board.removeBlock(*curBlock);
							curBlock->rotateCW();
							board.place(*curBlock);
							system("cls");
							board.showBoard();

						}
						break;
					case KEY_LEFT:		//왼쪽으로 이동
						curBlock->moveLeft();
						if (board.canPlace(*curBlock))
						{
							// 이전 상태 지우고 다시 왼쪽으로 이동
							curBlock->moveRight();
							board.removeBlock(*curBlock);
							curBlock->moveLeft();
							board.place(*curBlock);
							system("cls");
							board.showBoard();

						}
						break;
					case KEY_RIGHT:		//오른쪽으로 이동
						curBlock->moveRight();
						if (board.canPlace(*curBlock))
						{
							// 이전 상태 지우고 다시 왼쪽으로 이동
							curBlock->moveLeft();
							board.removeBlock(*curBlock);
							curBlock->moveRight();
							board.place(*curBlock);
							system("cls");
							board.showBoard();

						}
						break;
					case KEY_DOWN:		//아래로 이동
						curBlock->moveDown();
						if (board.canPlace(*curBlock))
						{
							// 이전 상태 지우고 다시 왼쪽으로 이동
							curBlock->moveUp();
							board.removeBlock(*curBlock);
							curBlock->moveDown();
							board.place(*curBlock);
							system("cls");
							board.showBoard();

						}
						break;
					}
				}
				
				// 입력 키가 스페이스 바면
				if (keytemp == 32)
				{
					while (board.canPlace(*curBlock))
					{
						curBlock->moveDown();
						if (board.canPlace(*curBlock))
						{
							// 이전 상태 지우고 다시 아래로 이동
							curBlock->moveUp();
							board.removeBlock(*curBlock);
							curBlock->moveDown();
							board.place(*curBlock);
						}
						else {
							curBlock->moveUp();
							break;
						}
					}

					system("cls");
					board.showBoard();
				}
			}

			// 방향키를 누르지 않았을 때, 블록이 밑으로 내려옴
			if (i % stage_data[level].getSpeed() == 0)
			{
				curBlock->moveDown();
				if (board.canPlace(*curBlock))
				{
					// 이전 상태 지우고 다시 아래로 이동
					curBlock->moveUp();
					board.removeBlock(*curBlock);
					curBlock->moveDown();
					board.place(*curBlock);
					system("cls");
					board.showBoard();
				}

			}

			// 클리어한 line 확인
			lines += board.checkFullLine();

			if (stage_data[level].getClear_line() == lines)	//클리어 스테이지
			{
				level++;
				lines = 0;
			}

			// 블록이 맞닿았음을 확인하고 닿았으면 블록이 윗칸을 넘는지 확인하여 게임 종료여부 확인
			curBlock->moveDown();
			if (board.canPlace(*curBlock)) {
				curBlock->moveUp();
			}
			else {
				curBlock->moveUp();

				if (curBlock->isAboveTop()) {
					// 게임오버
					show_gameover();
					SetColor(GRAY);
					break;
				}
				else {
					curBlock = move(nextBlock);
					nextBlock = make_unique<CropBlock>(rand()%7);
				}
			}
			
			gotoxy(77, 23);
			Sleep(15);
			gotoxy(77, 23);
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
	pos.Y = y;
	pos.X = x;
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

	for (i = 0;i >= 0;i++) {
		if (i % 40 == 0)
		{
			for (j = 0;j < 5;j++)
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
	_getche();
	system("cls");

	return 0;
}

int GameManager::show_cur_block(Block& b, int x, int y)
{
	int i, j;

	switch (1)
	{
	case 0:
		SetColor(RED);
		break;
	case 1:
		SetColor(BLUE);
		break;
	case 2:
		SetColor(SKY_BLUE);
		break;
	case 3:
		SetColor(WHITE);
		break;
	case 4:
		SetColor(YELLOW);
		break;
	case 5:
		SetColor(VOILET);
		break;
	case 6:
		SetColor(GREEN);
		break;
	}

	for (i = 0;i < 4;i++)
	{
		for (j = 0;j < 4;j++)
		{
			if ((j + y) < 0)
				continue;
			if (b.isFilled(i,j))
			{
				gotoxy((i + x) * 2 + ab_x, j + y + ab_y);
				printf("■");
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
	for (i = 1;i < 7;i++)
	{
		gotoxy(47, i);
		for (j = 0;j < 6;j++)
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


int GameManager::block_start(int shape, int* angle, int* x, int* y)
{
	*x = 5;
	*y = -3;
	*angle = 0;
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
	fflush(stdin);
	Sleep(1000);

	_getche();
	system("cls");

	return 0;
}

int GameManager::show_gamestat()
{
	static int printed_text = 0;
	SetColor(GRAY);
	if (printed_text == 0)
	{
		gotoxy(47, 7);
		printf("STAGE");

		gotoxy(47, 9);
		printf("SCORE");

		gotoxy(47, 12);
		printf("LINES");

	}
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


	while (i < 1 || i>8)
	{
		gotoxy(10, 3);
		printf("Select Start level[1-4]:       \b\b\b\b\b\b\b");
		scanf_s("%d", &i);
	}


	level = i - 1;
	system("cls");
	return 0;
}


void GameManager::SetColor(int color)
{
	static HANDLE std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(std_output_handle, color);
}
