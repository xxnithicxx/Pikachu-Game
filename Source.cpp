#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <conio.h> // For keyboard input (getch())
#include <windows.h>
#include <string>
#include <string.h>
#include <math.h>
#include <time.h>
#include <fstream>
#include <io.h>    // Call _setmode
#include <fcntl.h> // _O_U16TEXT

using namespace std;

#define _O_U16TEXT 0x20000

#define EASY 4   // Matrix size is 4x4
#define MEDIUM 6 // Matrix size is 6x6
#define HARD 8   // Matrix size is 8x8

#define ARROW_UP 0x48
#define ARROW_LEFT 0x4B
#define ARROW_RIGHT 0x4D
#define ARROW_DOWN 0x50
#define ARROW_NONE 0x00
#define ESC_KEY 0x1B
#define ENTER_KEY 0x0D
#define SPACE_KEY 0x20
#define HELP 0x68

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 40

#define BLACK 0
#define AQUA 3
#define RED 4
#define WHITE 7
#define GRAY 8
#define GREEN 10
#define YELLOW 14

#define WORD_WIDTH_SPACING 8
#define WORD_HEIGHT_SPACING 3

// Set windows specific size scale with difficulty
// Ref: https://codelearn.io/sharing/windowsh-va-ham-dinh-dang-console-p1

struct Selected
{
    SHORT posX;
    SHORT posY;

    bool isSelected;

    void swap(Selected &other)
    {
        SHORT tempX = posX;
        SHORT tempY = posY;

        posX = other.posX;
        posY = other.posY;

        other.posX = tempX;
        other.posY = tempY;
    }

    bool operator==(Selected other)
    {
        return (posX == other.posX && posY == other.posY);
    }

    // Pre-increment for easy working
    // Node should be selected from the left to the right, top to bottom. The only exception is when two node is in the 2 and 4 zone in xy field, they will go from bottom to right.
    void prepareSelected(Selected &other)
    {
        // If 2 node is the same, do nothing
        if (posX == other.posX && posY == other.posY)
            return;

        // If 2 node is not the same, swap their position
        // If this.x is higher than other.x, swap their position
        if (posX < other.posX && posY > other.posY)
        {
            return;
        }

        else if (posX > other.posX)
        {
            swap(other);
            return;
        }
        // If this.y is lower than other.y, swap their position
        else if (posY > other.posY)
        {
            swap(other);
        }
    }
};

struct Score
{
    time_t startTime;
    int error = 0;

    void start()
    {
        startTime = time(NULL);
    }

    int total()
    {
        return (int)floor(time(NULL) - startTime) - error * 5;
    }

    void addError()
    {
        error++;
    }
} score;

struct BackGround
{
    string **BG;

    void GetBG(int difficulty);
    void PrintBG(int difficulty, Selected A);
};

void SetWindowSize(int difficulty)
{
    // SHORT is the type of variable in WINAPI
    SHORT width = difficulty * 11 + 40; // Width of console
    SHORT height = difficulty * 5 + 5;  // Height of console

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    SMALL_RECT WindowSize;
    WindowSize.Top = 0;
    WindowSize.Left = 0;
    WindowSize.Right = width;
    WindowSize.Bottom = height;

    SetConsoleWindowInfo(hStdout, 1, &WindowSize);
}

void MoveWindow(int posx, int posy)
{
    RECT rectClient, rectWindow;
    HWND hWnd = GetConsoleWindow();
    GetClientRect(hWnd, &rectClient);
    GetWindowRect(hWnd, &rectWindow);
    MoveWindow(hWnd, posx, posy, rectClient.right - rectClient.left, rectClient.bottom - rectClient.top, TRUE);
}

// Disable resize window
void DisableResizeWindow()
{
    HWND hWnd = GetConsoleWindow();
    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_SIZEBOX);
}

// Disable maximize button
void DisableMaximizeButton()
{
    HWND hWnd = GetConsoleWindow();
    HMENU hMenu = GetSystemMenu(hWnd, false);

    DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
}

// Disable cursor under lines
void DisableCur()
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ConCurInf;

    ConCurInf.dwSize = 10;
    ConCurInf.bVisible = false;

    SetConsoleCursorInfo(handle, &ConCurInf);
}

// Hide the scroll bar
void HideScrollbar()
{
    HWND hWnd = GetConsoleWindow();
    ShowScrollBar(hWnd, SB_BOTH, false);
}

// Set console color
void SetColor(int backgound_color, int text_color)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    int color_code = backgound_color * 16 + text_color;
    SetConsoleTextAttribute(hStdout, color_code);
}

// Set cursor position
void GoTo(SHORT posX, SHORT posY)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD Position;
    Position.X = posX;
    Position.Y = posY;

    SetConsoleCursorPosition(hStdout, Position);
}

// Take arrow keys input
// Ref: https://www.dreamincode.net/forums/topic/210388-how-to-track-arrow-keys-in-c/page__p__1222836&#entry1222836
unsigned char GetArrow()
{
    unsigned char ch = getch();
    if (ch == 0x00)
    {
        // first char is a zero so lets look at the next char
        ch = getch();
        switch (ch)
        {
        case ARROW_UP:
            return ch;
        case ARROW_LEFT:
            return ch;
        case ARROW_RIGHT:
            return ch;
        case ARROW_DOWN:
            return ch;
        default:
            return ARROW_NONE;
        }
    }
    else
    {
        // When user use wasd keys
        switch (ch)
        {
        case 'w':
            return ARROW_UP;
        case 'a':
            return ARROW_LEFT;
        case 'd':
            return ARROW_RIGHT;
        case 's':
            return ARROW_DOWN;
        case 'h':
            return HELP;
        }
    }

    // Enter key is the same as space key
    if (ch == ENTER_KEY || ch == SPACE_KEY)
    {
        return ENTER_KEY;
    }

    if (ch == ESC_KEY)
    {
        return ch;
    }

    return ch;
}

// Calculate position of the word in the console
int calculatePositionWidth(int posInMatrix, int difficulty)
{
    return (WORD_WIDTH_SPACING + 1) * posInMatrix + 8; // +8 is for the border
}

int calculatePositionHeight(int posInMatrix, int difficulty)
{
    return (WORD_HEIGHT_SPACING + 1) * posInMatrix + 4; // +4 is for the border
}

