#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
using namespace std;
int main()
{
	vector< pair<int , int> > pair_arr;
	pair_arr.push_back(make_pair(1, 1));
	pair_arr.push_back(make_pair(2, 2));
	pair_arr.push_back(make_pair(3, 3));
	pair_arr.push_back(make_pair(4, 4));
	pair_arr.push_back(make_pair(5, 6));
	cout << pair_arr.size() << endl;
	for(int i = 0; i < pair_arr.size(); i++)
	{
		cout << pair_arr[i].first << " "  << pair_arr[i].second << endl;
	}
	return 0;
}
