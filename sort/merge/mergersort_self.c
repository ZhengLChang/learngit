#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mergesort(int arr[], int n);
int main(int argc, char **argv)
{
	return 0;
}
void Merge(int arr[], int TmpArr[], int Lpos, int Rpos, int RightEnd)
{
	int i, LeftEnd, NumElements, TmpPos;
	LeftEnd = Rpos - 1;
	TmpPos = Lpos;
	NumElement = RightEnd - Lpos + 1;
	while(Lpos <= LeftEnd && Rpos <= RightEnd)
		if(arr[Lpos] <= arr[Rpos])
		{
			TmpArr[TmpPos++] = arr[Lpos++];
		}
		else
		{
			TmpArr[TmpPos++] = arr[Rpos++];
		}
	while(Lpos <= LeftEnd)
		TmpArr[TmpPos++] = arr[Lpos++];
	while(Rpos <= RightEnd)
		TmpPos[TmpPos++] = arr[Rpos++];
	for(i = 0; i < NumElement; i++, RightEnd--)
		arr[RightEnd] = TmpArr[RightEnd]
	
}
void Msort(int arr[], int TmpArr[], int Left, int Right)
{
	int center;
	if(Left < Right)
	{
		center = (Left + Right) / 2;
		Msort(arr, TmpArr, Left, center);
		Msort(arr, TmpArr, center + 1, Right);
		Merge(arr, TmpArr, Left, center + 1, Right);
	}
}
void mergesort(int arr[], int n)
{
	int *TmpArr;
	TmpArr = malloc(n * sizeof(int));
	if(TmpArr != NULL)
	{
		MSort();
		free(TmpArr);
	}
	else
	{
		printf("No space for tmp array!!!\n");
	}
	return ;
}

