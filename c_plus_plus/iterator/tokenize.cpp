#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class tokenizer
{
public:
	vector<string> split(string s, string tok)
	{
		const char *q = s.c_str(); 
		while(count(tok.begin(), tok.end(), *q))
		{
			q++;
		}
		vector<string> oot;
		while(*q)
		{
			const char *e = q;
			while(*e && !count(tok.begin(), tok.end(), *e))
			{
				e++;
			}
			oot.push_back(string(q, e));
			q = e;
			while(count(tok.begin(), tok.end(), *q))
			{
				q++;
			}
		}
		return oot;
	}
};

int main()
{
	vector<string> str_arr;
	tokenizer mytokenizer;
	str_arr=mytokenizer.split("zheng hui jie", " ");
	for(int i = 0; i < str_arr.size(); i++)
	{
		cout << str_arr[i] << endl;
	}
	return 0;
}





