#include "stdio.h"
#include "stdlib.h"
#include "conio.h"
#include "iostream"
#include <windows.h>

#include <io.h> // Call _setmode
#include <fcntl.h> // _O_U16TEXT
using namespace std;

#define EASY 4   // Matrix size is 4x4
#define MEDIUM 6 // Matrix size is 6x6
#define HARD 8   // Matrix size is 8x8

void SetWindowSize(int difficulty)
{
    // SHORT is the type of variable in WINAPI
    SHORT width = difficulty * 11 + 40;  // Width of console
    SHORT height = difficulty * 5 + 10; // Height of console

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    SMALL_RECT WindowSize;
    WindowSize.Top = 0;
    WindowSize.Left = 0;
    WindowSize.Right = width;
    WindowSize.Bottom = height;

    SetConsoleWindowInfo(hStdout, 1, &WindowSize);
}

int main(int argc, char **argv)
{
    _setmode(_fileno(stdout), _O_U16TEXT);

    SetWindowSize(MEDIUM);

    wprintf(L"test:  \x263a \x263b ");

    getch();
}