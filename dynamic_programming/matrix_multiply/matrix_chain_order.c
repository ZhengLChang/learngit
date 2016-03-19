#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct chain_optimal_order_
{
	int **m;
	int m_length;
	int m_width;

	int **s;
	int s_length;
	int s_width;
}chain_optimal_order;

chain_optimal_order *matrix_chain_order(int *price, int length);

int main(int argc, char **argv)
{
	int price[] = {30, 35, 15, 5,  10, 20, 25};
	int i = 0, j = 0;
	chain_optimal_order *order_p = matrix_chain_order(price, sizeof(price) / sizeof(int));
	
	for(i = 0; i <= order_p->m_length; i++)
	{
		for(j = 0; j <= order_p->m_width; j++)
			printf("%d\t", order_p->m[i][j]);	
		printf("\n");
		/*
		printf("%d\n", *(*order_p->m + 0 * (order_p->m_length) + order_p->m_length - 2));
		printf("%d\n", *(*order_p->m + i));
		*/
	}
	printf("--------------------------------------------\n");
	for(i = 0; i <= order_p->s_length; i++)
	{
		for(j = 0; j <= order_p->s_width; j++)
		{
			printf("%d\t", order_p->s[i][j]);
		}
		printf("\n");
	}
	return 0;
}

chain_optimal_order *matrix_chain_order(int *price, int length)
{
	chain_optimal_order *order_p = NULL;
	int i, l, j, k; 
	int q;
	int **m, **s;
	if(length <= 0)
		return NULL;
	m = (int **)malloc((length + 1) * sizeof(int **));
	if(m == NULL)
	{
		printf("malloc error\n");
		return NULL;
	}
	for(i = 0; i <= length; i++)
	{
		m[i] = (int *)calloc(length, sizeof(int *));
		if(m[i] == NULL)
		{
			printf("calloc error\n");
			return NULL;
		}
	}
	s = (int **)malloc((length) * sizeof(int **));
	if(s == NULL)
	{
		printf("malloc error\n");
		return NULL;
	}
	for(i = 0; i < length; i++)
	{
		s[i] = (int *)calloc(length, sizeof(int *));
		if(s[i] == NULL)
		{
			printf("calloc error\n");
			return NULL;
		}
	}

	for(l = 2; l <= length; l++)
	{
		for(i = 1; i < length - l + 1; i++)
		{
			j = i + l - 1;
			for(k = i; k <= j - 1; k++)
			{
				q = m[i][k] + m[k + 1][j] + price[i - 1]*price[k]*price[j];
				if(q < m[i][j] || m[i][j] == 0)
				{
					m[i][j] = q;
					s[i][j] = k;
				}
			}
		}
	}
	order_p = (chain_optimal_order *) calloc(1, sizeof(chain_optimal_order *));
	if(order_p == NULL)
	{
		printf("calloc error\n");
		return NULL;
	}
	order_p->m = m;
	order_p->s = s;
	order_p->m_length = length;
	order_p->m_width = length;
	order_p->s_length = length - 1;
	order_p->s_width = length - 1;
	return order_p;
}





