void DrawBorder(int difficulty)
{
    int minX = calculatePositionWidth(-1, difficulty) + 3;
    int maxX = calculatePositionWidth(difficulty, difficulty) + 4;
    int minY = calculatePositionHeight(-1, difficulty) + 1;
    int maxY = calculatePositionHeight(difficulty, difficulty) + 5;

    // Draw top border
    GoTo(minX, minY);
    SetColor(BLACK, WHITE);

    wprintf(L"┌");
    for (int i = minX + 1; i <= maxX; i++)
    {
        GoTo(i, minY);
        wprintf(L"-");
    }
    GoTo(maxX + 1, minY);
    wprintf(L"┐");

    // Draw bottom border
    GoTo(minX, maxY);
    wprintf(L"└");
    for (int i = minX + 1; i <= maxX; i++)
    {
        GoTo(i, maxY);
        wprintf(L"-");
    }
    GoTo(maxX + 1, maxY);
    wprintf(L"┘");

    // Draw left border
    for (int i = minY + 1; i < maxY; i++)
    {
        GoTo(minX, i);
        wprintf(L"|");
    }

    // Draw right border
    for (int i = minY + 1; i < maxY; i++)
    {
        GoTo(maxX + 1, i);
        wprintf(L"|");
    }
}

// Restore line
void RestoreLine(int posX, int posY, unsigned int numberOfChars, int difficulty)
{
    GoTo(posX, posY);
    SetColor(BLACK, WHITE);
    for (int i = 0; i < numberOfChars; i++)
    {
        GoTo(posX + i, posY);
        wprintf(L" ");
    }
}

// Restore column
void RestoreColumn(int posX, int posY, unsigned int numberOfChars, int difficulty)
{
    GoTo(posX, posY);
    SetColor(BLACK, WHITE);
    for (int i = 0; i < numberOfChars; i++)
    {
        GoTo(posX, posY + i);
        wprintf(L" ");
    }
}

// Print the matrix with color
void DrawMatrix(char **a, int difficulty)
{
    for (int i = 0; i < difficulty; i++)
    {
        for (int j = 0; j < difficulty; j++)
        {
            int posY = calculatePositionHeight(i, difficulty);
            int posX = calculatePositionWidth(j, difficulty);
            GoTo(posX, posY++);
            wprintf(L" ------- \n");
            GoTo(posX, posY++);
            wprintf(L"|       |\n");
            GoTo(posX, posY++);
            wprintf(L"|   %c   | \n", a[i][j]);
            GoTo(posX, posY++);
            wprintf(L"|       |\n");
            GoTo(posX, posY++);
            wprintf(L" ------- \n");
        }
    }
}

void DrawCube(char **a, int difficulty, Selected A, int backgound_color, int text_color)
{
    SetColor(backgound_color, text_color);
    // Draw the cube
    int posY = calculatePositionHeight(A.posY, difficulty) + 1;
    int posX = calculatePositionWidth(A.posX, difficulty) + 1;
    GoTo(posX, posY++);
    wprintf(L"       \n");
    GoTo(posX, posY++);
    wprintf(L"   %c   \n", a[A.posY][A.posX]);
    GoTo(posX, posY++);
    wprintf(L"       \n");
    SetColor(BLACK, WHITE);
}

void DeleteCube(char **a, int difficulty, Selected A)
{
    SetColor(BLACK, BLACK);
    int posY = calculatePositionHeight(A.posY, difficulty);
    int posX = calculatePositionWidth(A.posX, difficulty);
    GoTo(posX, posY++);
    if ((A.posY - 1) > 0 && (a[A.posY - 1][A.posX] == ' '))
        wprintf(L"        \n");
    if (A.posY == 0)
        wprintf(L"        \n");
    GoTo(posX, posY++);
    wprintf(L"|       |\n");
    GoTo(posX, posY++);
    wprintf(L"|       |\n");
    GoTo(posX, posY++);
    wprintf(L"|       |\n");
    GoTo(posX, posY++);
    if ((A.posY + 1) < difficulty && a[A.posY + 1][A.posX] == ' ')
        wprintf(L"        \n");
    if (A.posY == difficulty - 1)
        wprintf(L"        \n");
    SetColor(BLACK, WHITE);
}

void BackGround::GetBG(int difficulty)
{
    int sizeBG = difficulty * difficulty;
    BG = new string *[sizeBG];
    for (int i = 0; i < sizeBG; i++)
    {
        BG[i] = new string[4];
    }

    ifstream file;
    // Open 2.txt in Background folder
    file.open("./BackGround/1.txt");
    if (file.is_open())
    {
        string temp;
        for (int i = 0; i < difficulty; i++) // Number of row in matrix pikachu
        {
            for (int j = 0; j < 4; j++) // 4 is the width of the cube with not overlap the next cube
            {
                getline(file, temp);
                for (int k = 0; k < difficulty; k++) // Number of column in matrix pikachu
                {
                    // Cut the string for 9 characters
                    string content = temp.substr(k * WORD_WIDTH_SPACING, 9);
                    BG[i + k * difficulty][j] = content;
                }
            }
        }
    }
    else
    {
        cout << "Unable to open file" << endl;
    }

    file.close();
}

void BackGround::PrintBG(int difficulty, Selected A)
{
    if (A.posX < 0 || A.posY < 0 || A.posX >= difficulty || A.posY >= difficulty)
        return;

    SetColor(BLACK, WHITE);
    int position = A.posY + A.posX * difficulty;
    int posY = calculatePositionHeight(A.posY, difficulty);
    int posX = calculatePositionWidth(A.posX, difficulty);

    for (int i = 0; i < 4; i++)
    {
        GoTo(posX, posY++);
        wprintf(L"%s", BG[position][i].c_str());
        // wcout << BG[position][i].size();
    }
    if (A.posY == difficulty - 1)
    {
        GoTo(posX, posY++);
        wprintf(L"         ");
    }
}

void DrawHorizonLine(Selected A, Selected B)
{
    if (A == B)
        return;

    A.prepareSelected(B);

    int startX = calculatePositionWidth(A.posX, EASY) + 5;
    int endX = calculatePositionWidth(B.posX, EASY) + 3;
    int horizonPos = calculatePositionHeight(A.posY, EASY) + 2;
    GoTo(startX, horizonPos);
    wprintf(L"←");
    for (int i = startX + 1; i <= endX - 1; i++)
    {
        GoTo(i, horizonPos);
        wprintf(L"-");
    }
    GoTo(endX, horizonPos);
    wprintf(L"→");
    return;
}

