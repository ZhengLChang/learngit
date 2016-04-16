#include <iostream>
#include <vector>
#include <cstdio>
#include <string>
#include <cstring>
#include <sstream>


using namespace std;
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
			D[i][j] = -2;
		}
	}
	for(int i = 0; i <= width; i++)
	{
		for(int j = 0; j <= height; j++)
		{
			if(i == 0 && j == 0)
			{
				D[0][0] = 1;
				continue;
			}
			if(i != 0 && j == 0)
			{
				snprintf(is_bad1, sizeof(is_bad1), "%d %d %d %d", i - 1, j, i, j);
				snprintf(is_bad2, sizeof(is_bad2), "%d %d %d %d", i, j, i - 1, j);
				int p;
				for(p = 0; p < bad.size(); p++)
				{
					if(bad[p] == string(is_bad1) || bad[p] == string(is_bad2))
					{
						break;
					}
				}
				if((bad.size() != 0 && p != bad.size()) ||
						D[i - 1][j] == -1)
				{
					D[i][j]	= -1;
				}
				else if(D[i - 1][j] != -1)
				{
					D[i][j] = D[i - 1][j];
				}
			}
			else if(i == 0 && j != 0)
			{
				snprintf(is_bad1, sizeof(is_bad1), "%d %d %d %d", i, j - 1, i, j);
				snprintf(is_bad2, sizeof(is_bad2), "%d %d %d %d", i, j, i, j - 1);
				int p;
				for(p = 0; p < bad.size(); p++)
				{
					if(bad[p] == string(is_bad1) || bad[p] == string(is_bad2))
					{
						break;
					}
				}
				if((bad.size() != 0 && p != bad.size()) ||
						D[i][j - 1] == -1)
				{
					D[i][j]	= -1;
				}
				else if(D[i - 1][j] != -1)
				{
					D[i][j] = D[i][j - 1];
				}

			}
			else
			{
				int left = 1, down = 1;
				snprintf(is_bad1, sizeof(is_bad1), "%d %d %d %d", i - 1, j, i, j);
				snprintf(is_bad2, sizeof(is_bad2), "%d %d %d %d", i, j, i - 1, j);

				int p;
				for(p = 0; p < bad.size(); p++)
				{
					if(bad[p] == string(is_bad1) || bad[p] == string(is_bad2))
					{
						left = 0;
						break;
					}
				}
				snprintf(is_bad1, sizeof(is_bad1), "%d %d %d %d", i, j - 1, i, j);
				snprintf(is_bad2, sizeof(is_bad2), "%d %d %d %d", i, j, i, j - 1);
				for(p = 0; p < bad.size(); p++)
				{
					if(bad[p] == string(is_bad1) || bad[p] == string(is_bad2))
					{
						down = 0;
						break;
					}
				}
				if((left == 0 && down == 0) ||
					(D[i - 1][j] == -1 && D[i][j - 1] == -1) ||
					(left == 0 && D[i][j - 1] == -1) ||
					(D[i - 1][j] == -1 && down == 0))
				{
					D[i][j] = -1;
				}
				else if(left == 0 || D[i - 1][j] == -1)
				{
					D[i][j] = D[i][j - 1];
				}
				else if(down == 0 || D[i][j - 1] == -1)
				{
					D[i][j] = D[i - 1][j];
				}
				else
				{
					D[i][j] = D[i - 1][j] + D[i][j - 1];
				}

			}
		}
	}
	return D[width][height];
}















