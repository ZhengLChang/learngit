#include <sort.h>

void InsertionSort(int arr[], int n)
{
	int j, p;
	int Tmp;
	int i;
	for(p = 1; p < n; p++)
	{
		Tmp = arr[p];
		for(j = p; j > 0 && arr[j - 1] > Tmp; j--)
			arr[j] = arr[j - 1];
		arr[j] = Tmp;
		for(i = 0; i < n; i++)	
			printf("%d ", arr[i]);
		printf("\n");
	}
}
