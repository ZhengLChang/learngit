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
	char **p =(char **)&d[1][1];
	for(i = 0; i < s_size; i++)
		d[i][0] = i;
	for(j = 0; j < n_size; j++)
		d[0][j] = j;
	for(j = 0; j < n_size; j++)
		for(i = 0; i < s_size; i++)
			if(s[i] == n[j])
				p[i][j] = p[i - 1][j - 1];
			else
				p[i][j] = minimum3(p[i - 1][j] + 1,
						   p[i][j - 1] + 1,
						   p[i - 1][j - 1] + 1); 
	return p[s_size - 1][n_size - 1];
}
