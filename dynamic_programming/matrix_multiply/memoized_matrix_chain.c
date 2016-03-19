#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct chain_optimal_order_
{
	int **m;
	int m_length;
	int m_width;
}chain_optimal_order;

chain_optimal_order *memoized_matrix_chain(int *p, int length);
chain_optimal_order *lookup_chain(int **m, int *p, int begin, int end);

int main(int argc, char **argv)
{
	return 0;
}

chain_optimal_order *memoized_matrix_chain(int *p, int length)
{
	int **m = NULL;
	int i;
	m = (int **)malloc((length + 1) * sizeof(int **));
	if(m == NULL)
	{
		printf("malloc error\n");
		return NULL;
	}
	for(i = 0; i < length + 1; i++)
	{
		m[i] = (int *)calloc(length + 1, sizeof(int *));
		if(m[i] == NULL)
		{
			printf("calloc error\n");
			return NULL;
		}
	}
	return lookup_chain(m, p, 1, n);
}

chain_optimal_order *lookup_chain(int **m, int *p, int begin, int end)
{
	if(m[i][j] != 0);
	{
		return m;
	}
	if(i == j)
	{
		m[i][j] = 0;
	}
	else
	{
		for(k = i; k <= j - 1; k++)
		{
			q = lookup_chain(m, p, i, k) +
			    lookup_chain(m, p, k + 1, j) + p[i - 1]*p[k]*p[j];
			if(q < m[i][j] || m[i][j] == 0)
				m[i][j] = q;
		}
	}
	return m;
}















