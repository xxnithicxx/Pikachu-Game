#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <conio.h> // For keyboard input (getch())
#include <windows.h>
#include <string>

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

void SetWindowSize(int difficulty)
{
    // SHORT is the type of variable in WINAPI
    SHORT width = difficulty * 11 + 40; // Width of console
    SHORT height = difficulty * 5 + 10; // Height of console

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    SMALL_RECT WindowSize;
    WindowSize.Top = 0;
    WindowSize.Left = 0;
    WindowSize.Right = width;
    WindowSize.Bottom = height;

    SetConsoleWindowInfo(hStdout, 1, &WindowSize);
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
    if (ch == ENTER_KEY || ch == ESC_KEY)
    {
        return ch;
    }
    return ch;
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
    int sizeOfPair = difficulty * difficulty;
    char arrPair[sizeOfPair];
    for (int i = 0; i < sizeOfPair; i++)
    {
        arrPair[i] = i / 2 + 'A';
    }

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
}

// Calculate position of the word in the console
int calculatePositionWidth(int posInMatrix, int difficulty)
{
    return (WORD_WIDTH_SPACING + 1) * posInMatrix + 4; // +4 is for the border
}

int calculatePositionHeight(int posInMatrix, int difficulty)
{
    return (WORD_HEIGHT_SPACING + 1) * posInMatrix + 2; // +2 is for the border
}

// Print the matrix with color
void drawMatrix(char **a, int difficulty)
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

void DeleteCude(char **a, int difficulty, Selected A)
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

