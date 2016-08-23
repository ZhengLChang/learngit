#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Mergesort(int A[], int N);

int main(int argc, char **argv)
{
	int arr[] = {8, 9, 3, 4, 5, 28, 1};
	int i = 0; 
	Mergesort(arr, sizeof(arr) / sizeof(int));
	for(i = 0; i < sizeof(arr) / sizeof(int); i++)
	{
		printf("%d ", arr[i]);
	}
	printf("\n");
	return 0;
}
void Merge(int A[], int TmpArray[], int Lpos, int Rpos, int RightEnd)
{
	int i, LeftEnd, NumElements, TmpPos;

	LeftEnd = Rpos - 1;
	TmpPos = Lpos;
	NumElements = RightEnd - Lpos + 1;


	while(Lpos <= LeftEnd && Rpos <= RightEnd)
	{
		if(A[Lpos] <= A[Rpos])
		{
			TmpArray[TmpPos++] = A[Lpos++];
		}
		else
		{
			TmpArray[TmpPos++] = A[Rpos++];
		}
	}
	while(Lpos <= LeftEnd)
	{
		TmpArray[TmpPos++] = A[Lpos++];
	}
	while(Rpos <= RightEnd)
	{
		TmpArray[TmpPos++] = A[Rpos++];
	}
	for(i = 0; i < NumElements; i++, RightEnd--)
	{
		A[RightEnd] = TmpArray[RightEnd];
	}
}

void MSort(int A[], int TmpArray[], int Left, int Right)
{
	int Center;
	
	if(Left < Right)
	{
		Center = (Left + Right) / 2;
		MSort(A, TmpArray, Left, Center);
		MSort(A, TmpArray, Center + 1, Right);
		Merge(A, TmpArray, Left, Center + 1, Right);
	}
}

void Mergesort(int A[], int N)
{
	int *TmpArray;
	
	TmpArray = malloc(N * sizeof(int));
	if(TmpArray != NULL)
	{
		MSort(A, TmpArray, 0, N - 1);
	}
	else
	{
		printf("No space for tmp array!!!\n");
	}
}















