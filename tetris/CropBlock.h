#pragma once
#include "Block.h"

// 판에는 'c' 로 저장된다 (벽 1 과 구분하기 위해)
class CropBlock : public Block {
public:
    explicit CropBlock(int shapeIndex);                   // 0~6
    std::string name() const override;
    int color() const override;
    int shapeIndex() const { return shapeIndex_; }
private:
    int shapeIndex_;
};