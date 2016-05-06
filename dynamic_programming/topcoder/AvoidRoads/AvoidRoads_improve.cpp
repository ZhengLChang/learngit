#include <iostream>
#include <vector>
#include <cstdio>
#include <string>
#include <cstring>
#include <sstream>


using namespace std;
int dx[] = {1, 0};
int dy[] = {0, 1};
class AvoidRoads
{
public:
	long long numWays(int width, int height, vector<string> &bad);
};


int main(void)
{
	AvoidRoads l;
	vector<string> str;
#if 0
	str.push_back("0 3 0 2");
	str.push_back("1 2 1 3");
       	str.push_back("2 2 2 3");
	str.push_back("3 2 3 3");
	str.push_back("4 2 4 3");
       	str.push_back("5 2 5 3");
	str.push_back("6 2 6 3");
	str.push_back("7 2 7 3");
	str.push_back("8 2 8 3");
	str.push_back("9 2 9 3");
#endif
	str.push_back("0 0 0 1");
	str.push_back("6 6 5 6");
	cout << l.numWays(6, 6, str) << endl;
	return 0;
}
long long AvoidRoads::numWays(int width, int height, vector<string> &bad)
{
	long long D[width + 1][height + 1];
	char is_bad1[128], is_bad2[128];
	for(int i = 0; i <= width; i++)
	{
		for(int j = 0; j <= height; j++)
		{
			D[i][j] = 0;
		}
	}
	D[0][0] = 1;
	for(int i = 0; i <= width; i++)
	{
		for(int j = 0; j <= height; j++)
		{
			for(int k = 0; k < 2; k++)
			{
				int is_bad = 0;
				int tx = i + dx[k];
				int ty = j + dy[k];
#if 1        /*If you know why the next two line is needful, you are so clever*/
				if(tx > width || ty > height)
					continue;
#endif
				snprintf(is_bad1, sizeof(is_bad1), "%d %d %d %d", i, j, tx, ty);
				snprintf(is_bad2, sizeof(is_bad2), "%d %d %d %d", tx, ty, i, j);
				for(int p = 0; p < bad.size(); p++)
				{
					if(bad[p] == string(is_bad1) || bad[p] == string(is_bad2))
					{
						is_bad = 1;
						break;
					}
				}
				if(is_bad != 1)
				{
					D[tx][ty] += D[i][j];
				}
			}
		}
	}
	return D[width][height];
}















