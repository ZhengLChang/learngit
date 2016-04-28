#include <iostream>
#include <vector>

using namespace std;

int main(void)
{
	vector<int> vec;
	vector<int>::iterator pr;
	vec.push_back(1);
	vec.push_back(10);
	vec.push_back(100);
	vec.push_back(1000);
	for(pr = vec.begin(); pr != vec.end(); pr++)
	{
		cout << *pr << endl;
	}
	return 0;
}
