#include <iostream>
#include <sstream>
#include <string>

using namespace std;
int main(void)
{
	string str("There are two needles in this haystack with needles.");
	string str2("needle");
	size_t found = str.find(str2);
	if(found != std::string::npos)
	{
		cout << "first 'needle' found at: " << found << endl;
	}
	else
	{
		cout << "first found nothing" << endl;
	}

	found = str.find("needles are small", found + 1, 6);
	if(found != std::string::npos)
	{
		cout << "second 'needle' found at: " << found << endl;
	}
	else
	{
		cout << "second found nothing" << endl;
	}

	return 0;
}
