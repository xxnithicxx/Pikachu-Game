#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <conio.h> // For keyboard input (getch())
#include <dos.h> // For delay(ms)
#include <windows.h>
#include <string>
using namespace std;

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
#define WHITE 7
#define GRAY 8

#define WORD_WIDTH_SPACING 2
#define WORD_HEIGHT_SPACING 0

// Set windows specific size scale with difficulty
// Ref: https://codelearn.io/sharing/windowsh-va-ham-dinh-dang-console-p1

struct Selected
{
    SHORT posX;
    SHORT posY;

    bool isSelected;

    bool operator != (Selected &other)
    {
        return posX != other.posX || posY != other.posY;
    }
};

void SetWindowSize(int difficulty)
{
    // SHORT is the type of variable in WINAPI
    SHORT width;  // Width of console
    SHORT height; // Height of console

    switch (difficulty)
    {
    case EASY:
        system("Mode con: cols=40 lines=20");
        break;
    case MEDIUM:
        system("Mode con: cols=60 lines=30");
        break;
    case HARD:
        system("Mode con: cols=80 lines=40");
        break;
    }

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
    if (kbhit()) // Check if anykey is press
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
    return ARROW_NONE;
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

// Print the matrix for testing
void printMatrix(char **a, int difficulty)
{
    for (int i = 0; i < difficulty; i++)
    {
        for (int j = 0; j < difficulty; j++)
        {
            printf(" %c ", a[i][j]);
        }
        printf("\n");
    }
}

// Menu for the difficulty
char menuDifficulty()
{
    printf("Difficulty: \n");
    return getch();
}

// Menu for the game

// Check if the matrix is solved

// Select the position of the character in the matrix

// Restore the Node to previous state
void restoreNode(Selected &node, char **a, int difficulty)
{
    // Restore the node to previous state
    GoTo(node.posX + node.posX * WORD_WIDTH_SPACING, node.posY + node.posY * WORD_HEIGHT_SPACING);
    SetColor(BLACK, WHITE);
    printf(" %c ", a[node.posY][node.posX]);
}

// Move the cursor to the position with matrix and check if the position is valid
bool moveToPosition(char **a, int difficulty, SHORT posX, SHORT posY)
{
    if (posX < 0 || posX >= difficulty || posY < 0 || posY >= difficulty)
    {
        return true;
    }

    GoTo(posX + posX * WORD_WIDTH_SPACING, posY + posY * WORD_HEIGHT_SPACING);
    SetColor(WHITE, BLACK);
    printf(" %c ", a[posY][posX]);

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

int main(int argc, char **argv)
{
    char **matrix = NULL;
    int difficulty = HARD;

    system("cls");
    SetConsoleTitleW(L"Pikachu"); // Change console title (L is for Unicode)
    DisableResizeWindow();
    DisableMaximizeButton();
    DisableCur();
    HideScrollbar();

    createMatrixPikachu(matrix, difficulty);
    printMatrix(matrix, difficulty);

    // Clear keyboard buffer
    fflush(stdin);

    // Initialize the position of selected Node
    Selected firstNode = {0, 0, false};
    Selected secondNode = {0, 0, false};
    Selected tempNode = {0, 0, false};

    SHORT posX = 0, posY = 0;
    char ch = ENTER_KEY;
    while ((ch = GetArrow()) != ESC_KEY)
    {
        switch (ch)
        {
        case ARROW_UP:
            posY--;
            break;
        case ARROW_DOWN:
            posY++;
            break;
        case ARROW_LEFT:
            posX--;
            break;
        case ARROW_RIGHT:
            posX++;
            break;
        // case ENTER_KEY:
        //     if (!firstNode.isSelected)
        //     {
        //         GoTo(0, difficulty + difficulty * WORD_HEIGHT_SPACING);
        //         printf("First Node Selected\n");
        //         firstNode.isSelected = true;
        //         firstNode.posX = posX;
        //         firstNode.posY = posY;
        //     }
        //     else
        //     {
        //         GoTo(0, difficulty + difficulty * WORD_HEIGHT_SPACING);
        //         printf("Second Node Selected\n");
        //         secondNode.isSelected = true;
        //         secondNode.posX = posX;
        //         secondNode.posY = posY;
        //     }
        //     break;
        default:
            break;
        }

        // Restore the previous Node to original state
        // If the previous node is selected, don't restore it
        if (tempNode != firstNode || tempNode != secondNode || tempNode.isSelected)
        {
            restoreNode(tempNode, matrix, difficulty);
        }

        if (moveToPosition(matrix, difficulty, posX, posY))
        {
            GoTo(0, difficulty + difficulty * WORD_HEIGHT_SPACING);
            break;
        }

        tempNode.posX = posX;
        tempNode.posY = posY;
        tempNode.isSelected = true;

        // Wait for the user to press any key
        fflush(stdin);
        getch();
    }

    releaseMatrix(matrix, difficulty);

    system("pause");
    return 0;
}