void DrawVerticalLine(Selected A, Selected B)
{
    if (A == B)
        return;

    A.prepareSelected(B);

    int startY = calculatePositionHeight(A.posY, EASY) + 3;
    int endY = calculatePositionHeight(B.posY, EASY) + 1;
    int verticalPos = calculatePositionWidth(A.posX, EASY) + 4;
    GoTo(verticalPos, startY);
    wprintf(L"↑");
    for (int i = startY + 1; i <= endY - 1; i++)
    {
        GoTo(verticalPos, i);
        wprintf(L"|");
    }
    GoTo(verticalPos, endY);
    wprintf(L"↓");
    return;
}

// Solve I part horizontal
bool checkLineV(char **a, Selected A, Selected B, int difficulty)
{
    // Check if A and B is in the same Column
    if (A.posX != B.posX)
    {
        return false;
    }

    // This step already checked in Selected.prepare() function, A will always be higher than B
    /*
        int rowMax = max(A.posY, B.posY);
        int rowMin = min(A.posY, B.posY);
    */

    A.prepareSelected(B);

    for (int i = A.posY + 1; i < B.posY; i++)
    {
        if (a[i][A.posX] != ' ')
            return false;
    }
    return true;
}

// Sole I part vertical
bool checkLineH(char **a, Selected A, Selected B, int difficulty)
{
    // Check if A and B is in the same Row
    if (A.posY != B.posY)
    {
        return false;
    }

    // This step already checked in Selected.prepare() function, A will always at the right than B
    /*
        int colMax = max(A.posX, B.posX);
        int colMin = min(A.posX, B.posX);
    */

    A.prepareSelected(B);

    for (int i = A.posX + 1; i < B.posX; i++)
    {
        if (a[A.posY][i] != ' ')
            return false;
    }
    return true;
}

bool checkLine(char **&a, Selected A, Selected B, int difficulty, BackGround backGround)
{

    if (checkLineH(a, A, B, difficulty))
    {
        DrawCube(a, difficulty, A, GREEN, YELLOW);
        DrawCube(a, difficulty, B, GREEN, YELLOW);
        DrawHorizonLine(A, B);
        GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 3);
        wprintf(L"This is I shape");

        Sleep(1000);

        a[A.posY][A.posX] = ' ';
        a[B.posY][B.posX] = ' ';

        for (int i = A.posX; i <= B.posX + 1; i++)
        {
            // DeleteCube(a, difficulty, A);
            backGround.PrintBG(difficulty, A);
            A.posX = i;
        }
        return true;
    }

    if (checkLineV(a, A, B, difficulty))
    {
        DrawCube(a, difficulty, A, GREEN, YELLOW);
        DrawCube(a, difficulty, B, GREEN, YELLOW);
        DrawVerticalLine(A, B);
        GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 3);
        wprintf(L"This is I shape");

        Sleep(1000);

        a[A.posY][A.posX] = ' ';
        a[B.posY][B.posX] = ' ';

        for (int i = A.posY; i <= B.posY + 1; i++)
        {
            // DeleteCube(a, difficulty, A);
            backGround.PrintBG(difficulty, A);
            A.posY = i;
        }
        return true;
    }

    return false;
}

bool checkHLShape(char **a, Selected A, Selected B, int difficulty, Selected &C, bool &mode)
{
    // Take A is the coordinate root to start scanning
    C = {A.posX, B.posY};

    if (a[C.posY][C.posX] == ' ')
    {
        if (checkLineH(a, C, B, difficulty) && checkLineV(a, A, C, difficulty))
        {
            mode = true;
            return true;
        }
    }

    C = {B.posX, A.posY};

    if (a[C.posY][C.posX] == ' ')
    {
        if (checkLineV(a, C, B, difficulty) && checkLineH(a, A, C, difficulty))
        {
            mode = false;
            return true;
        }
    }

    return false;
}

bool checkLShape(char **a, Selected A, Selected B, int difficulty, BackGround backGround)
{
    Selected C;
    bool mode;

    A.prepareSelected(B);

    if (checkHLShape(a, A, B, difficulty, C, mode))
    {
        if (mode)
        {
            DrawCube(a, difficulty, A, GREEN, YELLOW);
            DrawCube(a, difficulty, B, GREEN, YELLOW);
            DrawHorizonLine(B, C);
            DrawVerticalLine(A, C);
            GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 3);
            wprintf(L"This is L Shape");

            Sleep(1000);

            a[A.posY][A.posX] = ' ';
            a[B.posY][B.posX] = ' ';

            for (B.posX; B.posX >= C.posX; B.posX--)
            {
                // DeleteCube(a, difficulty, B);
                backGround.PrintBG(difficulty, B);
            }

            C.prepareSelected(A);

            for (C.posY; C.posY <= A.posY; C.posY++)
            {
                // DeleteCube(a, difficulty, C);
                backGround.PrintBG(difficulty, C);
            }
        }
        else
        {
            DrawCube(a, difficulty, A, GREEN, YELLOW);
            DrawCube(a, difficulty, B, GREEN, YELLOW);
            DrawVerticalLine(C, B);
            DrawHorizonLine(C, A);
            GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 3);
            wprintf(L"This is L Shape");

            Sleep(1000);

            a[A.posY][A.posX] = ' ';
            a[B.posY][B.posX] = ' ';

            for (A.posX; A.posX <= B.posX; A.posX++)
            {
                // DeleteCube(a, difficulty, A);
                backGround.PrintBG(difficulty, A);

            }

            C.prepareSelected(B);

            for (C.posY; C.posY <= B.posY; C.posY++)
            {
                // DeleteCube(a, difficulty, C);
                backGround.PrintBG(difficulty, C);
            }
        }
        return true;
    }
    return false;
}

// Check horizontal rectangle
bool checkHRectangle(char **a, Selected A, Selected B, int difficulty, Selected &C, Selected &D)
{
    // Take A is the coordinate root to start scanning
    C = A;
    D = {A.posX, B.posY};

    int maxX = max(A.posX, B.posX);
    int minX = min(A.posX, B.posX);

    for (int i = minX; i <= maxX; i++)
    {
        if (a[C.posY][C.posX] != ' ' || a[D.posY][D.posX] != ' ')
        {
            C.posX++;
            D.posX++;
            continue;
        }
        if (checkLineV(a, C, D, difficulty))
        {
            if (checkLineH(a, A, C, difficulty) && checkLineH(a, B, D, difficulty))
            {
                return true;
            }
        }
        C.posX++;
        D.posX++;
    }
    return false;
}

