#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "windows.h"

void MoveWindow(int posx, int posy) 
{
    RECT rectClient, rectWindow;
    HWND hWnd = GetConsoleWindow();
    GetClientRect(hWnd, &rectClient);
    GetWindowRect(hWnd, &rectWindow);
    MoveWindow(hWnd, posx, posy, rectClient.right - rectClient.left, rectClient.bottom - rectClient.top, TRUE);
}

int main(int argc, char **argv)
{
    MoveWindow(0, 0);
    getchar();
    return 0;
}