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
vector<T> split(string s, string tok, bool flag = false)
{
	s+=tok;
	int cur = 0, i;
	vector<T> ans;
	while((i = s.find(tok, cur)) != s.npos)
	{
		stringstream ss;
		T tmp;
		ss << string(&s[cur], &s[i]);
		if((ss >> tmp) || flag)
		{
			ans.push_back(tmp);
		}
		cur = i + tok.size();
	}
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
	int arr[] = {1237, 625, 9618, 134760};
	int years = 500;
#endif
#if 0
	string str[] = {"99 01","99   001"};
	int arr[] = {1000000,0};
	int years = 2;
#endif
	string str[] = {"00 100","100 00"};
	int arr[] = {23, 37};
	int years = 999;
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
		vector<int> cnt(n, 0);
		for(int i = 0; i < n; i++)
		{
			int left = start[i];
			for(int j = 0; j < n; j++)
			{
				cnt[j] += start[i] * percent[i][j]/100;
				left -= start[i] * percent[i][j]/100;
			}
			cnt[i] += left;
		}
		cout << cnt;
		start = cnt;
	}
	return start;
}











