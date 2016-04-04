#include <iostream>
#include <vector>
using namespace std;
int max(int a, int b);
class BadNeighbors
{
public:
	int solve(int begin, int end, vector<int> &donations);
	int maxDonations(vector<int> donations);
};

int main(void)
{
	BadNeighbors badNb;
//	int arr[] = {10, 3, 2, 5, 7, 8};
//	int arr[] = {7, 7, 7, 7, 7, 7, 7};
//	int arr[] = {11, 15};
//	int arr[] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
	int arr[] = {94, 40, 49, 65, 21, 21, 106, 80, 92, 81, 679, 4, 61, \
		6, 237, 12, 72, 74, 29, 95, 265, 35, 47, 1, 61, 397, \
			52, 72, 37, 51, 1, 81, 45, 435, 7, 36, 57, 86, 81, 72};
	int max;
	vector<int> vec(arr, arr + sizeof(arr) / sizeof(int));
	max = badNb.maxDonations(vec);
	cout << max << endl;
	return 0;

	return 0;
}

int max(int a, int b)
{
	if(a > b)
		return a;
	else
		return b;
}

int BadNeighbors::solve(int begin, int end, vector<int> &donations)
{
	int dp[end - begin + 2];
	for(int i = begin; i < end + 1; i++)
	{
		if(i == begin)
		{
			dp[i] = donations[i];
			continue;
		}
		if(i > begin + 1)
			dp[i] = max(dp[i - 1], dp[i - 2] + donations[i]);
		else
		{
			dp[i] = max(donations[begin], donations[begin + 1]);
		}
	}
	return dp[end];
}

int BadNeighbors::maxDonations(vector<int> donations)
{
	int n = donations.size();
	int a, b;
	a = solve(0, n - 2, donations);	
	b = solve(1, n - 1, donations);
	if(a > b)
		return a;
	else 
		return b;
}
