#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include "conio.h"

#define EASY 4   // Matrix size is 4x4
#define MEDIUM 6 // Matrix size is 6x6
#define HARD 8   // Matrix size is 8x8

#define GOLEFT(x) (x--)
#define GORIGHT(x) (x++)
#define GOUP(y) (y--)
#define GODOWN(y) (y++)

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

        // If this.x is lower than other.x, and this.y is higher than other.y, swap their position
        else if (posX > other.posX && posY < other.posY)
        {
            swap(other);
        }

        // If this.y is lower than other.y, swap their position
        else if (posY > other.posY)
        {
            swap(other);
        }

        // *This is exception*
        // If this.x is lower than other.x, and this.y is lower than other.y, swap their position
        else if (posX < other.posX && posY < other.posY)
        {
            swap(other);
        }
    }
};

bool findIShape(char **matrix, int difficulty, Selected A, Selected B)
{
    if (matrix[A.posX][A.posY] == matrix[B.posX][B.posY])
    {
        if (A.posX == B.posX)
        {
            // Check if the any "\0" character is in the same row
            for (int i = A.posY; i < B.posY; i++)
            {
                if (matrix[i][A.posY] == '\0')
                {
                    return false;
                }
            }
            return true;
        }
        if (A.posY == B.posY)
        {
            // Check if the any "\0" character is in the same column
            for (int i = A.posY; i < B.posY; i++)
            {
                if (matrix[A.posX][i] == '\0')
                {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

int main(int argc, char **argv)
{
    char test[EASY][EASY]{
        {'A', 'A', 'C', 'D'},
        {'E', 'F', 'G', 'H'},
        {'I', 'J', 'K', 'L'},
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

    Selected A = {1, 1}, B = {0, 1};
    A.prepareSelected(B);

    // Print the Selected node
    printf("A: (%d, %d)\n", A.posX, A.posY);
    printf("B: (%d, %d)\n", B.posX, B.posY);

    if (findIShape(matrix, EASY, A, B))
    {
        printf("Found I Shape\n");
    }
    else
    {
        printf("Not Found I Shape\n");
    }

    getch();
    return 0;
}