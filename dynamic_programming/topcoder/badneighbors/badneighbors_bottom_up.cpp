#include <iostream>
#include <vector>
using namespace std;
int max(int a, int b);
class BadNeighbors
{
public:
	int solve(int a, int b, vector<int> &arr);
	int maxDonations(vector<int> donations);
};

int main(void)
{
	BadNeighbors badNb;
//	int arr[] = {10, 3, 2, 5, 7, 8};
	int arr[] = {7, 7, 7, 7, 7, 7, 7};
	int max;
	vector<int> vec(arr, arr + sizeof(arr) / sizeof(int));
	max = badNb.maxDonations(vec);
	cout << max << endl;
	return 0;
}

int BadNeighbors::solve(int a, int b, vector<int> &arr)
{
	int dp[b - a + 2];
	int ans = 0;
	for(int i = a; i < b + 1; i++)
	{
		if(i == a)
		{
			dp[i] = arr[a];
			ans = arr[a];
		}
		else
		{
			dp[i] = arr[i];
			for(int j = a; j < i - 1; j++)
			{
				dp[i] = max(dp[i], dp[j] + arr[i]);
			}
			if(dp[i] > ans)
				ans = dp[i];
		}
	}
	return ans;
}

int BadNeighbors::maxDonations(vector<int> donations)
{
	int n = donations.size();
	int a = solve(1, n - 1, donations);
	int b = solve(0, n - 2, donations);
	if(a > b)
		return a;
	else 
		return b;
}

int max(int a, int b)
{
	if(a > b)
		return a;
	else
		return b;
}