bool checkVRectangle(char **a, Selected A, Selected B, int difficulty, Selected &C, Selected &D)
{
    // Take A is the coordinate root to start scanning
    C = A;
    D = {B.posX, A.posY};

    int vec = (A.posY > B.posY) ? -1 : 1;

    int maxY = max(A.posY, B.posY);
    int minY = min(A.posY, B.posY);

    for (int i = minY; i <= maxY; i++)
    {
        if (a[C.posY][C.posX] != ' ' || a[D.posY][D.posX] != ' ')
        {
            C.posY += vec;
            D.posY += vec;
            continue;
        }
        if (checkLineH(a, C, D, difficulty))
        {
            if (checkLineV(a, A, C, difficulty) && checkLineV(a, B, D, difficulty))
            {
                return true;
            }
        }
        C.posY += vec;
        D.posY += vec;
    }
    return false;
}

bool checkRectangle(char **a, Selected A, Selected B, int difficulty, BackGround backGround)
{
    Selected temp1, temp2;

    A.prepareSelected(B);

    if (checkHRectangle(a, A, B, difficulty, temp1, temp2))
    {
        DrawCube(a, difficulty, A, GREEN, YELLOW);
        DrawCube(a, difficulty, B, GREEN, YELLOW);
        DrawVerticalLine(temp1, temp2);
        DrawHorizonLine(A, temp1);
        DrawHorizonLine(temp2, B);
        GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 3);
        wprintf(L"This is Z shape\n");
        Sleep(1000);

        a[A.posY][A.posX] = ' ';
        a[B.posY][B.posX] = ' ';

        for (SHORT i = A.posX; i <= temp1.posX; i++)
        {
            // DeleteCube(a, difficulty, Selected{i, A.posY});
            backGround.PrintBG(difficulty, Selected{i, A.posY});
        }
        for (SHORT i = temp2.posX; i <= B.posX; i++)
        {
            // DeleteCube(a, difficulty, Selected{i, B.posY});
            backGround.PrintBG(difficulty, Selected{i, B.posY});
        }
        temp1.prepareSelected(temp2);
        for (SHORT i = temp1.posY; i <= temp2.posY; i++)
        {
            // DeleteCube(a, difficulty, Selected{temp1.posX, i});
            backGround.PrintBG(difficulty, Selected{temp1.posX, i});
        }

        return true;
    }
    if (checkVRectangle(a, A, B, difficulty, temp1, temp2))
    {
        DrawCube(a, difficulty, A, GREEN, YELLOW);
        DrawCube(a, difficulty, B, GREEN, YELLOW);
        DrawHorizonLine(temp1, temp2);
        DrawVerticalLine(A, temp1);
        DrawVerticalLine(temp2, B);
        GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 3);
        wprintf(L"This is Z shape\n");

        Sleep(1000);

        a[A.posY][A.posX] = ' ';
        a[B.posY][B.posX] = ' ';

        for (SHORT i = temp1.posX; i <= temp2.posX; i++)
        {
            // DeleteCube(a, difficulty, Selected{i, temp1.posY});
            backGround.PrintBG(difficulty, Selected{i, temp1.posY});
        }
        A.prepareSelected(temp1);
        for (SHORT i = A.posY; i <= temp1.posY; i++)
        {
            // DeleteCube(a, difficulty, Selected{A.posX, i});
            backGround.PrintBG(difficulty, Selected{A.posX, i});
        }
        B.prepareSelected(temp2);
        for (SHORT i = B.posY; i <= temp2.posY; i++)
        {
            // DeleteCube(a, difficulty, Selected{B.posX, i});
            backGround.PrintBG(difficulty, Selected{B.posX, i});
        }
        return true;
    }

    return false;
}

// Return true if U left, false if U right
bool checkHorizontalU(char **a, Selected A, Selected B, int difficulty, Selected &C, Selected &D)
{
    SHORT difficultySHORT = (SHORT)difficulty;

    SHORT minY = min(A.posY, B.posY);
    SHORT maxY = max(A.posY, B.posY);

    C = {-1, minY};
    D = {-1, maxY};

    // Exception: if A.posY is higher than B.posY ("⬈" vector)
    if (A.posY > B.posY && A.posX < B.posX)
    {
        C.swap(D);
    }

    // if (a[C.posY][C.posX] == a[A.posY][A.posX] &&)
    if (checkLineH(a, A, C, difficulty) && checkLineH(a, B, D, difficulty))
    {
        return true;
    }

    C = {difficultySHORT, minY};
    D = {difficultySHORT, maxY};

    // Exception: if A.posY is higher than B.posY ("⬈" vector)
    if (A.posY > B.posY && A.posX < B.posX)
    {
        C.swap(D);
    }

    if (checkLineH(a, A, C, difficulty) && checkLineH(a, B, D, difficulty))
    {
        return true;
    }

    for (C.posX = 1, D.posX = 1; D.posX < B.posX && C.posX < A.posX; C.posX++, D.posX++)
    {
        if (checkLineV(a, C, D, difficulty) && a[C.posY][C.posX] == ' ' && a[D.posY][D.posX] == ' ')
        {
            if (checkLineH(a, A, C, difficulty) && checkLineH(a, B, D, difficulty))
            {
                return true;
            }
        }
    }

    for (C.posX = difficulty - 1, D.posX = difficulty - 1; D.posX > B.posX && C.posX > A.posX; C.posX--, D.posX--)
    {
        if (checkLineV(a, C, D, difficulty) && a[C.posY][C.posX] == ' ' && a[D.posY][D.posX] == ' ')
        {
            if (checkLineH(a, A, C, difficulty) && checkLineH(a, B, D, difficulty))
            {
                return true;
            }
        }
    }

    return false;
}

