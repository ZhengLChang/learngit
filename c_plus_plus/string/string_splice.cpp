#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main()
{
	string str, temp;
	stringstream stream;

	stream << 1;
	stream >> temp;
	stream.clear();
	str += temp;
	str += " + ";
	stream << 2;
	stream >> temp;
	stream.clear();
	str += temp;

	cout << str << endl;
	return 0;
}
