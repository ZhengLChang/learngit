#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int lcs_length(char *x, int x_length, char *y, int y_length);

int main(int argc, char **argv)
{
	char x[] = "ABCBDA";
	char y[] = "BDCABA";
	printf("%d\n", lcs_length(x, 6, y, 6));
}

int lcs_length(char *x, int x_length, char *y, int y_length)
{
	char **b, **c;
	int i = 0, j = 0;
	if((b = (char **)malloc((x_length + 1) * sizeof(char **))) == NULL)
	{
		printf("malloc error: %s\n", strerror(errno));
		return -1;
	}
	for(i = 0; i < x_length + 1; i++)
	{
		if((b[i] = (char *)malloc((y_length + 1) * sizeof(char *))) == NULL)
		{
			printf("malloc error: %s\n", strerror(errno));
			return -1;
		}
	}
	if((c = (char **)malloc((x_length + 1) * sizeof(char **))) == NULL)
	{
		printf("malloc error: %s\n", strerror(errno));
		return -1;
	}
	for(i = 0; i < x_length + 1; i++)
	{
		if((c[i] = (char *)malloc((y_length + 1) * sizeof(char *))) == NULL)
		{
			printf("malloc error: %s\n", strerror(errno));
			return -1;
		}
	}
	for(i = 1; i <= x_length; i++)
	{
		c[i][0] = 0;
	}
	for(i = 0; i <= y_length; i++)
	{
		c[0][i] = 0;
	}
	for(i = 1; i <=x_length ;i++)
	{
		for(j = 1; j <= y_length; j++)
		{
			if(x[i] == y[j])
			{
				c[i][j] = c[i - 1][j - 1] + 1;
				b[i][j] = '\\';
			}
			else if(c[i - 1][j] >= c[i][j - 1])
			{
				c[i][j] = c[i - 1][j];
				b[i][j] = '|';
			}
			else
			{
				c[i][j] = c[i][j - 1];
				b[i][j] = '-';
			}
		}
	}
	return c[x_length][y_length];
}



















