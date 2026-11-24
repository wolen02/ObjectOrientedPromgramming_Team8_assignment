#include "StoneBlock.h"

StoneBlock::StoneBlock(int x, int y)
	:Block(singleCell(), 3)//보드에 3(돌)으로 저장
{
	setPosition(x, y);
}

Block::Rotations StoneBlock::singleCell() {//한칸짜리 
	Grid g{};
	g[0][0] = true;
	return sameForAllAngles(g);
}

std::string StoneBlock::name() const {
	return durability_ == MAX_DURABILITY ? "큰 돌" : "금 간 돌";
}

int StoneBlock::color() const {
	return durability_ == MAX_DURABILITY ? ConsoleColor::GRAY : ConsoleColor::DARK_GRAY;
}

const char* StoneBlock::symbol() const {
	return durability_==MAX_DURABILITY ? "●" : "◎";
}

bool StoneBlock::LineCompleted() {
	--durability_;
	return durability_ <=  0; //내구도 0되면 파괴
}
