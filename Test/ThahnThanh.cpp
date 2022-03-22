#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <ctime>

struct point
{
    int x = 0;
    int y = 0;
};
struct Pokemon
{
    point vitri{};
    char name = 0;
};
using namespace std;
HANDLE hStdin;
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;
#define BLACK 0
#define AQUA 3
#define RED 4
#define WHITE 7
#define GRAY 8

/*#define ARROW_UP 0x48
#define ARROW_LEFT 0x4B
#define ARROW_RIGHT 0x4D
#define ARROW_DOWN 0x50
#define ARROW_NONE 0x00*/

#define ESC_KEY 0x1B
#define ENTER_KEY 0x00
void gotoXY(int x, int y) // internet
{
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}
void SetColor(int backgound_color, int text_color)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    int color_code = backgound_color * 16 + text_color;
    SetConsoleTextAttribute(hStdout, color_code);
}
unsigned char GetArrow()
{
    unsigned char ch = _getch();
    switch (ch)
    {
    case 'w':
        return ch;
    case 'a':
        return ch;
    case 'd':
        return ch;
    case 's':
        return ch;
    default:
        return 0;
    }
    if (ch == 32 || ch == 27)
    {
        return ch;
    }
    return ch;
}

char move(Pokemon **A, int row, int col, int &posX, int &posY)
{
    char m = 0;
    m = _getch();
    /*int tempX, tempY;
    int cnt = 0;*/
    static int posXTemp;
    static int posYTemp;

    // If user press ENTER_KEY, set color for text and background
    if (m == 32)
    {
        int tempX = 10 + posX * 8;
        int tempY = 10 + posY * 4;

        gotoXY(tempX, tempY);

        SetColor(RED, WHITE);
        cout << A[posY][posX].name;

        posX = posXTemp;
        posY = posYTemp;

        SetColor(BLACK, WHITE);
        return m;
    }

    SetColor(BLACK, WHITE);
    gotoXY(10 + posX * 8, 10 + posY * 4);
    cout << A[posY][posX].name;
    // tempX = 10 + posX * 8;
    // tempY = 10 + posY * 4;*/

    if (m == 'd')
    {
        posXTemp = posX;
        posYTemp = posY;
        posX += 1;
        if (posX >= col)
        {
            posX -= 1;
            return m;
        }
        gotoXY(10 + posX * 8, 10 + posY * 4);
        SetColor(WHITE, BLACK);
        cout << A[posY][posX].name;
        SetColor(BLACK, WHITE);
        /*gotoXY(10 + (posX - 1) * 8, 10 + (posY - 1) * 4);
        SetColor(WHITE, BLACK);
        cout << A[posY-1][posX-1].name;
        SetColor(BLACK, WHITE);*/
        return m;
    }
    if (m == 'a')
    {
        posXTemp = posX;
        posYTemp = posY;
        posX -= 1;
        if (posX < 0)
        {
            posX += 1;
            return m;
        }
        gotoXY(10 + posX * 8, 10 + posY * 4);
        SetColor(WHITE, BLACK);
        cout << A[posY][posX].name;
        SetColor(BLACK, WHITE);
        return m;
    }
    if (m == 's')
    {
        posXTemp = posX;
        posYTemp = posY;
        posY += 1;
        if (posY >= row)
        {
            posY -= 1;
            return m;
        }

        gotoXY(10 + posX * 8, 10 + posY * 4);
        SetColor(WHITE, BLACK);
        cout << A[posY][posX].name;
        SetColor(BLACK, WHITE);
        return m;
    }
    if (m == 'w')
    {
        posXTemp = posX;
        posYTemp = posY;
        posY -= 1;
        if (posY < 0)
        {
            posY += 1;
            return m;
        }
        gotoXY(10 + posX * 8, 10 + posY * 4);
        SetColor(WHITE, BLACK);
        cout << A[posY][posX].name;
        SetColor(BLACK, WHITE);
        return m;
    }

    /*if (m == 32) {
        cnt++;
        gotoXY(10 + posX*8, 10 + posY*4);
        SetColor(RED, WHITE);
        cout << A[posY][posX].name;
        SetColor(BLACK, WHITE);
        while (cnt==1 || cnt==2)
        {
            gotoXY(tempX, tempY);
            SetColor(RED, WHITE);
            m = move(A, row, col, posX, posY);
        }
        if (cnt > 2) { cnt = 0; }


        return m;
    }*/

    return m;
}

