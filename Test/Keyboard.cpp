#include "stdio.h"
#include "conio.h"

#define ARROW_UP 0x48
#define ARROW_LEFT 0x4B
#define ARROW_RIGHT 0x4D
#define ARROW_DOWN 0x50
#define ARROW_NONE 0x00
#define ESC_KEY 0x1B
#define ENTER_KEY 0x0D

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

int main(int argc, char **argv)
{
    unsigned char ch;
    while ((ch = GetArrow()) != ESC_KEY)
    {
        fflush(stdin);
        printf("%02X\n", ch);
        if (ch == ARROW_NONE)
        {
            getch();
        }
    }
    return 0;
}