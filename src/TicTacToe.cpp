#include "TicTacToe.h"

/* constructor definition */
TicTacToe::TicTacToe()
    :   isTurnOfX(true), diagLeftCounter(0), diagRightCounter(0),
        rFlag(false), cFlag(false), dlFlag(false), drFlag(false)
{
    board = board_t(BOARD_SIZE, boardrow_t(BOARD_SIZE, ' '));
    rowCounter = std::vector<int>(BOARD_SIZE, 0);
    colCounter = std::vector<int>(BOARD_SIZE, 0);
}

bool TicTacToe::makeMove(short x, short y)
{
    /* unable to move because the cell is occupied */
    if(board.at(y).at(x) != ' ') { return false; }

    board.at(y).at(x) = (isTurnOfX) ? 'x' : 'o';

    rowCounter.at(y) += (isTurnOfX) ? 1 : -1;
    colCounter.at(x) += (isTurnOfX) ? 1 : -1;
    if(x == y) { diagLeftCounter += (isTurnOfX) ? 1 : -1; }
    if((x + y) == 2) { diagRightCounter += (isTurnOfX) ? 1 : -1; }

    isTurnOfX = !isTurnOfX;

    /* if one of these flags is true, a straight line is formed */
    rFlag = abs(rowCounter.at(y)) == BOARD_SIZE;
    cFlag = abs(colCounter.at(x)) == BOARD_SIZE;
    dlFlag = abs(diagLeftCounter) == BOARD_SIZE;
    drFlag = abs(diagRightCounter) == BOARD_SIZE;

    /* define in which row or column the straight line is formed */
    if(rFlag) { rPos == y; }
    if(cFlag) { cPos == x; }

    return (rFlag | cFlag | dlFlag | drFlag);

    return false;
}

void TicTacToe::undoMove(short x, short y)
{
    if(board.at(y).at(x) == ' ') { return; }
    
    /* return to previous turn if current character is deleted */
    isTurnOfX = board.at(y).at(x) == 'o';
    board.at(y).at(x) = ' ';

    rowCounter.at(y) += (isTurnOfX) ? 1 : -1;
    colCounter.at(x) += (isTurnOfX) ? 1 : -1;

    if(x == y) { diagLeftCounter += (isTurnOfX) ? 1 : -1; }
    if((x + y) == 2) { diagRightCounter += (isTurnOfX) ? 1 : -1; }

    isTurnOfX = !isTurnOfX;
}

char TicTacToe::get(short x, short y) const
{
    return board.at(y).at(x);
}