#include <stdio.h>

int binary_search(int array[], int n, int value)
{
	int left = 0;
	int right = n - 1;

	while(left <= right)
	{
		int middle = left + ((right - left) >> 1);
		if(array[middle] > value)
		{
			right = middle - 1;
		}
		else if(array[middle] < value)
		{
			left = middle + 1;
		}
		else
			return middle;
	}
	return -1;
}

int main(void)
{
	int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14};
	int pos = 0; 
	if((pos = binary_search(arr, sizeof(arr) / sizeof(int), 9)) != -1)
	{
		printf("pos = %d\n", pos);
	}
	return 0;
}
