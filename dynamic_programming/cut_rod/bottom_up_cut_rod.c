#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int price[] = {0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30};

int max(int a, int b)
{
	return (a > b ? a : b);
}

int bottom_up_cut_rod(int price[], int length)
{
	int q = 0, i = 1, j = 1;
	int *subproblem_method, maxPrice;
	if(length <= 0)
		return 0;
	subproblem_method = (int *)calloc(length + 1, sizeof(int));
	if(subproblem_method == NULL)
	{
		printf("calloc error\n");
		return -1;
	}
	for(j = 1; j <= length; j++)
	{
		q = 0;
		for(i = 1; i <= j; i++)
		{
			q = max(q, price[i] + subproblem_method[j - i]);
		}
		subproblem_method[j] = q;
	}
	maxPrice = subproblem_method[length];
	free(subproblem_method);
	subproblem_method = NULL;
	return maxPrice;
}

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("usage: %s rodLength\n", argv[0]);
		return -1;
	}
	printf("best: %d\n", bottom_up_cut_rod(price, atoi(argv[1])));
	return 0;
}








