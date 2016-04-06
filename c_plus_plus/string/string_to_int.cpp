#include <iostream>
#include <sstream>
using namespace std;
int main(void)
{
	stringstream stream;
	string result = "10000";
	int n = 0;
	stream << result;
	stream >> n;
	cout << "n: " << n << endl;
	return 0;
}
