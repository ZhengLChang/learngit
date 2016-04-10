#include <iostream>
#include <vector>
using namespace std;

class series
{
public:
	int increasingLength(vector<int> a);
};

int main(void)
{
	int arr[] = {-4, 5, -2, 0, 4, 5, 9, 9};
	vector<int> vec(arr, arr + sizeof(arr) / sizeof(int));
	series mySer;
	int maxNum; 
	maxNum = mySer.increasingLength(vec);
	cout << maxNum << endl; 
	return 0;
}

int series::increasingLength(vector<int> a)
{
	int n = a.size();
	vector<int> increaingArr, tempArr;
	int begin = -1, end = -1;
	int i;
	if(n > 1 && a[1] < a[0])
	{
		increaingArr.push_back(a[1]);
		i = 2;
	}
	else
	{
		increaingArr.push_back(a[0]);
		i = 1;
	}
	for(; i < n ; i++)
	{
		int size = increaingArr.size();
		if(a[i] > increaingArr[size - 1])
		{
			increaingArr.push_back(a[i]);
		}
		else if(a[i] < increaingArr[size - 1])
		{
			for(int j = i; j < n - 1; j++)
			{
				if(a[j + 1] > a[j] && begin == -1)
				{
					begin = j;
					continue;
				}
				else if(a[j + 1] < a[j] && begin == -1)
				{
					continue;
				}
				else if(a[j + 1] < a[j] && begin > -1)
				{
					end = j;
				}
				if(begin > -1 && end > -1)
				{
					int sum = 0;
					for(int k = begin; k < end + 1; k++)
					{
						if(a[begin] <= increaingArr[increaingArr.size() - 1 - sum])
						{
							sum++;
						}
						else if(sum <= end - begin + 1)
						{
							int p = end - begin + 1;
							while(end - begin + 1 > 0)
							{
								if(sum > 0)
								{
									increaingArr[increaingArr.size() - 1 - sum] = a[begin];
									sum--;
								}
								else
								{
									increaingArr.push_back(a[begin]);
								}
								begin++;
							}
						}
						else
						{
							k = begin;
							begin++;
						}
					}
				}
			}
		}
	}
	return increaingArr.size();
}











