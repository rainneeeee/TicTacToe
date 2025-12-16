/*
    AUTHOR: raii_
    COMPILER: MinGW
*/

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cassert>

#include <windows.h>
#include <conio.h>

#include "TicTacToe.h"

#define UpAndLeftFunc(n) (n + (BOARD_SIZE-1)) % BOARD_SIZE
#define DownAndRightFunc(n) (n + 1) % BOARD_SIZE

static constexpr char newl = '\n';

static constexpr SHORT MSB = 0x8000;

enum class Color : WORD {
    DEFAULT = 0x7,
    HIGHLIGHT = 0x9,
    LIGHT_YELLOW = 0xE
};

/* valid points on printed board */
static const std::vector<short> X_POINTS { 2, 6, 10 };
static const std::vector<short> Y_POINTS { 1, 3, 5 };

static inline void clearScr(void) { system("cls"); }

static void printBoardBorder(void)
{
    std::vector<std::string> borders { "|---|---|---|", "|   |   |   |" };

    for(int i = 0; i < 7; i++) {
        std::cout << borders[i & 1] << newl;
    }
}

static inline void gotoXY(HANDLE handle, const short& x, const short& y)
{
    SetConsoleCursorPosition(handle, {x, y});
}

static inline void gotoBoardXY(HANDLE handle, const short& x, const short& y)
{
    SetConsoleCursorPosition(handle, {X_POINTS[x], Y_POINTS[y]});
}

static inline void setColor(HANDLE handle, WORD color)
{
    SetConsoleTextAttribute(handle, color);
}

static inline void setHighlight(HANDLE handle, bool&& isHighlighted)
{
    WORD color = static_cast<WORD>((isHighlighted) ? Color::HIGHLIGHT : Color::DEFAULT);
    setColor(handle, color);
}

static void setCursorVisible(HANDLE handle, CONSOLE_CURSOR_INFO* ci, bool&& setVisible)
{
    ci->bVisible = setVisible;
    SetConsoleCursorInfo(handle, ci);
}

static void printCaretOnBoard(HANDLE handle, const TicTacToe& t, const short& x, const short& y)
{
    gotoBoardXY(handle, x, y);

    setHighlight(handle, true);
    std::cout << ((t.get({x, y}) != ' ') ? t.get({x, y}) : '_');
    setHighlight(handle, false);
}

static void clearCaret(HANDLE handle, const TicTacToe& t, const short& x, const short& y)
{
    gotoBoardXY(handle, x, y);
    if(t.get({x, y}) == ' ') {
        std::cout << ' ';
        return;
    }

    setHighlight(handle, false);
    std::cout << t.get({x, y});
}

static inline SHORT keyState(int&& vKey) { return GetAsyncKeyState(vKey) & MSB; }

static void arrowFunc(  HANDLE handle, 
                        const TicTacToe& t, 
                        short& x, short& y, 
                        bool&& changeX, 
                        short&& newVal)
{
    clearCaret(handle, t, x, y);

    if(changeX) { x = newVal; }
    else { y = newVal; }

    printCaretOnBoard(handle, t, x, y);
    Sleep(150);
}

static bool turnFunc(HANDLE handle, TicTacToe& t, const short& x, const short& y)
{
    if(t.get({x, y}) != ' ') { return false; }

    bool result = t.makeMove({x, y});

    gotoBoardXY(handle, x, y);

    setHighlight(handle, true);
    std::cout << t.get({x, y});
    setHighlight(handle, false);

    return result;
}

static bool enterFunc(HANDLE handle, TicTacToe& t, const short& x, const short& y)
{
    if(!turnFunc(handle, t, x, y)) { return false; }

    clearCaret(handle, t, x, y);
    SetConsoleTextAttribute(handle, static_cast<WORD>(Color::LIGHT_YELLOW));

    if(t.rFlag) {
        for(short i = 0; i < BOARD_SIZE; i++) {
            gotoBoardXY(handle, i, y);
            std::cout << t.get({i, y});
        }
    }
    if(t.cFlag) {
        for(short i = 0; i < BOARD_SIZE; i++) {
            gotoBoardXY(handle, x, i);
            std::cout << t.get({x, i});
        }
    }
    if(t.dlFlag) {
        for(short i = 0; i < BOARD_SIZE; i++) {
            gotoBoardXY(handle, i, i);
            std::cout << t.get({i, i});
        }
    }
    if(t.drFlag) {
        for(short i = 0, j = 2; i < BOARD_SIZE; i++, j--) {
            gotoBoardXY(handle, j, i);
            std::cout << t.get({j, i});
        }
    }

    gotoXY(handle, 0, 7);
    setHighlight(handle, true);

    std::cout << "Closing in ";
    for(int i = 3; i >= 1; i--) {
        gotoXY(handle, 11, 7);
        std::cout << i;
        Sleep(1000);
    }

    setHighlight(handle, false);

    return true;
}

static void backFunc(HANDLE handle, TicTacToe& t)
{
    COORD toRemove;
    t.undoMove(toRemove);

    gotoBoardXY(handle, toRemove.X, toRemove.Y);
    
    setHighlight(handle, false);
    std::cout << t.get(toRemove);
}

static void escFunc(HANDLE handle)
{
    gotoXY(handle, 0, 7);

    setHighlight(handle, true);
    std::cout << "Press [ENTER] to quit the game.";
    setHighlight(handle, false);
}

int main(void)
{
    system("color 07"); /* 
                            making sure the background color is set to black 
                            and the foreground color is set to white
                        */
    clearScr();

    assert(X_POINTS.size() == BOARD_SIZE);
    assert(Y_POINTS.size() == BOARD_SIZE);

    HANDLE hStdOut = nullptr;
    CONSOLE_CURSOR_INFO ci;

    try {
        hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if(!hStdOut) { throw std::runtime_error("Unable to get STD_OUTPUT_HANDLE."); }

        GetConsoleCursorInfo(hStdOut, &ci);
        setCursorVisible(hStdOut, &ci, false);

        TicTacToe t;
        short x = 0, y = 0;

        printBoardBorder();
        std::cout << newl;
        printCaretOnBoard(hStdOut, t, x, y);

        while(true) {
            /* key inputs */
            if(keyState(VK_UP)) { 
                arrowFunc(hStdOut, t, x, y, false, UpAndLeftFunc(y)); 
            }
            if(keyState(VK_DOWN)) { 
                arrowFunc(hStdOut, t, x, y, false, DownAndRightFunc(y)); 
            }
            if(keyState(VK_RIGHT)) { 
                arrowFunc(hStdOut, t, x, y, true, DownAndRightFunc(x)); 
            }
            if(keyState(VK_LEFT)) { 
                arrowFunc(hStdOut, t, x, y, true, UpAndLeftFunc(x)); 
            }
            if(keyState(VK_RETURN)) {
                if(enterFunc(hStdOut, t, x, y)) { break; }
                Sleep(150);
            }
            if(keyState(VK_BACK)) {
                backFunc(hStdOut, t);
                printCaretOnBoard(hStdOut, t, x, y);
                Sleep(150);
            }
            if(keyState(VK_ESCAPE)) {
                escFunc(hStdOut);
                Sleep(150);
                break;
            }
        }

        std::cin.get();

        setCursorVisible(hStdOut, &ci, true);
    } catch(std::exception& e) {
        std::cerr << e.what() << newl;
    }

    clearScr();

    return 0;
}