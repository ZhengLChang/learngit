#include <string>
#include <vector>
#include <map>
#include <set>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <iostream>

using namespace std;

typedef long long i64;
i64 ans[102][102];
int dx[] = {1, 0};
int dy[] = {0, 1};

vector<string> tokenize(string s, string tok)
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

vector<int> sti(const vector<string> &a)
{
	vector<int> oot;
	for(int i = 0; i < a.size(); i++)
	{
		oot.push_back(atoi(a[i].c_str()));
	}
	return oot;
}

int gcd(int a, int b)
{
	if(!b)
		return a;
	return gcd(b, a % b);
}

class AvoidRoads
{
public:
	long long numWays(int width, int height, vector<string> bad)
	{
		ans[0][0] = 1;
		set< pair< pair<int, int>, pair< int, int> > > nt;
		for(int i = 0; i < bad.size(); i++)
		{
			vector<int> st = sti(tokenize(bad[i], " "));
			nt.insert(make_pair(make_pair(st[0], st[1]), make_pair(st[2], st[3])));
			nt.insert(make_pair(make_pair(st[2], st[1]), make_pair(st[0], st[1])));
		}
		for(int x = 0; x <= width; x++)
		{
			for(int y = 0; y <= height; y++)
			{
				for(int k = 0; k < 2; k++)
				{
					int tx = x + dx[k];
					int ty = y + dy[k];
					if(!nt.count(make_pair(make_pair(x, y), make_pair(tx, ty))))
					{
						ans[tx][ty] += ans[x][y];
					}
				}
			}
		}
		return ans[width][height];
	}
};

int main(void)
{
	AvoidRoads l;
	vector<string> str;
	str.push_back("0 2 0 3");
	str.push_back("1 2 1 3");
       	str.push_back("2 2 2 3");
	str.push_back("3 2 3 3");
	str.push_back("4 2 4 3");
       	str.push_back("5 2 5 3");
	str.push_back("6 2 6 3");
	str.push_back("7 2 7 3");
	str.push_back("8 2 8 3");
	str.push_back("9 2 9 3");
//	str.push_back("0 0 0 1");
//	str.push_back("5 6 6 6");
	cout << l.numWays(10, 100, str) << endl;
	return 0;
}














