#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct cut_rod_method_
{
	int *subproblem_perfect_price;
	int *subproblem_perfect_method;
}cut_rod_method;

int price[11] = {0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30};
cut_rod_method subproblem_method;

int max(int a, int b)
{
	return (a > b ? a : b);
}

int bottom_up_cut_rod(int price[], int length)
{
	int q = 0, i = 1, j = 1;
	int maxPrice;
	if(length <= 0)
		return 0;
	subproblem_method.subproblem_perfect_price = (int *)calloc(length + 1,2 * sizeof(int));
	if(subproblem_method.subproblem_perfect_price == NULL)
	{
		printf("calloc error\n");
		return -1;
	}
	subproblem_method.subproblem_perfect_method = subproblem_method.subproblem_perfect_price + length + 1;
	for(j = 1; j <= length; j++)
	{
		q = 0;
		for(i = 1; i <= j; i++)
		{
			if(i <= 11)
			{
				if(q < price[i] + subproblem_method.subproblem_perfect_price[j - i])
				{
					q = price[i] + subproblem_method.subproblem_perfect_price[j - i];
					subproblem_method.subproblem_perfect_method[j] = i;
				}
			}
			else
			{
				if(q < subproblem_method.subproblem_perfect_price[i] + subproblem_method.subproblem_perfect_price[j - i])
				{
					q =subproblem_method.subproblem_perfect_price[i] + subproblem_method.subproblem_perfect_price[j - i];
					subproblem_method.subproblem_perfect_method[j] = i;
				}
			}
		}
		subproblem_method.subproblem_perfect_price[j] = q;
	}
	maxPrice = subproblem_method.subproblem_perfect_price[length];
	return maxPrice;
}

int main(int argc, char **argv)
{
	int n = 0;
	if(argc != 2)
	{
		printf("usage: %s rodLength\n", argv[0]);
		return -1;
	}
	printf("best: %d\n", bottom_up_cut_rod(price, atoi(argv[1])));
	n = atoi(argv[1]);
	while(n > 0)
	{
		printf("%d ", subproblem_method.subproblem_perfect_method[n]);
		n = n - subproblem_method.subproblem_perfect_method[n];
	}
	printf("\n");
	return 0;
}








