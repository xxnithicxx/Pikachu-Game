#include "stdio.h"
#include "conio.h"
#include "windows.h"
#include "iostream"

#define ARROW_UP 0x48
#define ARROW_LEFT 0x4B
#define ARROW_RIGHT 0x4D
#define ARROW_DOWN 0x50
#define ARROW_NONE 0x00
#define ESC_KEY 0x1B
#define ENTER_KEY 0x0D

#define CLEAR_KEYBUFFER         \
    while ((getchar()) != '\n') \
        ;

using namespace std;

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

int main(int argc, char **argv)
{
    unsigned char ch;
    while ((ch = GetArrow()) != ESC_KEY)
    {
        printf("%02X : %c\n", ch, ch);
    }

    // SHORT i = -1 ;
    // printf("%d\n", i);
    // cout << "Character: \u092E\n";

    getch();
    return 0;
}