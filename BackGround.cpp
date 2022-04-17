#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "iostream"
#include <windows.h>
#include "fstream"
using namespace std;

#define EASY 4   // Matrix size is 4x4
#define MEDIUM 6 // Matrix size is 6x6
#define HARD 8   // Matrix size is 8x8

#define WORD_WIDTH_SPACING 8
#define WORD_HEIGHT_SPACING 3

// Set cursor position
void GoTo(SHORT posX, SHORT posY)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD Position;
    Position.X = posX;
    Position.Y = posY;

    SetConsoleCursorPosition(hStdout, Position);
}

struct Selected
{
    SHORT posX;
    SHORT posY;

    bool isSelected;
};

struct BackGround
{
    string **BG;

    void GetBG(int difficulty);
    void PrintBG(char **a, int difficulty, Selected A);
};

// Calculate position of the word in the console
int calculatePositionWidth(int posInMatrix, int difficulty)
{
    return (WORD_WIDTH_SPACING + 1) * posInMatrix + 8; // +8 is for the border
}

int calculatePositionHeight(int posInMatrix, int difficulty)
{
    return (WORD_HEIGHT_SPACING + 1) * posInMatrix + 4; // +4 is for the border
}

void BackGround::GetBG(int difficulty)
{
    int sizeBG = difficulty * difficulty;
    BG = new string *[sizeBG];
    for (int i = 0; i < sizeBG; i++)
    {
        BG[i] = new string[5];
    }

    ifstream file;
    // Open 2.txt in Background folder
    file.open("./BackGround/1.txt");
    if (file.is_open())
    {
        string temp;
        for (int i = 0; i < difficulty; i++) // Number of row in matrix pikachu
        {
            for (int j = 0; j < 5; j++) // 5 is the width of the cube
            {
                getline(file, temp);
                for (int k = 0; k < difficulty; k++) // Number of column in matrix pikachu
                {
                    // Cut the string for 9 characters
                    BG[i + k * difficulty][j] += temp.substr(k * difficulty, k * difficulty + 9);
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

void BackGround::PrintBG(char **a, int difficulty, Selected A)
{
    int position = A.posY + A.posX * difficulty;
    int posY = calculatePositionHeight(A.posY, difficulty);
    int posX = calculatePositionWidth(A.posX, difficulty);
    for (int i = 0; i < 5; i++)
    {
        GoTo(posX, posY++);
        cout << BG[position][i];
    }
}

int main(int argc, char **argv)
{
    BackGround backGround;
    char **a = new char *[HARD];
    backGround.GetBG(HARD);

    // Print the background
    for (SHORT i = 0; i < HARD; i++)
    {
        for (SHORT j = 0; j < HARD; j++)
        {
            backGround.PrintBG(a, HARD, {i, j});
        }
    }

    system("pause");
    return 0;
}
