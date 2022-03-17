#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define EASY 4     // Matrix size is 4x4
#define MEDIUM 6   // Matrix size is 6x6
#define HARD 8     // Matrix size is 8x8

// void swap(auto &a, auto &b) { a = b; }

void createMatrixPikachu(char **&a, int difficulty)
{
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
        arrPair[i] = i + 'A';
    }

    for (int i = 0; i < difficulty; i++)
    {
        for (int j = 0; j < difficulty; j++)
        {
            int posInPairArray = rand() % (sizeOfPair + 1);
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
            printf("%c ", a[i][j]);
        }
        printf("\n");
    }
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
    createMatrixPikachu(matrix, difficulty);
    printMatrix(matrix, difficulty);
    releaseMatrix(matrix, difficulty);
    return 0;
}