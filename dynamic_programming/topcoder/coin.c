#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int *min_coin(int money_value, int *money_type, int type_len)
{
	int i = 0, j = 0;
	int *min = (int *)calloc(money_value + 1, sizeof(int));
	if(min == NULL)
	{
		printf("calloc error: %s\n", strerror(errno));
		return 0;
	}
	min[0] = 0;
	for(i = 1; i <= money_value; i++)
	{
		for(j = 0; j < type_len; j++)
		{
			if(min[i] == 0)
			{
				min[i] = min[i - money_type[j]] + 1;
			}
			else if(i >= money_type[j] && min[i] > min[i - money_type[j]] + 1)
			{
				min[i] = min[i - money_type[j]] + 1;
			}
		}
	}
	return min;
}

int main(int argc, char **argv)
{
	int money_type[] = {1, 3, 5};
	int len = sizeof(money_type) / sizeof(int);
	int *min = min_coin(11, money_type, len);
	int i;
	if(min != NULL)
	{
		for(i = 0; i <= 11; i++)
		{
			printf("%d\t", min[i]);
		}
		printf("\n");
		free(min);
		min = NULL;
	}
	return 0;
}










