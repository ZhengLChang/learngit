#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

template<class T>
void to_string(string &result, const T &t)
{
	ostringstream oss;
	oss << t;
	result = oss.str();
	return ;
}

int main(void)
{
	string str;
	to_string(str, 10.5);
	cout << str << endl;
	return 0;
}
