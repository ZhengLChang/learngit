#include <editDistance.h>

static int minimum3(int a, int b, int c) 
{
	int minimum = (a < b ? a : b);
	return (minimum < c ? minimum : c);
}

int EditDistance(const char *s, const int s_size, 
			const char *n, const int n_size)
{
	int i, j;
	char d[s_size + 1][n_size + 1];
	for(i = 0; i < s_size + 1; i++)
		d[i][0] = i;
	for(j = 0; j < n_size + 1; j++)
		d[0][j] = j;
	for(j = 0; j < n_size; j++)
		for(i = 0; i < s_size; i++)
			if(s[i] == n[j])
				d[i + 1][j + 1] = d[i][j];
			else
				d[i + 1][j + 1] = minimum3(d[i][j + 1] + 1,
						   d[i + 1][j] + 1,
						   d[i][j] + 1); 
	return d[s_size][n_size];
}

void EditPath(const char *s, const int s_size, 
			const char *n, const int n_size)
{
	int i, j, miniNum = 0;
	char d[s_size + 1][n_size + 1];
	for(i = 0; i < s_size + 1; i++)
		d[i][0] = i;
	for(j = 0; j < n_size + 1; j++)
		d[0][j] = j;
	for(j = 0; j < n_size; j++)
		for(i = 0; i < s_size; i++)
		{
			if(s[i] == n[j])
			{
				d[i + 1][j + 1] = d[i][j];
				continue;
			}
			miniNum = minimum3(d[i][j + 1] + 1,
						d[i + 1][j] + 1,
						d[i][j] + 1); 
			d[i + 1][j + 1] = miniNum;
			if(i == j)
			if(miniNum == d[i][j + 1])
			{
				printf("remove %c\n", s[j]);
			}
			else if(miniNum == d[i + 1][j])
			{
				printf("remove %c\n", n[j]);
			}
			else if(miniNum == d[i][j])
			{
				printf("same the before\n");
			}
		}
}
