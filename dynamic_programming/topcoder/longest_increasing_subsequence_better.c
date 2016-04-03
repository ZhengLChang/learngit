#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <alloca.h>

int upper_bound(int arr[], int s, int e, int key)
{
	int mid;
	if(arr[e] <= key)
		return e + 1;
	while(s < e)
	{
		mid = s + (e - s) / 2;
		if(arr[mid] <= key)
			s = mid + 1;
		else
			e = mid;
	}
	return s;
}

int LIS(int d[], int n)
{
	int i = 0, len = 1, *end = (int *)alloca(sizeof(int) * (n + 1));
	int pos;
	end[1] = d[0];
	for(i = 1; i < n; i++)
	{
		pos = upper_bound(end, 1, len, d[i]);
		end[pos] = d[i];
		if(len < pos)
			len = pos;
	}
	return len;
}

int main(int argc, char **argv)
{
	int arr[] = {5, 3, 4, 8, 6, 7};
	printf("%d\n", LIS(arr, 6));
	return 0;
}









