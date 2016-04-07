#include <iostream>
#include <string>
#include <vector>

using namespace std;
template <class T>
vector<T> split<string s, string tok>
{
	int cur = 0, i;
	vector<T> ans;
	while((i = s.find(tok, cur)) != s.n_pos)
	{
		stringstream ss;
		T tmp;
		ss << string(&s[cur], &s[i]);
		if(ss >> tmp)
		{
			ans.push_back(tmp);
		}
		cur = i + tok.size();
	}
	return ans;
}

class TCU
{
public:
	vector<int> majors(vector<string> percentages, vector<int> start, 
			vector<int> years);
};
int main(void)
{
	return 0;
}


vector<int> TCU::majors(vector<string> percentages, vector<int> start, 
			vector<int> years)
{
	vector<vector<int>> percent;
	int n = percentages.size();
	for(int i = 0; i < n; i++)
	{
		percent.push_back(split<int>(percentages[i], " "));
	}
	while(years--)
	{
		vector<int> cnt(n, 0);
		for(int i = 0; i < n; i++)
		{
			int left = start[i];
			for(int j = 0; j < n; j++)
			{
				if(i != j)
				{
					cnt[j] += start[i] * p[i][j]/100;
					left -= start[i] * p[i][j]/100;
				}
			}
		}

	}
}











