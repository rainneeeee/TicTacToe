/*
    AUTHOR: raii_
    COMPILER: MinGW
*/

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <exception>
#include <cassert>

#include <windows.h>
#include <conio.h>

static constexpr char newl = '\n';

static constexpr size_t BOARD_SIZE = 3;

using boardrow_t = std::vector<char>;
using board_t = std::vector<boardrow_t>;

class TicTacToe {
public:
    /* flags */
    bool rFlag, cFlag, dlFlag, drFlag;
    /* row or column where a player won */
    int rPos, cPos;

    TicTacToe()
    : isTurnOfX(true), diagLeftCounter(0), diagRightCounter(0),
      rFlag(false), cFlag(false), dlFlag(false), drFlag(false) {
        board = board_t(BOARD_SIZE, boardrow_t(BOARD_SIZE, ' '));
        rowCounter = std::vector<int>(BOARD_SIZE, 0);
        colCounter = std::vector<int>(BOARD_SIZE, 0);
    }

    /* FALSE if no winner and TRUE if a player wins */
    bool makeMove(short x, short y) {
        if(x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) { 
            throw std::runtime_error("Out of bounds exception.");
        }
        if(board[y][x] != ' ') { return false; }

        board[y][x] = (isTurnOfX) ? 'x' : 'o';

        rowCounter[y] += (isTurnOfX) ? 1 : -1;
        colCounter[x] += (isTurnOfX) ? 1 : -1;

        if(x == y) { diagLeftCounter += (isTurnOfX) ? 1 : -1; }
        if((x + y) == 2) { diagRightCounter += (isTurnOfX) ? 1 : -1; }

        isTurnOfX = !isTurnOfX;

        rFlag = abs(rowCounter[y]) == BOARD_SIZE;
        cFlag = abs(colCounter[x]) == BOARD_SIZE;
        dlFlag = abs(diagLeftCounter) == BOARD_SIZE;
        drFlag = abs(diagRightCounter) == BOARD_SIZE;

        if(rFlag) { rPos == y; }
        if(cFlag) { cPos == x; }

        return (rFlag | cFlag | dlFlag | drFlag);

        return false;
    }

    void undoMove(short x, short y) {
        if(x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) { 
            throw std::runtime_error("Out of bounds exception.");
        }
        if(board[y][x] == ' ') { return; }
        
        /* revert to previous turn if current character was deleted */
        isTurnOfX = board[y][x] == 'o';
        board[y][x] = ' ';

        rowCounter[y] += (isTurnOfX) ? 1 : -1;
        colCounter[x] += (isTurnOfX) ? 1 : -1;

        if(x == y) { diagLeftCounter += (isTurnOfX) ? 1 : -1; }
        if((x + y) == 2) { diagRightCounter += (isTurnOfX) ? 1 : -1; }

        isTurnOfX = !isTurnOfX;
    }

    char get(short x, short y) const {
        if(x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) { 
            throw std::runtime_error("Out of bounds exception.");
        }

        return board[y][x];
    }
private:
    /* the matrix that represents the board */
    board_t board;
    /* 
        if x won, the value of one of these counters will be 3
        if o won, the value of one of these counters will be -3
    */
    std::vector<int> rowCounter, colCounter;
    int diagLeftCounter, diagRightCounter;
    /* turn flag */
    bool isTurnOfX;
};

static constexpr SHORT MSB = 0x8000;

static constexpr WORD DEFAULT_COLOR = 0x7;
static constexpr WORD HIGHLIGHT_COLOR = 0x9;

static const std::vector<short> X_POINTS { 2, 6, 10 };
static const std::vector<short> Y_POINTS { 1, 3, 5 };

static void printBoardBorder(void)
{
    std::vector<std::string> borders { "|---|---|---|", "|   |   |   |" };

    for(int i = 0; i < 7; i++) {
        std::cout << borders[i & 1] << newl;
    }
}

static inline void setHighlight(HANDLE handle, bool&& isHighlighted)
{
    SetConsoleTextAttribute(handle, ((isHighlighted) ? HIGHLIGHT_COLOR : DEFAULT_COLOR));
}

static inline void gotoXY(HANDLE handle, const short& x, const short& y)
{
    SetConsoleCursorPosition(handle, {X_POINTS[x], Y_POINTS[y]});
}

static void setCursorVisible(HANDLE handle, CONSOLE_CURSOR_INFO* ci, bool&& setVisible)
{
    ci->bVisible = setVisible;
    SetConsoleCursorInfo(handle, ci);
}

static void printCaretOnBoard(HANDLE handle, const TicTacToe& t, const short& x, const short& y)
{
    gotoXY(handle, x, y);

    setHighlight(handle, true);
    std::cout << ((t.get(x, y) != ' ') ? t.get(x, y) : '_');
    setHighlight(handle, false);
}