Pokemon **KhoiTao(int &row, int &col)
{
    while (1)
    {
        cout << "Nhap so dong: ";
        cin >> row;
        cout << "Nhap so cot: ";
        cin >> col;
        if ((row * col) % 2 == 0)
            break;
        else
            cout << "Nhap lai: " << endl;
    }
    srand((int)time(0));
    Pokemon **A = new Pokemon *[row];
    for (int i = 0; i < row; i++)
    {
        A[i] = new Pokemon[col];
    }
    int count = 0;
    for (char i = 'A';; i++)
    {
        int d, c;
        while (true)
        {
            d = rand() % row;
            c = rand() % col;
            if (A[d][c].name == 0)
            {
                A[d][c].name = i;
                count++;
                break;
            }
        }
        while (true)
        {
            d = rand() % row; // random tu 1 -> row -1
            c = rand() % col;
            if (A[d][c].name == 0)
            {
                A[d][c].name = i;
                count++;
                break;
            }
        }
        if (count == row * col)
            break;
    }
    return A;
}
void output_matrix(Pokemon **A, int row, int col)
{
    int x = 0;
    int y = 0;
    system("cls");
    x = 10;
    y = 10;
    gotoXY(x, y);
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            cout << A[i][j].name;
            x += 8;
            gotoXY(x, y);
        }
        x = 10;
        y += 4;
        gotoXY(x, y);
    }
    x = 6, y = 8;
    gotoXY(x, y);
    int count = 0;
    for (int i = 0; i < row + 1; i++)
    {
        for (int j = 0; j < col * 8 + 1; j++)
        {
            cout << "-";
        }
        y += 4;
        gotoXY(x, y);
    }
    x = 6, y = 9;
    x = 6, y = 8;
    gotoXY(x, y);
    int cnt = 0;
    for (int i = 0; i < row * 8 + 1; i++)
    {
        for (int j = 0; j <= col * 8 + 1; j++)
        {
            cout << "|";
            cnt++;
            x += 8;
            gotoXY(x, y);
            if (cnt == col + 1)
                break;
        }
        if (i == row * 4)
            break;
        cnt = 0;
        x = 6;
        y += 1;
        gotoXY(x, y);
    }
    x = 6, y = 9;
}
void nhapthaotac(point &T, int row, int col, int posX, int posY)
{
    while (1)
    {
        T.x = posX;
        T.y = posY;
        if (T.x >= row || T.y >= col)
            cout << "Nhap lai: ";
        else
            break;
    }
}
bool check_line_col(Pokemon **A, int col, int row1, int row2) // hang doc
{

    int row_max = max(row1, row2);
    int row_min = min(row1, row2);
    for (int i = row_min; i <= row_max; i++)
    {
        if (A[i][col].name != ' ')
            return false;
    }
    return true;
}
bool check_line_row(Pokemon **A, int row, int col1, int col2) // hang ngang
{
    int col_max = max(col1, col2);
    int col_min = min(col1, col2);
    for (int i = col_min; i <= col_max; i++)
    {
        if (A[row][i].name != ' ')
            return false;
    }
    return true;
}
bool check_Z_col(Pokemon **A, point p1, point p2)
{
    int col_max = max(p1.x, p2.x);
    int col_min = min(p1.x, p2.x);
    for (int i = col_min + 1; i <= col_max - 1; i++)
    {
        if (check_line_col(A, i, p1.y, p2.y))
        {
            if (check_line_row(A, p1.y, p1.x, i) == 0 || check_line_row(A, p2.y, p2.x, i) == 0)
                return false;
            else
                return true;
        }
    }
    return false;
}
bool check_Z_row(Pokemon **A, point p1, point p2)
{

    int row_max = max(p1.y, p2.y);
    int row_min = min(p1.y, p2.y);
    for (int i = row_min + 1; i <= row_max - 1; i++)
    {
        if (check_line_row(A, i, p1.x, p2.x))
        {
            if (check_line_col(A, p1.x, p1.y, i) == 0 || check_line_col(A, p2.x, p2.y, i) == 0)
                return false;
            else
                return true;
        }
    }
    return false;
}
bool check_U_col(Pokemon **A, int row, int col, point p1, point p2)
{
    if (p1.y == p2.y && p1.y == 0)
        return true;
    if (p1.y == p2.y && p1.y == row - 1)
        return true;
    if ((check_line_row(A, p1.y, p1.x - 1, 0) && check_line_row(A, p2.y, p2.x - 1, 0)))
        return true;
    if ((check_line_row(A, p1.y, p1.x + 1, col - 1) && check_line_row(A, p2.y, p2.x + 1, col - 1)))
        return true;
    for (int i = 0; i < col; i++)
    {
        if (check_line_col(A, i, p1.y, p2.y))
        {
            int c1 = 0;
            int c2 = 0;
            if (p1.x > i)
            {
                c1 = -1;
            }
            if (p1.x < i)
            {
                c1 = 1;
            }
            if (p2.x > i)
            {
                c2 = -1;
            }
            if (p2.x < i)
            {
                c2 = 1;
            }
            if ((check_line_row(A, p1.y, p1.x + c1, i) && check_line_row(A, p2.y, p2.x + c2, i)))
                return true;
        }
    }
    return false;
}
bool check_U_row(Pokemon **A, int row, int col, point p1, point p2)
{
    if (p1.x == p2.x && p1.x == 0)
        return true;
    if (p1.x == p2.x && p1.x == col - 1)
        return true;
    if ((check_line_col(A, p1.x, p1.y - 1, 0) && check_line_col(A, p2.x, p2.y - 1, 0)))
        return true;
    if ((check_line_col(A, p1.x, p1.y + 1, row - 1) && check_line_col(A, p2.x, p2.y + 1, row - 1)))
        return true;

    for (int i = 0; i < row; i++)
    {
        int y_min = min(p1.y, p2.y);
        int y_max = max(p1.y, p2.y);
        if (i == y_min) // y : row
        {
            int x_min = min(p1.x, p2.x);
            int x_max = max(p1.x, p2.x);
            if (check_line_row(A, i, x_min + 1, x_max) && check_line_col(A, x_max, y_max - 1, i))
                return true;
            if (check_line_row(A, i, x_min, x_max - 1) && check_line_col(A, x_min, i, y_max - 1))
                return true;
        }
        if (i == y_max)
        {
            int x_min = min(p1.x, p2.x);
            int x_max = max(p1.x, p2.x);
            if (check_line_row(A, i, x_min + 1, x_max) && check_line_col(A, x_max, y_min + 1, i))
                return true;
            if (check_line_row(A, i, x_min, x_max - 1) && check_line_col(A, x_min, i, y_min + 1))
                return true;
        }
        if (check_line_row(A, i, p1.x, p2.x))
        {
            int c1 = 0;
            int c2 = 0;
            if (p1.y > i)
            {
                c1 = -1;
            }
            if (p1.y < i)
            {
                c1 = 1;
            }
            if (p2.y > i)
            {
                c2 = -1;
            }
            if (p2.y < i)
            {
                c2 = 1;
            }
            if ((check_line_col(A, p1.x, p1.y + c1, i) && check_line_col(A, p2.x, p2.y + c2, i)))
                return true;
        }
    }
    return false;
}
bool is_score(Pokemon **A, int row, int col, point T1, point T2)
{
    if (A[T1.y][T1.x].name != A[T2.y][T2.x].name)
        return false;
    if (T1.x == T2.x)
    {
        int maxA = max(T1.y, T2.y);
        int minA = min(T1.y, T2.y);
        if (check_line_col(A, T1.x, minA + 1, maxA - 1) || maxA - minA == 1)
        {
            return true;
        }
        else if (check_U_col(A, row, col, T1, T2))
            return true;
    }
    if (T1.y == T2.y)
    {
        int maxA = max(T1.x, T2.x);
        int minA = min(T1.x, T2.x);
        if (check_line_row(A, T1.y, minA + 1, maxA - 1) || maxA - minA == 1)
            return true;
        else if (check_U_row(A, row, col, T1, T2))
            return true;
    }
    if (check_U_row(A, row, col, T1, T2) || check_U_col(A, row, col, T1, T2))
    {
        return true;
    }
    return false;
}
void play(Pokemon **A, int row, int col, int &posX, int &posY, int &tempX, int &tempY)
{
    point T1, T2; // 2 2 // 2 3
    char key = 0;

    output_matrix(A, row, col);
    gotoXY(10, 10);
    SetColor(WHITE, BLACK);
    cout << A[0][0].name;
    SetColor(BLACK, WHITE);
    char m = 0;
    int count = 0;
    while (1)
    {
        m = move(A, row, col, posX, posY);
        if (m == 27)
            break;
        if (m == 32) // spaxe
        {

            count++;

            if (count == 1)
            {
                nhapthaotac(T1, row, col, posX, posY);
            }

            if (count == 2)
            {
                nhapthaotac(T2, row, col, posX, posY);

                if (T1.x != T2.x || T1.y != T2.y) // T1 != T2
                {
                    if (is_score(A, row, col, T1, T2))
                    {
                        system("pause");
                        A[T1.y][T1.x].name = ' ';
                        A[T2.y][T2.x].name = ' ';
                        system("cls");
                        output_matrix(A, row, col);
                    }
                }
                T1.x = 0;
                T2.x = 0;
                T1.y = 0;
                T2.y = 0;
                count = 0;
            }
        }
    }
}
int main()
{
    int row, col;
    point T;
    int posX = 0;
    int posY = 0;
    int tempX, tempY;

    Pokemon **A = KhoiTao(row, col);

    output_matrix(A, row, col);
    play(A, row, col, posX, posY, tempX, tempY);
    // unsigned char cha;
    // cha = GetArrow();
    // printf("%02X", cha);
    for (int i = 0; i < row; i++)
    {
        delete[] A[i];
    }
    delete[] A;
    return 0;
}