#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int memoized_cut_rod_aux(int price[], int length, int *subproblem_method);
int price[] = {0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30};

int max(int a, int b)
{
	return (a > b ? a : b);
}

int memoized_cut_rod(int price[], int length)
{
	if(length <= 0)
	{
		return 0;
	}
	int *subproblem_method = (int *)calloc(length + 1, sizeof(int));
	if(subproblem_method == NULL)
	{
		printf("calloc error\n");
		return -1;
	}
	int maxPrice = memoized_cut_rod_aux(price, length, subproblem_method);
	free(subproblem_method);
	subproblem_method = NULL;
	return maxPrice;
}

int memoized_cut_rod_aux(int price[], int length, int *subproblem_method)
{
	int q = 0, i = 0;
	if(subproblem_method[length] > 0)
	{
		return subproblem_method[length];
	}
	for(i = 1; i <= length; i++)
	{
		q = max(q, price[i] + memoized_cut_rod_aux(price, length - i, subproblem_method));
	}
	subproblem_method[length] = q;
	return q;
}

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("usage: %s rodLength\n", argv[0]);
		return -1;
	}
	printf("best: %d\n", memoized_cut_rod(price, atoi(argv[1])));
	return 0;
}








