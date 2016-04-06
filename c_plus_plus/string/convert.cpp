#include <iostream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;
template <class out_type, class in_value>

out_type convert(const in_value &t)
{
	stringstream stream;
	out_type result;

	stream << t;
	stream >> result;
	return result;
}

int main(void)
{
	double d;
	string salary;
	string s = "12.56";
	d = convert<double>(s);
	salary = convert<string>(9000.0);
	cout << "d: " << d << endl;
	cout << "salary: " << salary << endl;
	return 0;
}
