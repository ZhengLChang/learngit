#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int **matrix_multiply(int *a, int a_row, int a_column,
		      int *b, int b_row, int b_column);

int main(int argc, char **argv)
{
	int a[2][3] = {{1, 2, 3}, {4, 5, 6}};
	int b[3][2] = {{1, 2}, {3, 4}, {5, 6}};
	int **c = matrix_multiply(*a, 2, 3, *b, 3, 2);
	if(c == NULL)
	{
		printf("matrix multiply error\n");
	}
	else
	{
		int i = 0, j = 0;
		for(i = 0; i < 2; i++)
		{
			for(j = 0; j < 2; j++)
			{
				printf("%d\t", c[i][j]);
			}
			printf("\n");
		}
	}
	return 0;
}

int **matrix_multiply(int *a, int a_row, int a_column,
		      int *b, int b_row, int b_column)
{
	int **c = NULL;
	int i, j, k;
	if(a == NULL || b == NULL || a_column < 0 || a_column != b_row)
	{
		return NULL;
	}
	c = (int **)calloc(a_row, sizeof(int **));
	if(c == NULL)
	{
		printf("calloc error: %s\n", strerror(errno));
		return NULL;
	}

	for(i = 0; i < a_row; i ++)
	{
		c[i] = (int *)calloc(b_column, sizeof(int *));
		if(c[i] == NULL)
		{
			printf("calloc error: %s\n", strerror(errno));
			return NULL;
		}
	}

	for(i = 0; i < a_row; i++)
	{
		for(j = 0; j < b_column; j++)
		{
			for(k = 0; k < b_row; k++)
			{
				c[i][j] += *(a + a_column * i + k) * (*(b +  b_column * k + j));
			}
		}
	}
	return c;
}

















