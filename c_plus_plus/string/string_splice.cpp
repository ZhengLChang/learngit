#include <iostream>
#include <string>
#include <sstream>

using namespace std;
string str;
template<class T>
stringstream &operator<<(stringstream &stream, T value)
{
	stream << value;
	stream >> str;
	stream.clear();
	return stream;
}

class my_string
{
public:
	string str;

};

int main()
{
	stringstream stream;
	stream << 1 << " + " << 2;
	cout << str << endl;
	
#if 0
	string str, temp;
	istream input;
	ostream output;
	stream << 1;
	stream >> temp;
	stream.clear();
	str += temp;
	str += " + ";
	stream << 2;
	stream >> temp;
	stream.clear();
	str += temp;
	isput << 1 << " + " << 2;
	output >> str;

	cout << str << endl;
#endif
	return 0;
}
