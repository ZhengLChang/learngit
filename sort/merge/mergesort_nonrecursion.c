#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void mergesort(int arr[], int n);
int main(int argc, char **argv)
{
	int arr[] = {8, 9, 3, 4, 5, 28, 1};
	int i = 0; 
	mergesort(arr, sizeof(arr) / sizeof(int));
	for(i = 0; i < sizeof(arr) / sizeof(int); i++)
	{
		printf("%d ", arr[i]);
	}
	printf("\n");

	return 0;
}

void mergesort(int arr[], int n)
{
	int i, left_min, left_max, right_min, right_max, next;
	int *tmp = malloc(n * sizeof(int));
	if(tmp == NULL)
	{
		printf("No space for tmp\n");
		abort();
	}
	for(i = 1; i < n; i *= 2)
	{
		for(left_min = 0; left_min < n - i; left_min = right_max)
		{
			right_min = left_max = left_min + i;
			right_max = left_max + i;
			if(right_max > n)
				right_max = n;
			next = 0;
			while(left_min < left_max && right_min < right_max)
			{
				tmp[next++] = arr[left_min] > arr[right_min] ? arr[right_min++]:arr[left_min++];
			}
			while(left_min < left_max)
				arr[--right_min] = arr[--left_max];
			while(next > 0)
			{
				arr[--right_min] = tmp[--next];
			}
		}
	}
	free(tmp);
	return;
}



















