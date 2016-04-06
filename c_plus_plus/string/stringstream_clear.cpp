#include <iostream>
#include <sstream>
#include <string>

int main(void)
{
	std::stringstream stream;
	int first, second;
	stream << "456";
	stream >> first;
	std::cout << first << std::endl;

	stream.clear();
	stream << true;
	stream >> second;
	std::cout << second << std::endl;
	return 0;
}
