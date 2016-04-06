#include <iostream>
using namespace std;

int main(void)
{
	char name[40], dessert[40], str[40];

	cin.get(name, 40);
	cin.get();
	cin.get(dessert, 40);
	cin.get();
	cin.get(str, 40);
	cin.get();

	cout << "name: " << name << endl;
	cout << "dessert: " << dessert << endl;
	cout << "str: " << dessert << endl;

	return 0;
}
