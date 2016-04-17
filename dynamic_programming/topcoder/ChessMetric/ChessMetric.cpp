#include <iostream>
#include <vector>
#include <string>

using namespace std;

class ChessMetric
{
private:
	vector< pair<int, int> > next_step_arr;
public: 
	ChessMetric()
	{
		next_step_arr.push_back(make_pair(1, 1));
		next_step_arr.push_back(make_pair(1, 0));
		next_step_arr.push_back(make_pair(1, -1));
		next_step_arr.push_back(make_pair(0, -1));
		next_step_arr.push_back(make_pair(-1, -1));
		next_step_arr.push_back(make_pair(-1, 0));
		next_step_arr.push_back(make_pair(-1, 1));
		next_step_arr.push_back(make_pair(0, 1));
		next_step_arr.push_back(make_pair(2, -1));
		next_step_arr.push_back(make_pair(1, -2));
		next_step_arr.push_back(make_pair(-1, -2));
		next_step_arr.push_back(make_pair(-2, -1));
		next_step_arr.push_back(make_pair(-2, 1));
		next_step_arr.push_back(make_pair(-1, 2));
		next_step_arr.push_back(make_pair(1, 2));
		next_step_arr.push_back(make_pair(2, 1));
	}
	long long howMany(int size, int start[], int end[], int numMoves)
	{
		long long D[size][size][numMoves + 1];
		for(int i = 0; i < size; i++)
		{
			for(int j = 0; j < size; j++)
			{
				for(int k = 0; k < numMoves + 1 ; k++)
				{
					D[i][j][k] = 0;
				}
			}
		}
		D[start[0]][start[1]][0] = 1;
		for(int step = 1; step <= numMoves; step++)
		{
			for(int i = 0; i < size; i++)
			{
				for(int j = 0; j < size; j++)
				{
					for(int k = 0; k < next_step_arr.size(); k++)
					{
						int next_position_x = i + next_step_arr[k].first;
						int next_position_y = j + next_step_arr[k].second;
						if(next_position_x < 0 ||
						   next_position_y < 0 ||
						   next_position_x >= size ||
						   next_position_y >= size)
						{
							continue;
						}
						D[next_position_x][next_position_y][step] += D[i][j][step - 1];
					}
				}
			}
		}
		return D[end[0]][end[1]][numMoves];
	}
};

int main()
{
	ChessMetric cm;
	int start[] = {0, 0}, end[] = {2, 2};
	cout << cm.howMany(3, start, end, 20) << endl;

	return 0;
}








