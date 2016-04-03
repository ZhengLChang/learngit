#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int lis(int *arr, int n)
{
	int len = 1, i, j;
	int *d = calloc(n, sizeof(int));
	if(d == NULL)
	{
		printf("calloc error: %s\n", strerror(errno));
		return -1;
	}
	for(i = 0; i < n; i++)
	{
		d[i] = 1;
		for(j = 0; j < i; j++)
		{
			if(arr[j] <= arr[i] && d[j] + 1 > d[i])
			{
				d[i] = d[j] + 1;
			}
		}
		if(d[i] > len)
			len = d[i];
	}
	free(d);
	return len;
}

int main(int argc, char **argv)
{
	int arr[] = {5, 3, 4, 8, 6, 7};
	printf("%d\n", lis(arr, 6));
	return 0;
}












