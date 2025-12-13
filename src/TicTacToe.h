#pragma once

#include <vector>
#include <stdexcept>
#include <exception>

#ifndef TICTACTOE_H
#define TICTACTOE_H

constexpr size_t BOARD_SIZE = 3;

using boardrow_t = std::vector<char>;
using board_t = std::vector<boardrow_t>;

class TicTacToe {
public:
    /* one of these will be equal to true if a player wins */
    bool rFlag,     /* horizontal line */
         cFlag,     /* vertical line */
         dlFlag,    /* diagonal left line */
         drFlag;    /* diagonal right line */
    /* row or column in which a player wins */
    int rPos, cPos;

    TicTacToe();

    /* FALSE = no winner; TRUE = a player wins */
    bool makeMove(short x, short y);
    void undoMove(short x, short y);
    char get(short x, short y) const;

private:
    /* the matrix representing the board */
    board_t board;
    /* 
        if x wins, the value of one of these counters will be 3
        if o wins, the value of one of these counters will be -3.
    */
    std::vector<int> rowCounter, colCounter;    /* straights */
    int diagLeftCounter, diagRightCounter;      /* diagonals */
    /* turn flag */
    bool isTurnOfX;
};

#endif