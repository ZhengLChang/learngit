#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

template <class T>
ostream &operator<<(ostream &os, vector<T> v)
{
	int size = v.size();
	int i;
	for(i = 0; i < size; )
	{
		os << v[i] << ", ";	
		if(++i % 9 == 0)
		{
			os << endl;
		}
	}
	os << endl;
	return os;
}

template <class T>
vector<T> split(string s, string tok)
{
	int cur = 0, i;
	vector<T> ans;
	stringstream ss;
	T tmp;
	while((i = s.find(tok, cur)) != s.npos)
	{
		ss << string(&s[cur], &s[i]);
		if(ss >> tmp)
		{
			ans.push_back(tmp);
		}
		cur = i + tok.size();
		ss.clear();
	}
	ss << string(&s[cur], s.size() - 1);
	if(ss >> tmp)
	{
		ans.push_back(tmp);
	}
	ss.clear();

	cout << "ans: " << ans << endl;
	return ans;
}

class TCU
{
public:
	vector<int> majors(vector<string> percentages, vector<int> start, 
			int years);
};
int main(void)
{
	TCU my_tcu;
#if 0
	string str[] = {"80 1 5 14", "2 76 19 3", "1 3 45 51", "30 32 26 12"};
	int arr[] = {1237, 625, 9618, 13476};
	int years = 5;
#endif
	string str[] = {"80 1 5 14","2 76 19 3","1 3 45 51","30 32 26 12"};
	int arr[] = {1237, 625, 9618, 13476};
	int years = 5;
	vector<string> percentages(str, str + sizeof(str) / sizeof(str[0]));
	vector<int> start(arr, arr + sizeof(arr) / sizeof(arr[0]));
	cout << start << endl;	
	start = my_tcu.majors(percentages, start, years);
	cout << start << endl;
	return 0;
}


vector<int> TCU::majors(vector<string> percentages, vector<int> start, 
			int years)
{
	vector< vector<int> > percent;
	int n = percentages.size();
	cout << "**********out put param***********" << endl;
	cout << percentages;
	cout << start;
	cout << years << endl;
	cout << "**************end*****************" << endl;
	for(int i = 0; i < n; i++)
	{
		percent.push_back(split<int>(percentages[i], " "));
	}
	while(years--)
	{
		vector<int> cnt = start;
		for(int i = 0; i < n; i++)
		{
			for(int j = 0; j < n; j++)
			{
				if(i != j)
				{
					cnt[j] += start[i] * percent[i][j]/100 + 0.5;
				}
				else
				{
					cnt[j] -= start[i] * (100 - percent[i][j])/100 + 0.5;
				}
			}
		}
		cout << cnt;
		start = cnt;
	}
	return start;
}











