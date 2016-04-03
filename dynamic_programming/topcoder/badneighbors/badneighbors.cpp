#include <iostream>
#include <map>
#include <vector>

class BadNeighbors
{
public:
	map<int, int> answer;
	vector<int>  blah;

	int getAnswer(int i1, int i2)
	{
		int key = i1 * 500 + i2;
		if(answer.count(key))
			return answer[key];
		if(i1 > i2)
			return 0;
		answer[key] = MAX(getAnsweri1 + 1, i2, getAnswer(i1 + 2, i2) + blah[i1]);
		return answer[key];
	}
	int maxDonations(vector<int> donations)
	{
		blah = donations;
		int n = donations.size();
		return MAX(getAnswer(0, n - 2), getAnswer(1, n - 1));
	}
};

int main(int argc, char **argv)
{
	int arr[] = {};
	return 0;
}










