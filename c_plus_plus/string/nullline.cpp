#include <iostream>
using namespace std;

int main(void)
{
	char str1[4], str2[4];

	cin.getline(str1, 4);
	cin.clear();
	cin.getline(str2, 4);
	cout << "first string: " << str1 << endl;
	cout << "second string: " << str2 << endl;
	return 0;
}
