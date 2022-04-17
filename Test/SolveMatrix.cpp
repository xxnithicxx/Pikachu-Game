#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <iostream>
#include <wchar.h>
#include <locale.h>
#include <io.h>    // Call _setmode
#include <fcntl.h> // _O_U16TEXT
using namespace std;

#define _O_U16TEXT 0x20000

#define EASY 4   // Matrix size is 4x4
#define MEDIUM 6 // Matrix size is 6x6
#define HARD 8   // Matrix size is 8x8

#define BLACK 0
#define AQUA 3
#define RED 4
#define WHITE 7
#define GRAY 8
#define GREEN 10
#define YELLOW 14

#define WORD_WIDTH_SPACING 8
#define WORD_HEIGHT_SPACING 3

struct Selected
{
    /* posX is represented by the cols number
     * posY is represented by the rows number
     */
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

    // Pre-increment for easy working
    // Node should be selected from the left to the right, top to bottom. The only exception is when two node is in the 2 and 4 zone in xy field, they will go from bottom to right.
    void prepareSelected(Selected &other)
    {
        // If 2 node is the same, do nothing
        if (posX == other.posX && posY == other.posY)
            return;

        // If 2 node is not the same, swap their position
        // If this.x is higher than other.x, swap their position
        if (posX > other.posX)
        {
            swap(other);
        }

        // If this.y is lower than other.y, swap their position
        else if (posY > other.posY)
        {
            swap(other);
        }
    }
};

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

// Calculate position of the word in the console
int calculatePositionWidth(int posInMatrix, int difficulty)
{
    return (WORD_WIDTH_SPACING + 1) * posInMatrix + 4; // +4 is for the border
}

int calculatePositionHeight(int posInMatrix, int difficulty)
{
    return (WORD_HEIGHT_SPACING + 1) * posInMatrix + 2; // +2 is for the border
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

void DrawHorizonLine(Selected A, Selected B)
{
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

// Draw U shape arrow with 2 given selected nodes
// void drawU(Selected &selected1, Selected &selected2, char *side)
// {
//     wchar_t *arrowLeft = L"→";
//     wprintf(L"%s", arrowLeft);
//     // Draw the arrow
//     if (strcmp(strlwr(side), "left"))
//     {
//         int posX1 = calculatePositionWidth(selected1.posX, EASY) + 5;
//         int posY1 = calculatePositionHeight(selected1.posY, EASY) + 2;
//         int posX2 = calculatePositionWidth(selected2.posX, EASY) + 5;
//         int posY2 = calculatePositionHeight(selected2.posY, EASY) + 2;
//         GoTo(posX1, posY1);
//     }
// }

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
            DeleteCube(a, difficulty, A);
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
            DeleteCube(a, difficulty, A);
            A.posY = i;
        }
        return true;
    }

    return false;
}

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

int main(int argc, char **argv)
{
    _setmode(_fileno(stdout), _O_U16TEXT);

    char test[EASY][EASY]{
        {'A', ' ', 'A', 'D'},
        {'D', ' ', 'G', 'H'},
        {'I', 'C', 'K', 'P'},
        {'M', 'N', 'O', 'P'}};

    char **matrix = (char **)malloc(sizeof(char *) * EASY);
    for (int i = 0; i < EASY; i++)
    {
        matrix[i] = (char *)malloc(sizeof(char) * EASY);
    }
    for (int i = 0; i < EASY; i++)
    {
        for (int j = 0; j < EASY; j++)
        {
            matrix[i][j] = test[i][j];
        }
    }

    int difficulty = EASY;

    // Random matrix of char in pair
    DrawMatrix(matrix, difficulty);

    // Get the position of the word
    Selected first = {1, 0};
    Selected second = {1, 1};
    first.prepareSelected(second);

    checkLine(matrix, first, second, difficulty);

    first = {0, 0};
    second = {2, 0};
    first.prepareSelected(second);

    checkLine(matrix, first, second, difficulty);

    first = {3, 3};
    second = {3, 2};
    first.prepareSelected(second);

    checkLine(matrix, first, second, difficulty);

    getch();
    return 0;
}