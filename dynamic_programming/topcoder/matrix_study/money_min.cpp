#include <iostream>
#include <vector>
using namespace std;

class money_min
{
public:
	int get_min(int money, vector<int> denominations);
};

int main(void)
{
	int arr[] = {1, 3, 5};
	vector<int> denominations(arr, arr + sizeof(arr) / sizeof(arr[0]));
	money_min min;
	for(int sum = 1; sum <= 11; sum++)
	{
		cout << sum << " = "<< min.get_min(sum, denominations) << endl;
	}
	return 0;
}

int money_min::get_min(int money, vector<int> denominations)
{
	int subproblem[money + 1];
	int n = denominations.size();
//	cout << "n = " << n << endl;
	for(int i = 0; i < money; i++)
	{
		subproblem[i] = 1000000;
	}
	subproblem[0] = 0;
	for(int i = 1; i <= money; i++)
	{
		for(int j = 0; j < n && denominations[j] <= i; j++)
		{
			if(subproblem[i - denominations[j]] + 1 < subproblem[i])
			{
				subproblem[i] = subproblem[i - denominations[j]] + 1;
			}
		}
	}
	return subproblem[money];
}