// Return true if U up, false if U down
bool checkVerticalU(char **a, Selected A, Selected B, int difficulty, Selected &C, Selected &D)
{
    SHORT difficultySHORT = (SHORT)difficulty;

    SHORT minX = min((SHORT)A.posX, B.posX);
    SHORT maxX = max((SHORT)A.posX, B.posX);

    C = {minX, -1};
    D = {maxX, -1};

    if (checkLineV(a, A, C, difficulty) && checkLineV(a, B, D, difficulty))
    {
        return true;
    }

    C = {minX, difficultySHORT};
    D = {maxX, difficultySHORT};

    if (checkLineV(a, A, C, difficulty) && checkLineV(a, B, D, difficulty))
    {
        return true;
    }

    for (C.posY = 1, D.posY = 1; D.posY < B.posY && C.posY < A.posY; C.posY++, D.posY++)
    {
        if (checkLineH(a, C, D, difficulty) && a[C.posY][C.posX] == ' ' && a[D.posY][D.posX] == ' ')
        {
            if (checkLineV(a, A, C, difficulty) && checkLineV(a, B, D, difficulty))
            {
                return true;
            }
        }
    }

    for (C.posY = difficulty - 1, D.posY = difficulty - 1; D.posY > B.posY && C.posY > A.posY; C.posY--, D.posY--)
    {
        if (checkLineH(a, C, D, difficulty) && a[C.posY][C.posX] == ' ' && a[D.posY][D.posX] == ' ')
        {
            if (checkLineV(a, A, C, difficulty) && checkLineV(a, B, D, difficulty))
            {
                return true;
            }
        }
    }

    return false;
}

bool checkUShape(char **a, Selected A, Selected B, int difficulty, BackGround backGround)
{
    Selected C;
    Selected D;

    A.prepareSelected(B);

    if (checkHorizontalU(a, A, B, difficulty, C, D))
    {
        DrawCube(a, difficulty, A, GREEN, YELLOW);
        DrawCube(a, difficulty, B, GREEN, YELLOW);
        DrawHorizonLine(A, C);
        DrawHorizonLine(B, D);
        DrawVerticalLine(C, D);
        GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 3);
        wprintf(L"This is U shape\n");

        Sleep(1000);

        a[A.posY][A.posX] = ' ';
        a[B.posY][B.posX] = ' ';

        // Exception: if A.posY is higher than B.posY ("⬈" vector)
        if (A.posY > B.posY && A.posX < B.posX)
        {
            C.swap(D);
        }

        SHORT minX = min(A.posX, C.posX);
        SHORT maxX = max(A.posX, C.posX);

        for (SHORT i = minX + 1; i < maxX; i++)
        {
            // DeleteCube(a, difficulty, Selected{i, A.posY});
            backGround.PrintBG(difficulty, Selected{i, A.posY});
        }

        minX = min(B.posX, D.posX);
        maxX = max(B.posX, D.posX);

        for (SHORT i = minX + 1; i < maxX; i++)
        {
            // DeleteCube(a, difficulty, Selected{i, B.posY});
            backGround.PrintBG(difficulty, Selected{i, B.posY});
        }

        // DeleteCube(a, difficulty, A);
        // DeleteCube(a, difficulty, B);
        backGround.PrintBG(difficulty, A);
        backGround.PrintBG(difficulty, B);

        if (C.posX == -1)
        {
            int posXDel = calculatePositionWidth(-1, difficulty) + 4;
            int posYDel = calculatePositionHeight(C.posY, difficulty) + 2;
            int sizeofLine = (D.posY - C.posY) * (WORD_HEIGHT_SPACING + 1) + 2;

            for (int i = 0; i < 5; i++)
            {
                RestoreColumn(posXDel + i, posYDel, sizeofLine, difficulty);
            }
        }
        else if (C.posX == difficulty)
        {
            int posXDel = calculatePositionWidth(difficulty, difficulty);
            int posYDel = calculatePositionHeight(C.posY, difficulty) + 2;
            int sizeofLine = (D.posY - C.posY) * (WORD_HEIGHT_SPACING + 1) + 2;
            for (int i = 0; i < 5; i++)
            {
                RestoreColumn(posXDel + i, posYDel, sizeofLine, difficulty);
            }
        }
        else
        {
            for (C.posY; C.posY <= D.posY; C.posY++)
            {
                // DeleteCube(a, difficulty, C);
                backGround.PrintBG(difficulty, C);
            }
        }

        return true;
    }

    if (checkVerticalU(a, A, B, difficulty, C, D))
    {
        DrawCube(a, difficulty, A, GREEN, YELLOW);
        DrawCube(a, difficulty, B, GREEN, YELLOW);
        DrawVerticalLine(A, C);
        DrawVerticalLine(B, D);
        DrawHorizonLine(C, D);
        GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 3);
        wprintf(L"This is U shape\n");

        Sleep(1000);

        a[A.posY][A.posX] = ' ';
        a[B.posY][B.posX] = ' ';

        SHORT minY = min(A.posY, C.posY);
        SHORT maxY = max(A.posY, C.posY);

        for (SHORT i = minY + 1; i < maxY; i++)
        {
            // DeleteCube(a, difficulty, Selected{A.posX, i});
            backGround.PrintBG(difficulty, Selected{A.posX, i});
        }

        minY = min(B.posY, D.posY);
        maxY = max(B.posY, D.posY);

        for (SHORT i = minY + 1; i < maxY; i++)
        {
            // DeleteCube(a, difficulty, Selected{B.posX, i});
            backGround.PrintBG(difficulty, Selected{B.posX, i});
        }

        // DeleteCube(a, difficulty, A);
        // DeleteCube(a, difficulty, B);
        backGround.PrintBG(difficulty, A);
        backGround.PrintBG(difficulty, B);

        if (C.posY == -1)
        {
            int posXDel = calculatePositionWidth(C.posX, difficulty) + 3;
            int posYDel = calculatePositionHeight(-1, difficulty) + 2;
            int sizeofLine = (D.posX - C.posX) * (WORD_WIDTH_SPACING + 1) + 2;
            for (int i = 0; i < 2; i++)
            {
                RestoreLine(posXDel, posYDel + i, sizeofLine, difficulty);
            }
        }
        else if (C.posY == difficulty)
        {
            int posXDel = calculatePositionWidth(C.posX, difficulty) + 3;
            int posYDel = calculatePositionHeight(C.posY, difficulty) + 1;
            int sizeofLine = (D.posX - C.posX) * (WORD_WIDTH_SPACING + 1) + 2;
            for (int i = 0; i < 2; i++)
            {
                RestoreLine(posXDel, posYDel + i, sizeofLine, difficulty);
            }
        }
        else
        {
            for (C.posX; C.posX <= D.posX; C.posX++)
            {
                // DeleteCube(a, difficulty, C);
                backGround.PrintBG(difficulty, C);
            }
        }
        return true;
    }
    return false;
}

// Menu for the difficulty

