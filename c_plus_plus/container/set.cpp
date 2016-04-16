#include <iostream>
#include <set>

using namespace std;

int main()
{
	set<int> s;
	s.insert(1);
	s.insert(2);
	s.insert(3);
	s.insert(0);
	cout << "the size of set: " << s.size() << endl;
	cout << "the max size of set: " << s.max_size() << endl;
	cout << "the first element: " << *s.begin() << endl;
	cout << "the last element: " << *s.end() << endl;
	s.clear();
	if(s.empty())
	{
		cout << "set is empty!!!" << endl;
	}
	cout << "the size of set: " << s.size() << endl;
	cout << "the max size of set: " << s.max_size() << endl;
	return 0;
}
