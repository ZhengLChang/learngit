#include <iostream>
using namespace std;

int get_lcs_len(int record[], int arr[], int len);

int lcs(int arr[], int len)
{
	int *record = new int[len];
	int lcs_len = -1;
	if(record == NULL)
	{
		cout << "new error" << endl;
	}
	for(int i = 0; i < len; i++)
	{
		record[i] = -1;
	}
	lcs_len = get_lcs_len(record, arr, len);
	return lcs_len;
}
int get_lcs_len(int record[], int arr[], int len)
{
	if(record[len - 1] > 0)
		return record[len - 1];
	if(arr[len - 1] > arr[len - 2])
	{
		record[len - 1] = get_lcs_len(record, arr, len - 1) + 1;
		return record[len - 1];
	}
}

int main(int argc, char **argv)
{
	int A[] = {2, 1, 5, 3, 6, 4, 8, 9, 7};
	cout << lcs(A, 9) << endl;
	return 0;
}















