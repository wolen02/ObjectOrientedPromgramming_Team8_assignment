#pragma once
#include <array>
#include <string>

namespace ConsoleColor {
    constexpr int DARK_GRAY = 8, BLUE = 9, GREEN = 10, SKY_BLUE = 11,
        RED = 12, VIOLET = 13, YELLOW = 14, WHITE = 15,
        DARK_YELLOW = 6, GRAY = 7, DARK_GREEN = 2;
}

class Block {
public:
    static constexpr int SIZE = 4;                        // 4x4 격자
    using Grid = std::array<std::array<bool, SIZE>, SIZE>;   // [행][열]
    using Rotations = std::array<Grid, 4>;                // 회전 4방향

    static constexpr int START_X = 5;                     // 원본 block_start 값
    static constexpr int START_Y = -3;

    virtual ~Block() = default;


    bool isFilled(int row, int col) const { return rotations_[angle_][row][col]; }

    int x() const { return x_; }
    int y() const { return y_; }
    int angle() const { return angle_; }

    void moveLeft() { --x_; }
    void moveRight() { ++x_; }
    void moveDown() { ++y_; }
    void moveUp() { --y_; }
    void rotateCW() { angle_ = (angle_ + 1) % 4; }
    void rotateCCW() { angle_ = (angle_ + 3) % 4; }
    void resetPosition();


    bool isAboveTop() const;


    virtual std::string name() const = 0;
    virtual int color() const = 0;
    virtual const char* symbol() const { return "■"; }
    char cellValue() const { return cellValue_; }

    virtual std::string landMessage() const { return ""; }

protected:
    Block(const Rotations& rotations, char value);

    static const Rotations& tetromino(int shapeIndex);

    static Rotations sameForAllAngles(const Grid& g) { return { g, g, g, g }; }

private:
    Rotations rotations_;
    char cellValue_;
    int angle_ = 0;
    int x_ = START_X;
    int y_ = START_Y;
};