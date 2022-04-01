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
        if (posX < other.posX && posY > other.posY) // if ⬋ vector change it to ⬈ vector
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
            return;
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
inline int calculatePositionWidth(int posInMatrix, int difficulty)
{
    return (WORD_WIDTH_SPACING + 1) * posInMatrix + 8; // +8 is for the border
}

inline int calculatePositionHeight(int posInMatrix, int difficulty)
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

bool checkUShape(char **a, Selected A, Selected B, int difficulty)
{
    Selected C;
    Selected D;

    A.prepareSelected(B);

    if (checkHorizontalU(a, A, B, difficulty, C, D))
    {
        DrawHorizonLine(A, C);
        DrawHorizonLine(B, D);
        DrawVerticalLine(C, D);
        GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 3);
        wprintf(L"This is Z shape\n");

        Sleep(1000);

        // Exception: if A.posY is higher than B.posY ("⬈" vector)
        if (A.posY > B.posY && A.posX < B.posX)
        {
            C.swap(D);
        }

        SHORT minX = min(A.posX, C.posX);
        SHORT maxX = max(A.posX, C.posX);

        for (SHORT i = minX + 1; i < maxX; i++)
        {
            DeleteCude(a, difficulty, Selected{i, A.posY});
        }

        minX = min(B.posX, D.posX);
        maxX = max(B.posX, D.posX);

        for (SHORT i = minX + 1; i < maxX; i++)
        {
            DeleteCude(a, difficulty, Selected{i, B.posY});
        }

        DeleteCude(a, difficulty, A);
        DeleteCude(a, difficulty, B);

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
                DeleteCude(a, difficulty, C);
            }
        }

        a[A.posY][A.posX] = ' ';
        a[B.posY][B.posX] = ' ';

        return true;
    }

    if (checkVerticalU(a, A, B, difficulty, C, D))
    {
        DrawVerticalLine(A, C);
        DrawVerticalLine(B, D);
        DrawHorizonLine(C, D);
        GoTo(WORD_WIDTH_SPACING, calculatePositionHeight(difficulty, difficulty) + 3);
        wprintf(L"This is Z shape\n");

        Sleep(1000);

        SHORT minY = min(A.posY, C.posY);
        SHORT maxY = max(A.posY, C.posY);

        for (SHORT i = minY + 1; i < maxY; i++)
        {
            DeleteCude(a, difficulty, Selected{A.posX, i});
        }

        minY = min(B.posY, D.posY);
        maxY = max(B.posY, D.posY);

        for (SHORT i = minY + 1; i < maxY; i++)
        {
            DeleteCude(a, difficulty, Selected{B.posX, i});
        }

        DeleteCude(a, difficulty, A);
        DeleteCude(a, difficulty, B);

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
                DeleteCude(a, difficulty, C);
            }
        }

        a[A.posY][A.posX] = ' ';
        a[B.posY][B.posX] = ' ';

        return true;
    }
    return false;
}

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

int main(int argc, char **argv)
{
    _setmode(_fileno(stdout), _O_U16TEXT);

    char test[EASY][EASY]{
        {'X', ' ', 'X', ' '},
        {' ', 'S', 'T', 'S'},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'S'}};

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
    drawMatrix(matrix, difficulty);
    DrawBorder(difficulty);

    // Get the position of the word
    Selected first = {0, 0};
    // Selected second = {0, 0};
    // Selected first = {0, 2};
    Selected second = {3, 1};

    checkUShape(matrix, first, second, difficulty);

    GoTo(WORD_WIDTH_SPACING, 0);
    wprintf(L"Reprint matrix\n");
    Sleep(1000);
    // Reprint the matrix
    drawMatrix(matrix, difficulty);

    getch();
    return 0;
}