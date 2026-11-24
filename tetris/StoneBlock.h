#pragma once
#include "Block.h"
#include <string>
using namespace std;

class StoneBlock : public Block
{
public:
	static constexpr int MAX_DURABILITY = 2;//내구도
	StoneBlock(int x, int y);//보드에 놓이는 위치

	std::string name() const override;
	int color() const override;
	const char* symbol() const override;
	bool LineCompleted() override;//내구도 1 감소, 0되면 깨짐

	int durability() const{return durability_;}

private:
	int durability_ = MAX_DURABILITY;
	static Rotations singleCell();//[4][4]에서 왼쪽 위 한칸
	
};