void DrawHorizonLine(Selected A, Selected B)
{
    if (A == B)
        return;
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

bool checkLine(char **&a, Selected A, Selected B, int difficulty)
{

    if (checkLineH(a, A, B, difficulty))
    {
        DrawCube(a, difficulty, A, GREEN, YELLOW);
        DrawCube(a, difficulty, B, GREEN, YELLOW);
        DrawHorizonLine(A, B);
        GoTo(4, calculatePositionHeight(difficulty, difficulty) + 4);
        wprintf(L"This is I shape");

        Sleep(1000);

        a[A.posY][A.posX] = ' ';
        a[B.posY][B.posX] = ' ';

        for (int i = A.posX; i <= B.posX + 1; i++)
        {
            DeleteCude(a, difficulty, A);
            A.posX = i;
        }
        return true;
    }

    if (checkLineV(a, A, B, difficulty))
    {
        DrawCube(a, difficulty, A, GREEN, YELLOW);
        DrawCube(a, difficulty, B, GREEN, YELLOW);
        DrawVerticalLine(A, B);
        GoTo(4, calculatePositionHeight(difficulty, difficulty) + 4);
        wprintf(L"This is I shape");

        Sleep(1000);

        a[A.posY][A.posX] = ' ';
        a[B.posY][B.posX] = ' ';

        for (int i = A.posY; i <= B.posY + 1; i++)
        {
            DeleteCude(a, difficulty, A);
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

bool checkLShape(char **a, Selected A, Selected B, int difficulty)
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
            GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 4);
            wprintf(L"L H 1 Shape");

            Sleep(1000);

            a[A.posY][A.posX] = ' ';
            a[B.posY][B.posX] = ' ';

            for (B.posX; B.posX >= C.posX; B.posX--)
            {
                DeleteCude(a, difficulty, B);
            }

            C.prepareSelected(A);

            for (C.posY; C.posY <= A.posY; C.posY++)
            {
                DeleteCude(a, difficulty, C);
            }
        }
        else
        {
            DrawCube(a, difficulty, A, GREEN, YELLOW);
        DrawCube(a, difficulty, B, GREEN, YELLOW);
            DrawVerticalLine(C, B);
            DrawHorizonLine(C, A);
            GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 4);
            wprintf(L"L H 2 Shape");

            Sleep(1000);

            a[A.posY][A.posX] = ' ';
            a[B.posY][B.posX] = ' ';

            for (A.posX; A.posX <= B.posX; A.posX++)
            {
                DeleteCude(a, difficulty, A);
            }

            C.prepareSelected(B);

            for (C.posY; C.posY <= B.posY; C.posY++)
            {
                DeleteCude(a, difficulty, C);
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
    for (int i = A.posX; i <= B.posX; i++)
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
    for (int i = A.posY; i <= B.posY; i++)
    {
        if (a[C.posY][C.posX] != ' ' || a[D.posY][D.posX] != ' ')
        {
            C.posY++;
            D.posY++;
            continue;
        }
        if (checkLineH(a, C, D, difficulty))
        {
            if (checkLineV(a, A, C, difficulty) && checkLineV(a, B, D, difficulty))
            {
                return true;
            }
        }
        C.posY++;
        D.posY++;
    }
    return false;
}

bool checkRectangle(char **a, Selected A, Selected B, int difficulty)
{
    if (a[A.posY][A.posX] != a[B.posY][B.posX])
    {
        return false;
    }

    Selected temp1, temp2;
    if (checkHRectangle(a, A, B, difficulty, temp1, temp2))
    {
        DrawCube(a, difficulty, A, GREEN, YELLOW);
        DrawCube(a, difficulty, B, GREEN, YELLOW);
        DrawVerticalLine(temp1, temp2);
        DrawHorizonLine(A, temp1);
        DrawHorizonLine(temp2, B); // Bug here
        GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 4);
        wprintf(L"This is Z shape\n");
        Sleep(1000);

        a[A.posY][A.posX] = ' ';
        a[B.posY][B.posX] = ' ';

        for (SHORT i = A.posX; i <= temp1.posX; i++)
        {
            DeleteCude(a, difficulty, Selected{i, A.posY});
        }
        for (SHORT i = temp2.posX; i <= B.posX; i++)
        {
            DeleteCude(a, difficulty, Selected{i, B.posY});
        }
        temp1.prepareSelected(temp2);
        for (SHORT i = temp1.posY; i <= temp2.posY; i++)
        {
            DeleteCude(a, difficulty, Selected{temp1.posX, i});
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
        GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 4);
        wprintf(L"This is Z shape\n");
        Sleep(1000);

        a[A.posY][A.posX] = ' ';
        a[B.posY][B.posX] = ' ';

        for (SHORT i = A.posY; i <= temp1.posY; i++)
        {
            DeleteCude(a, difficulty, Selected{A.posX, i});
        }
        for (SHORT i = temp2.posY; i <= B.posY; i++)
        {
            DeleteCude(a, difficulty, Selected{B.posX, i});
        }
        temp1.prepareSelected(temp2);
        for (SHORT i = temp1.posX; i <= temp2.posX; i++)
        {
            DeleteCude(a, difficulty, Selected{i, temp1.posY});
        }

        return true;
    }

    return false;
}

// Menu for the difficulty
char menuDifficulty()
{
    wprintf(L"Difficulty: \n");
    return getch();
}

// Menu for the game

// Check if the matrix is solved

// Check if 2 node selected is the same as the pair
bool checkNodeIdentical(char **&matrix, int difficulty, Selected &a, Selected &b)
{
    bool identical = false;

    identical = (matrix[a.posY][a.posX] == matrix[b.posY][b.posX]);

    // Check if 2 node can link with together with pattern
    if (identical)
    {
        if (checkLine(matrix, a, b, difficulty))
        {
            a.isSelected = false;
            b.isSelected = false;
            return true;
        }
        else if (checkLShape(matrix, a, b, difficulty))
        {
            a.isSelected = false;
            b.isSelected = false;
            return true;
        }
        else if (checkRectangle(matrix, a, b, difficulty))
        {
            a.isSelected = false;
            b.isSelected = false;
            return true;
        }
        return false;
    }

    // If 2 node is not the same, hightlight the 2 node with red backgound_color and delay for 1 second
    if (!identical)
    {
        // Highlight the 2 node with red backgound_color
        DrawCube(matrix, difficulty, a, RED, YELLOW);
        DrawCube(matrix, difficulty, b, RED, YELLOW);
        wprintf(L"%c", 7);
        // Beep(440, 1000);
        Sleep(1000);

        // Delete the red background_color
        DrawCube(matrix, difficulty, a, BLACK, WHITE);
        DrawCube(matrix, difficulty, b, BLACK, WHITE);
        a.isSelected = false;
        b.isSelected = false;
    }

    return identical;
}

// Restore the Node to previous state
void restoreNode(Selected &node, char **a, int difficulty)
{
    // Restore the node to previous state
    // GoTo(node.posX + node.posX * WORD_WIDTH_SPACING, node.posY + node.posY * WORD_HEIGHT_SPACING);
    // SetColor(BLACK, WHITE);
    // wprintf(L" %c ", a[node.posY][node.posX]);

    DrawCube(a, difficulty, node, BLACK, WHITE);
}

// Restore line
void restoreLine(SHORT line, unsigned int numberOfChars)
{
    GoTo(0, line);
    SetColor(BLACK, WHITE);
    for (int i = 0; i < numberOfChars; i++)
    {
        wprintf(L" ");
    }
}

// Move the cursor to the position with matrix and check if the position is valid
bool moveToPosition(char **a, int difficulty, SHORT posX, SHORT posY)
{
    if (posX < 0 || posX >= difficulty || posY < 0 || posY >= difficulty)
    {
        return true;
    }

    // GoTo(calculatePositionWidth(posX, difficulty) + 3, calculatePositionHeight(posY, difficulty) + 2);
    // SetColor(WHITE, BLACK);
    // wprintf(L"►%c", a[posY][posX]);
    DrawCube(a, difficulty, Selected{posX, posY}, WHITE, BLACK);

    return false;
}

void hightlightNodeSelect(char **a, int difficulty, Selected node)
{
    // Highlight the node
    // GoTo(calculatePositionWidth(node.posX, difficulty), calculatePositionHeight(node.posY, difficulty));
    // SetColor(AQUA, BLACK);
    // wprintf(L" %c√", a[node.posY][node.posX]);

    DrawCube(a, difficulty, node, AQUA, BLACK);
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

int main(int argc, char **argv)
{
    char **matrix = NULL;
    int difficulty = EASY;

    system("cls");
    SetWindowSize(difficulty);
    SetConsoleTitleW(L"Pikachu"); // Change console title (L is for Unicode)
    DisableResizeWindow();
    DisableMaximizeButton();
    DisableCur();
    HideScrollbar();
    _setmode(_fileno(stdout), _O_U16TEXT);

    srand(time(NULL));
    createMatrixPikachu(matrix, difficulty);
    drawMatrix(matrix, difficulty);

    // Clear keyboard buffer
    fflush(stdin);

    // Initialize the position of selected Node
    Selected firstNode = {-1, -1, false};
    Selected secondNode = {-1, -1, false};
    Selected tempNode = {0, 0, false};

    SHORT posX = 0, posY = 0;
    char ch = ENTER_KEY;

    // Hightlight the (0, 0) position
    DrawCube(matrix, difficulty, Selected{0, 0}, WHITE, BLACK);

    // Run loop for playing game
    while ((ch = GetArrow()) != ESC_KEY)
    {
        switch (ch)
        {
        case ARROW_UP:
            if (posY - 1 < 0)
            {
                posY = difficulty;
            }
            posY--;
            break;
        case ARROW_DOWN:
            if (posY + 1 >= difficulty)
            {
                posY = 0;
                break;
            }
            posY++;
            break;
        case ARROW_LEFT:
            if (posX - 1 < 0)
            {
                posX = difficulty;
            }
            posX--;
            break;
        case ARROW_RIGHT:
            if (posX + 1 >= difficulty)
            {
                posX = 0;
                break;
            }
            posX++;
            break;
        case ENTER_KEY:
            if (matrix[posY][posX] == ' ')
                break;
            if (!firstNode.isSelected)
            {
                GoTo(0, calculatePositionHeight(difficulty, difficulty) + 1);
                wprintf(L"First Node Selected\n");
                firstNode.isSelected = true;
                firstNode.posX = posX;
                firstNode.posY = posY;
            }
            else if (!secondNode.isSelected)
            {
                GoTo(0, calculatePositionHeight(difficulty, difficulty) + 1);
                wprintf(L"Second Node Selected\n");
                secondNode.isSelected = true;
                secondNode.posX = posX;
                secondNode.posY = posY;
            }
            else
            {
                GoTo(0, calculatePositionHeight(difficulty, difficulty) + 3);
                wprintf(L"You can't select more than 2 nodes\n");
            }
            break;
        default:
            break;
        }

        // Restore the previous Node to original state
        restoreNode(tempNode, matrix, difficulty);

        if (firstNode.isSelected)
        {
            hightlightNodeSelect(matrix, difficulty, firstNode);
        }

        if (secondNode.isSelected)
        {
            hightlightNodeSelect(matrix, difficulty, secondNode);
            firstNode.prepareSelected(secondNode);

            // Check if 2 nodes are selected is the same character
            if (checkNodeIdentical(matrix, difficulty, firstNode, secondNode))
            {
                GoTo(0, calculatePositionHeight(difficulty, difficulty) + 2);
                wprintf(L"2 nodes are identical\n");
            }
            else
            {
                GoTo(0, calculatePositionHeight(difficulty, difficulty) + 2);
                wprintf(L"2 nodes are different\n");
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