static void clearCaret(HANDLE handle, const TicTacToe& t, const short& x, const short& y)
{
    gotoXY(handle, x, y);
    if(t.get(x, y) == ' ') {
        std::cout << ' ';
        return;
    }

    setHighlight(handle, false);
    std::cout << t.get(x, y);
}

static bool enterFunc(HANDLE handle, TicTacToe& t, const short& x, const short& y)
{
    if(t.get(x, y) != ' ') { return false; }

    bool result = t.makeMove(x, y);

    gotoXY(handle, x, y);

    setHighlight(handle, true);
    std::cout << t.get(x, y);
    setHighlight(handle, false);

    return result;
}

static void backFunc(HANDLE handle, TicTacToe& t, const short& x, const short& y)
{
    t.undoMove(x, y);

    gotoXY(handle, x, y);
    
    setHighlight(handle, false);
    std::cout << t.get(x, y);
}

int main(void)
{
    system("color 07");
    system("cls");

    assert(X_POINTS.size() == BOARD_SIZE);
    assert(Y_POINTS.size() == BOARD_SIZE);

    HANDLE hStdOut = nullptr;
    CONSOLE_CURSOR_INFO ci;

    try {
        hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if(!hStdOut) { throw std::runtime_error("Unable to retrieve STD_OUTPUT_HANDLE."); }

        GetConsoleCursorInfo(hStdOut, &ci);
        setCursorVisible(hStdOut, &ci, false);

        TicTacToe t;
        short x = 0, y = 0;

        printBoardBorder();
        std::cout << newl;
        printCaretOnBoard(hStdOut, t, x, y);

        while(true) {
            /* key inputs */
            if(GetAsyncKeyState(VK_UP) & MSB) {
                clearCaret(hStdOut, t, x, y);
                y = (y + (BOARD_SIZE - 1)) % BOARD_SIZE;
                printCaretOnBoard(hStdOut, t, x, y);
                Sleep(150);
            }
            if(GetAsyncKeyState(VK_DOWN) & MSB) {
                clearCaret(hStdOut, t, x, y);
                y = (y + 1) % BOARD_SIZE;
                printCaretOnBoard(hStdOut, t, x, y);
                Sleep(150);
            }
            if(GetAsyncKeyState(VK_RIGHT) & MSB) {
                clearCaret(hStdOut, t, x, y);
                x = (x + 1) % BOARD_SIZE;
                printCaretOnBoard(hStdOut, t, x, y);
                Sleep(150);
            }
            if(GetAsyncKeyState(VK_LEFT) & MSB) {
                clearCaret(hStdOut, t, x, y);
                x = (x + (BOARD_SIZE - 1)) % BOARD_SIZE;
                printCaretOnBoard(hStdOut, t, x, y);
                Sleep(150);
            }
            if(GetAsyncKeyState(VK_RETURN) & MSB) {
                if(enterFunc(hStdOut, t, x, y)) {
                    clearCaret(hStdOut, t, x, y);
                    SetConsoleTextAttribute(hStdOut, 0xE);

                    if(t.rFlag) {
                        for(short i = 0; i < BOARD_SIZE; i++) {
                            gotoXY(hStdOut, i, y);
                            std::cout << t.get(i, y);
                        }
                    }
                    if(t.cFlag) {
                        for(short i = 0; i < BOARD_SIZE; i++) {
                            gotoXY(hStdOut, x, i);
                            std::cout << t.get(x, i);
                        }
                    }
                    if(t.dlFlag) {
                        for(short i = 0; i < BOARD_SIZE; i++) {
                            gotoXY(hStdOut, i, i);
                            std::cout << t.get(i, i);
                        }
                    }
                    if(t.drFlag) {
                        for(short i = 0, j = 2; i < BOARD_SIZE; i++, j--) {
                            gotoXY(hStdOut, j, i);
                            std::cout << t.get(j, i);
                        }
                    }

                    SetConsoleCursorPosition(hStdOut, {0, 7});
                    setHighlight(hStdOut, true);

                    std::cout << "Closing in ";
                    for(int i = 3; i >= 1; i--) {
                        SetConsoleCursorPosition(hStdOut, {11, 7});
                        std::cout << i;
                        Sleep(1000);
                    }

                    setHighlight(hStdOut, false);

                    break;
                }
                Sleep(150);
            }
            if(GetAsyncKeyState(VK_BACK) & MSB) {
                backFunc(hStdOut, t, x, y);
                Sleep(150);
            }
            if(GetAsyncKeyState(VK_ESCAPE) & MSB) {
                SetConsoleCursorPosition(hStdOut, {0, 7});
                setHighlight(hStdOut, true);
                std::cout << "Press [ENTER] to quit the game.";
                setHighlight(hStdOut, false);
                Sleep(150);
                break;
            }
        }

        std::cin.get();

        setCursorVisible(hStdOut, &ci, true);
    } catch(std::exception& e) {
        std::cerr << e.what() << newl;
    }

    system("cls");

    if(hStdOut) { CloseHandle(hStdOut); }

    return 0;
}