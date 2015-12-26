#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int price[] = {0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30};
#define PRICERANGE 11
typedef struct perfectSolution_
{
	int length;
	int *selectedPrice;
	int *selectedSolution;
}perfectSolution_s;

perfectSolution_s ourSolution;

perfectSolution_s *init_perfectSolution(perfectSolution_s *ourSolution, const int length)
{
	if(ourSolution == NULL || length == 0)
		return NULL;
	ourSolution->selectedPrice = (int *)calloc(2 * (length + 1), sizeof(int));
	if(ourSolution->selectedPrice == NULL)
	{
		return NULL;
	}
	ourSolution->length = length + 1;
	ourSolution->selectedSolution = ourSolution->selectedPrice + length + 1;
	return ourSolution;
}

int free_perfectSolution(perfectSolution_s *ourSolution)
{
	if(ourSolution != NULL || ourSolution->selectedPrice != NULL)
	{
		free(ourSolution->selectedPrice);
		ourSolution->length = 0;
		ourSolution->selectedPrice = NULL;
		ourSolution->selectedSolution = NULL;
		return 0;
	}
	return -1;
}

int cut_rod(int length, int *price,
			perfectSolution_s *ourSolution);

int cut_rod_proc(int length, int *price,
			perfectSolution_s *ourSolution)
{
	if(price == NULL || ourSolution == NULL || length <= 0)
	{
		return 0;
	}
	init_perfectSolution(ourSolution, length);
	return cut_rod(length, price, ourSolution);
}

int cut_rod(int length, int *price,
			perfectSolution_s *ourSolution)
{
	int i, j, max;
	if(price == NULL || ourSolution == NULL || length <= 0)
	{
		return 0;
	}
	for(j = 1; j <= length; j++)
	{
		max = 0;
		for(i = 1; i <= j && i <= PRICERANGE; i++)
		{
			if(max < price[i] + ourSolution->selectedPrice[j - i])
			{
				max = price[i] + ourSolution->selectedPrice[j - i];
				ourSolution->selectedSolution[j] = i;
				ourSolution->selectedPrice[j] = max;
			}
		}
	}
	return ourSolution->selectedPrice[length];
}

int main(int argc, char **argv)
{
	int n;
	int length;
	if(argc != 2)
	{
		printf("usage: %s rodLength\n", argv[0]);
	}
	length = atoi(argv[1]);
	printf("length = %d\n", length);
	printf("range = %d\n", PRICERANGE);
	if(cut_rod_proc(length, price, &ourSolution) == 0)
	{
		printf("error length\n");
		return 0;
	}
	printf("perfect price: %d\n", ourSolution.selectedPrice[length]);
	printf("perfect solution:\n");
	for(n = length; n > 0; n = n - ourSolution.selectedSolution[n])
	{
		printf("%d\t", ourSolution.selectedSolution[n]);
	}
	printf("\n");
	free_perfectSolution(&ourSolution);
	return 0;
}




















