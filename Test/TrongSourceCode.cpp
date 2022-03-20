#include "stdio.h"
#include "stdlib.h"
#include "conio.h"
#include "iostream"
#include "wchar.h"
#include <locale>
using namespace std;

bool check_line_col(Pokemon** A, int col, int row1, int row2) // hang doc
{
	int row_max = max(row1, row2); 
	int row_min = min(row1, row2); 
	for (int i = row_min +1 ; i <= row_max -1 ; i++)
	{
		if (A[i][col].name != 32)
			return false; 
	}
	return true; 
}

bool check_line_row(Pokemon** A, int row, int col1, int col2) // hang ngang
{
	int col_max = max(col1, col2); 
	int col_min = min(col1, col2); 
	for (int i = col_min+1 ; i <= col_max -1 ; i++)
	{
		if (A[row][i].name != 32)
			return false;
	}
	return true;
} 

bool check_Z_col(Pokemon** A, point p1, point p2)
{
	int col_max = max(p1.x, p2.x);
	int col_min = min(p1.x, p2.x);
	for (int i = col_min+1; i <= col_max-1; i++)
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

bool check_Z_row(Pokemon** A, point p1, point p2)
{
	int row_max = max(p1.y, p2.y); 
	int row_min = min(p1.y, p2.y);
	for (int i = row_min+1 ; i <= row_max -1 ; i++)
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
}// tu tu fix sau 

bool check_U_col(Pokemon** A, int row, int col, point p1, point p2)
{

	for (int i = 0; i < col; i++)
	{
		if (check_line_col(A, i, p1.y, p2.y))
		{
			if (check_line_row(A, p1.y, p1.x, i) && check_line_row(A, p2.y, p2.x, i))
				return true; 
		}
	}
	//
	if (p1.x == p2.x && p1.x == 0)
		return true;
	if (p1.x == p2.x && p1.x == col - 1)
		return true;
	return false;
}

bool check_U_row (Pokemon** A, int row ,int col , point p1, point p2)
{
	for (int i = 0; i < row; i++)
	{
		if (check_line_row(A, i, p1.x, p2.x))
		{
			if (check_line_col(A, p1.x, p1.y, i) && check_line_col(A, p2.x, p2.y, i))
				return true;
		}
	}
	if (p1.y == p2.y && p1.y == 0)
		return true; 
	if (p1.y == p2.y && p1.y == row - 1)
		return true;
	
	return false;
}