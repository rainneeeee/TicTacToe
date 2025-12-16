#include "TicTacToe.h"

/* constructor definition */
TicTacToe::TicTacToe()
    :   isTurnOfX(true), diagLeftCounter(0), diagRightCounter(0),
        rFlag(false), cFlag(false), dlFlag(false), drFlag(false), 
        moveStack()
{
    board = board_t(BOARD_SIZE, boardrow_t(BOARD_SIZE, ' '));
    rowCounter = std::vector<int>(BOARD_SIZE, 0);
    colCounter = std::vector<int>(BOARD_SIZE, 0);
}

char TicTacToe::get(COORD coord) const
{
    return board.at(coord.Y).at(coord.X);
}

bool TicTacToe::makeMove(COORD coord)
{
    /* unable to move because the cell is occupied */
    if(this->get(coord) != ' ') { return false; }

    board.at(coord.Y).at(coord.X) = (isTurnOfX) ? 'x' : 'o';
    moveStack.push(coord);

    rowCounter.at(coord.Y) += (isTurnOfX) ? 1 : -1;
    colCounter.at(coord.X) += (isTurnOfX) ? 1 : -1;
    if(coord.X == coord.Y) { diagLeftCounter += (isTurnOfX) ? 1 : -1; }
    if((coord.X + coord.Y) == 2) { diagRightCounter += (isTurnOfX) ? 1 : -1; }

    isTurnOfX = !isTurnOfX;

    /* if one of these flags is true, a straight line is formed */
    rFlag = abs(rowCounter.at(coord.Y)) == BOARD_SIZE;
    cFlag = abs(colCounter.at(coord.X)) == BOARD_SIZE;
    dlFlag = abs(diagLeftCounter) == BOARD_SIZE;
    drFlag = abs(diagRightCounter) == BOARD_SIZE;

    /* define in which row or column the straight line is formed */
    if(rFlag) { rPos == coord.Y; }
    if(cFlag) { cPos == coord.X; }

    return (rFlag | cFlag | dlFlag | drFlag);

    return false;
}

void TicTacToe::undoMove(COORD& toRemove)
{
    if(moveStack.empty()) { return; }

    COORD lastMove(moveStack.top());
    toRemove = lastMove;

    moveStack.pop();
    
    board.at(lastMove.Y).at(lastMove.X) = ' ';

    rowCounter.at(lastMove.Y) += (isTurnOfX) ? 1 : -1;
    colCounter.at(lastMove.X) += (isTurnOfX) ? 1 : -1;

    if(lastMove.X == lastMove.Y) { diagLeftCounter += (isTurnOfX) ? 1 : -1; }
    if((lastMove.X + lastMove.Y) == 2) { diagRightCounter += (isTurnOfX) ? 1 : -1; }

    isTurnOfX = !isTurnOfX;
}