// Menu for the game

// Check if the matrix is solved
bool isSolved(char **a, int difficulty)
{
    for (int i = 0; i < difficulty; i++)
    {
        for (int j = 0; j < difficulty; j++)
        {
            if (a[i][j] != ' ')
            {
                return false;
            }
        }
    }
    return true;
}

// Check if 2 node selected is the same as the pair
bool checkNodeIdentical(char **&matrix, int difficulty, Selected &a, Selected &b, BackGround backGround)
{
    bool identical = false;

    identical = (matrix[a.posY][a.posX] == matrix[b.posY][b.posX]);

    // Check if 2 node can link with together with pattern
    if (identical)
    {
        if (checkLine(matrix, a, b, difficulty, backGround))
        {
            a.isSelected = false;
            b.isSelected = false;
            return true;
        }
        else if (checkLShape(matrix, a, b, difficulty, backGround))
        {
            a.isSelected = false;
            b.isSelected = false;
            return true;
        }
        else if (checkRectangle(matrix, a, b, difficulty, backGround))
        {
            a.isSelected = false;
            b.isSelected = false;
            return true;
        }
        else if (checkUShape(matrix, a, b, difficulty, backGround))
        {
            a.isSelected = false;
            b.isSelected = false;
            return true;
        }

        DrawCube(matrix, difficulty, a, RED, YELLOW);
        DrawCube(matrix, difficulty, b, RED, YELLOW);
        wprintf(L"%c", 7);
        Sleep(1000);
        score.addError();

        DrawCube(matrix, difficulty, a, BLACK, WHITE);
        DrawCube(matrix, difficulty, b, BLACK, WHITE);
        a.isSelected = false;
        b.isSelected = false;
        return false;
    }

    // If 2 node is not the same, hightlight the 2 node with red backgound_color and delay for 1 second
    if (!identical)
    {
        // Highlight the 2 node with red backgound_color
        DrawCube(matrix, difficulty, a, RED, YELLOW);
        DrawCube(matrix, difficulty, b, RED, YELLOW);
        wprintf(L"%c", 7);
        Sleep(1000);
        score.addError();

        // Delete the red background_color
        DrawCube(matrix, difficulty, a, BLACK, WHITE);
        DrawCube(matrix, difficulty, b, BLACK, WHITE);
        a.isSelected = false;
        b.isSelected = false;
    }

    return identical;
}

