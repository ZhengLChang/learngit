#include <iostream>

int main()
{
	int arr[2][2];
	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			arr[i][j] = 0;
		}
	}
	arr[0][3] = 5;
	return 0;	
}
