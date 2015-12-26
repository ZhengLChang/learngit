#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int price[] = {0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30};

int max(int a, int b)
{
	return (a > b ? a : b);
}

int cut_rod(int price[], int length)
{
	int q = 0, i = 0;
	if(length <= 0)
		return 0;
	for(i = 1; i <= length; i++)
	{
		q = max(q, price[i] + cut_rod(price, length - i));
	}
	return q;
}

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("usage: %s rodLength\n", argv[0]);
		return -1;
	}
	printf("best: %d\n", cut_rod(price, atoi(argv[1])));
	return 0;
}








