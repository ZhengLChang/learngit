#include <iostream>
#include <vector>
#include <algorithm>


int main(void)
{
	int myints[] = {10, 20, 30, 30, 20, 10, 10, 20};
	int mycount = std::count(myints, myints + 8, 10);
	std::cout << "10 appears " << mycount << " times\n";

	std::vector<int> myvector(myints, myints + 8);
	mycount = std::count(myvector.begin(), myvector.end(), 20);
	std::cout << "20 appears " << mycount << " times.\n";
	return 0;
}