// No drawing graphic
bool checkNodeIdenticalN(char **a, int difficulty, Selected A, Selected B)
{
    Selected C, D;
    bool mode = 0;

    A.prepareSelected(B);

    if (checkLineH(a, A, B, difficulty))
    {
        return true;
    }
    else if (checkLineV(a, A, B, difficulty))
    {
        return true;
    }
    else if (checkHLShape(a, A, B, difficulty, C, mode))
    {
        return true;
    }
    else if (checkHRectangle(a, A, B, difficulty, C, D))
    {
        return true;
    }
    else if (checkVRectangle(a, A, B, difficulty, C, D))
    {
        return true;
    }
    else if (checkHorizontalU(a, A, B, difficulty, C, D))
    {
        return true;
    }
    else if (checkVerticalU(a, A, B, difficulty, C, D))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Check if the matrix can be solve
bool checkSolve(char **&a, int difficulty)
{
    // Find pair of identical elements in the matrix, run the checkNodeIdentical function to check if the pair can link with pattern, if true, delete the pair and start again
    char **temp = NULL;
    temp = (char **)malloc(sizeof(char *) * difficulty);
    for (int i = 0; i < difficulty; i++)
    {
        temp[i] = (char *)malloc(sizeof(char) * difficulty);
    }
    for (int i = 0; i < difficulty; i++)
    {
        for (int j = 0; j < difficulty; j++)
        {
            temp[i][j] = a[i][j];
        }
    }

Checked:
    for (SHORT i = 0; i < difficulty; i++) // i is the y coordinate
    {
        for (SHORT j = 0; j < difficulty; j++) // j is the x coordinate
        {
            // If a[i][j] is space or ' ', continue
            if (temp[i][j] == ' ')
            {
                continue;
            }

            for (SHORT k = 0; k < difficulty; k++) // k is the y coordinate
            {
                for (SHORT l = 0; l < difficulty; l++) // l is the x coordinate
                {
                    if (i == k && j == l)
                    {
                        continue;
                    }

                    if (temp[i][j] != temp[k][l])
                    {
                        continue;
                    }

                    // If a[i][j] is the same as a[k][l], check if the pair can link with pattern
                    Selected A = {j, i};
                    Selected B = {l, k};

                    if (checkNodeIdenticalN(temp, difficulty, A, B))
                    {
                        temp[i][j] = ' ';
                        temp[k][l] = ' ';

                        goto Checked;
                    }
                }
            }
        }
    }

    if (isSolved(temp, difficulty))
    {
        // Free temp
        for (int i = 0; i < difficulty; i++)
        {
            free(temp[i]);
        }
        free(temp);

        return true;
    }

    // Free temp
    for (int i = 0; i < difficulty; i++)
    {
        free(temp[i]);
    }
    free(temp);

    return false;
}

void helpMove(char **&a, int difficulty)
{
    for (SHORT i = 0; i < difficulty; i++) // i is the y coordinate
    {
        for (SHORT j = 0; j < difficulty; j++) // j is the x coordinate
        {
            // If a[i][j] is space or ' ', continue
            if (a[i][j] == ' ')
            {
                continue;
            }

            for (SHORT k = 0; k < difficulty; k++) // k is the y coordinate
            {
                for (SHORT l = 0; l < difficulty; l++) // l is the x coordinate
                {
                    if (i == k && j == l)
                    {
                        continue;
                    }

                    if (a[i][j] != a[k][l])
                    {
                        continue;
                    }

                    // If a[i][j] is the same as a[k][l], check if the pair can link with pattern
                    Selected A = {j, i};
                    Selected B = {l, k};

                    if (checkNodeIdenticalN(a, difficulty, A, B))
                    {
                        DrawCube(a, difficulty, A, GRAY, YELLOW);
                        DrawCube(a, difficulty, B, GRAY, YELLOW);
                        Sleep(1000);

                        DrawCube(a, difficulty, A, BLACK, WHITE);
                        DrawCube(a, difficulty, B, BLACK, WHITE);
                        return;
                    }
                }
            }
        }
    }
}

void createMatrixPikachu(char **&a, int difficulty)
{
    // Create a square matrix for the given difficulty
    a = (char **)malloc(sizeof(char *) * difficulty);

    for (int i = 0; i < difficulty; i++)
    {
        a[i] = (char *)malloc(sizeof(char) * difficulty);
    }

    // Create a array of pair interger pairs
    // The array will only contain 25 distinct character pairs
    int sizeOfPair = difficulty * difficulty;
    char arrPair[sizeOfPair];
    int count = 0;
    for (int i = 0; i < sizeOfPair; i++)
    {
        arrPair[i] = count / 2 + 'A';
        count++;
        if (count == 50)
        {
            count = 0;
        }
    }

// Shuffle the array of pairs
Shuffle:
    for (int i = 0; i < difficulty; i++)
    {
        for (int j = 0; j < difficulty; j++)
        {
            int posInPairArray = rand() % (sizeOfPair);
            a[i][j] = arrPair[posInPairArray];

            // Swap the value in posInPairArray to the end of array and decrease the size of array
            swap(arrPair[posInPairArray], arrPair[sizeOfPair - 1]);
            sizeOfPair--;
        }
    }

    if (!checkSolve(a, difficulty))
    {
        sizeOfPair = difficulty * difficulty;
        goto Shuffle;
    }
}

// Move the cursor to the position with matrix and check if the position is valid
bool moveToPosition(char **a, int difficulty, SHORT posX, SHORT posY)
{
    if (posX < 0 || posX >= difficulty || posY < 0 || posY >= difficulty)
    {
        return true;
    }

    DrawCube(a, difficulty, Selected{posX, posY}, WHITE, BLACK);

    return false;
}

// Release the memory of matrix
void releaseMatrix(char **a, int difficulty)
{
    for (int i = 0; i < difficulty; i++)
    {
        free(a[i]);
    }
    free(a);
}

Selected coordinateValid(char **a, int difficulty, char move, Selected cod)
{
    switch (move)
    {
    case 'w':
        for (SHORT i = cod.posY - 1; i >= 0; i--)
        {
            for (SHORT j = 0; j < difficulty; j++)
            {
                if (a[i][j] != ' ')
                {
                    return Selected{j, i};
                }
            }
        }

        for (SHORT i = difficulty - 1; i > cod.posY; i--)
        {
            for (SHORT j = 0; j < difficulty; j++)
            {
                if (a[i][j] != ' ')
                {
                    return Selected{j, i};
                }
            }
        }
    case 's':
        for (SHORT i = cod.posY + 1; i < difficulty; i++)
        {
            for (SHORT j = 0; j < difficulty; j++)
            {
                if (a[i][j] != ' ')
                {
                    return Selected{j, i};
                }
            }
        }

        for (SHORT i = 0; i <= cod.posY; i++)
        {
            for (SHORT j = 0; j < difficulty; j++)
            {
                if (a[i][j] != ' ')
                {
                    return Selected{j, i};
                }
            }
        }
    case 'a':
        for (SHORT i = cod.posX - 1; i >= 0; i--)
        {
            for (SHORT j = 0; j < difficulty; j++)
            {
                if (a[j][i] != ' ')
                {
                    return Selected{i, j};
                }
            }
        }

        for (SHORT i = difficulty - 1; i > cod.posX; i--)
        {
            for (SHORT j = 0; j < difficulty; j++)
            {
                if (a[j][i] != ' ')
                {
                    return Selected{i, j};
                }
            }
        }
    case 'd':
        for (SHORT i = cod.posX + 1; i < difficulty; i++)
        {
            for (SHORT j = 0; j < difficulty; j++)
            {
                if (a[j][i] != ' ')
                {
                    return Selected{i, j};
                }
            }
        }

        for (SHORT i = 0; i < cod.posX; i++)
        {
            for (SHORT j = 0; j < difficulty; j++)
            {
                if (a[j][i] != ' ')
                {
                    return Selected{i, j};
                }
            }
        }
    }

    return Selected{-1, -1};
}

SHORT coordinateH(char **a, int difficulty, bool up, Selected cod)
{
    if (up == true)
    {

        for (SHORT i = cod.posY - 1; i >= 0; i--)
        {
            if (a[i][cod.posX] != ' ')
            {
                return i;
            }
        }

        for (SHORT i = difficulty - 1; i > cod.posY; i--)
        {
            if (a[i][cod.posX] != ' ')
            {
                return i;
            }
        }

        return -1;
    }

    for (SHORT i = cod.posY + 1; i < difficulty; i++)
    {
        if (a[i][cod.posX] != ' ')
        {
            return i;
        }
    }

    for (SHORT i = 0; i <= cod.posY; i++)
    {
        if (a[i][cod.posX] != ' ')
        {
            return i;
        }
    }

    return -1;
}

SHORT coordinateV(char **a, int difficulty, bool left, Selected cod)
{
    if (left == true)
    {
        for (SHORT i = cod.posX - 1; i >= 0; i--)
        {
            if (a[cod.posY][i] != ' ')
            {
                return i;
            }
        }

        for (SHORT i = difficulty - 1; i > cod.posX; i--)
        {
            if (a[cod.posY][i] != ' ')
            {
                return i;
            }
        }

        return -1;
    }

    for (SHORT i = cod.posX + 1; i < difficulty; i++)
    {
        if (a[cod.posY][i] != ' ')
        {
            return i;
        }
    }

    for (SHORT i = 0; i < cod.posX; i++)
    {
        if (a[cod.posY][i] != ' ')
        {
            return i;
        }
    }

    return -1;
}

Selected checkLocalElements(char **a, int difficulty, Selected A)
{
    SHORT posX = A.posX;
    SHORT posY = A.posY;

    if (posX - 1 >= 0 && a[posY][posX - 1] != ' ')
    {
        return Selected{(SHORT)(posX - 1), posY};
    }

    if (posX + 1 < difficulty && a[posY][posX + 1] != ' ')
    {
        return Selected{(SHORT)(posX + 1), posY};
    }

    if (posY - 1 >= 0 && a[posY - 1][posX] != ' ')
    {
        return Selected{posX, (SHORT)(posY - 1)};
    }

    if (posY + 1 < difficulty && a[posY + 1][posX] != ' ')
    {
        return Selected{posX, (SHORT)(posY + 1)};
    }

    if (posX - 1 >= difficulty && posY - 1 >= difficulty && a[posY - 1][posX - 1] != ' ')
    {
        return Selected{(SHORT)(posX - 1), (SHORT)(posY - 1)};
    }

    if (posX + 1 < difficulty && posY + 1 < difficulty && a[posY + 1][posX + 1] != ' ')
    {
        return Selected{(SHORT)(posX + 1), (SHORT)(posY + 1)};
    }

    if (posX - 1 >= 0 && posY + 1 < difficulty && a[posY + 1][posX - 1] != ' ')
    {
        return Selected{(SHORT)(posX - 1), (SHORT)(posY + 1)};
    }

    if (posX + 1 < difficulty && posY - 1 >= 0 && a[posY - 1][posX + 1] != ' ')
    {
        return Selected{(SHORT)(posX + 1), (SHORT)(posY - 1)};
    }

    Selected newCor = coordinateValid(a, difficulty, 'w', A);
    if (memcmp(&newCor, &A, sizeof(Selected)) != 0)
        return newCor;
    newCor = coordinateValid(a, difficulty, 's', A);
    if (memcmp(&newCor, &A, sizeof(Selected)) != 0)
        return newCor;
    newCor = coordinateValid(a, difficulty, 'a', A);
    if (memcmp(&newCor, &A, sizeof(Selected)) != 0)
        return newCor;
    newCor = coordinateValid(a, difficulty, 'd', A);
    if (memcmp(&newCor, &A, sizeof(Selected)) != 0)
        return newCor;

    return Selected{-1, -1};
}

int main(int argc, char **argv)
{
    MoveWindow(0, 0);
    char **matrix = NULL;
    int difficulty = HARD;

    system("cls");
    SetWindowSize(difficulty);
    SetConsoleTitleW(L"Pikachu"); // Change console title (L is for Unicode)
    // DisableResizeWindow();
    // DisableMaximizeButton();
    DisableCur();
    // HideScrollbar();
    _setmode(_fileno(stdout), _O_U16TEXT);

    srand(time(NULL));
    createMatrixPikachu(matrix, difficulty);
    DrawMatrix(matrix, difficulty);
    DrawBorder(difficulty);

    // Clear keyboard buffer
    fflush(stdin);

    // Initialize the position of selected Node
    Selected firstNode = {-1, -1, false};
    Selected secondNode = {-1, -1, false};
    Selected tempNode = {0, 0, false};

    SHORT posX = 0, posY = 0;
    char ch = ENTER_KEY;
    BackGround backGround;

    // Hightlight the (0, 0) position
    DrawCube(matrix, difficulty, Selected{0, 0}, WHITE, BLACK);

    backGround.GetBG(difficulty);
    score.startTime;

    // Run loop for playing game
    while ((ch = GetArrow()) != ESC_KEY)
    {
        switch (ch)
        {
        case ARROW_UP:
            posY = coordinateH(matrix, difficulty, true, Selected{posX, posY});
            if (posY == -1)
            {
                Selected newCor = coordinateValid(matrix, difficulty, 'w', Selected{posX, posY});
                posX = newCor.posX;
                posY = newCor.posY;
            }
            break;
        case ARROW_DOWN:
            posY = coordinateH(matrix, difficulty, false, Selected{posX, posY});
            if (posY == -1)
            {
                Selected newCor = coordinateValid(matrix, difficulty, 's', Selected{posX, posY});
                posX = newCor.posX;
                posY = newCor.posY;
            }
            break;
        case ARROW_LEFT:
            posX = coordinateV(matrix, difficulty, true, Selected{posX, posY});
            if (posX == -1)
            {
                Selected newCor = coordinateValid(matrix, difficulty, 'a', Selected{posX, posY});
                posX = newCor.posX;
                posY = newCor.posY;
            }
            break;
        case ARROW_RIGHT:
            posX = coordinateV(matrix, difficulty, false, Selected{posX, posY});
            if (posX == -1)
            {
                Selected newCor = coordinateValid(matrix, difficulty, 'd', Selected{posX, posY});
                posX = newCor.posX;
                posY = newCor.posY;
            }
            break;
        case HELP:
            helpMove(matrix, difficulty);
            break;
        case ENTER_KEY:
            if (matrix[posY][posX] == ' ')
                break;
            if (!firstNode.isSelected)
            {
                GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 1);
                wprintf(L"First Node Selected\n");
                firstNode.isSelected = true;
                firstNode.posX = posX;
                firstNode.posY = posY;
            }
            else if (!secondNode.isSelected)
            {
                if (firstNode.posX == posX && firstNode.posY == posY)
                {
                    GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 1);
                    wprintf(L"You can't select the same node twice\n");
                    continue;
                }
                GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 1);
                wprintf(L"Second Node Selected\n");
                secondNode.isSelected = true;
                secondNode.posX = posX;
                secondNode.posY = posY;
            }
            else
            {
                GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 3);
                wprintf(L"You can't select more than 2 nodes\n");
            }
            break;
        default:
            break;
        }

        // Restore the previous Node to original state
        DrawCube(matrix, difficulty, tempNode, BLACK, WHITE);

        if (firstNode.isSelected)
        {
            DrawCube(matrix, difficulty, firstNode, AQUA, BLACK);
        }

        if (secondNode.isSelected)
        {
            DrawCube(matrix, difficulty, secondNode, AQUA, BLACK);
            firstNode.prepareSelected(secondNode);

            // Check if 2 nodes are selected is the same character
            if (checkNodeIdentical(matrix, difficulty, firstNode, secondNode, backGround))
            {
                GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 2);
                wprintf(L"2 nodes are identical\n");
                // Draw background at the posX and posY coordinates

                // Check local elements of the matrix if it different from ' '
                Selected newCor = checkLocalElements(matrix, difficulty, secondNode);

                posX = newCor.posX;
                posY = newCor.posY;
            }
            else
            {
                GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 2);
                wprintf(L"2 nodes are different\n");
            }

            if (isSolved(matrix, difficulty))
            {
                GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 3);
                wprintf(L"You win\n");
            }
        }

        if (moveToPosition(matrix, difficulty, posX, posY))
        {
            break;
        }

        tempNode.posX = posX;
        tempNode.posY = posY;
    }

    releaseMatrix(matrix, difficulty);

    GoTo(0, calculatePositionHeight(difficulty, difficulty) + 2);
    SetColor(BLACK, AQUA);
    wprintf(L"Thanks for playing!\n");

    system("pause");

    return 0;
}