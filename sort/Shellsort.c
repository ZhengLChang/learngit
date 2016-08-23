#include <sort.h>

void Shellsort(int A[], int N)
{
	int i, j, Increment;
	int Tmp;
	for(Increment = N / 2; Increment > 0; Increment /= 2)
	{
		for(i = Increment; i < N; i++)
		{
			Tmp = A[i];
			for(j = i; j >= Increment; j -= Increment)
			{
				if(Tmp < A[j - Increment])
					A[j] = A[j - Increment];
				else 
					break;
			}
			A[j] = Tmp;
		}
	}
	for(i = 0; i < N; i++)
	{
		printf("%d\t", A[i]);
	}
	printf("\n");
	return ;
}
