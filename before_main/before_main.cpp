#include <iostream>
#include <cstdlib>
using namespace std;

class TestClass
{
	public:
		TestClass();
};

TestClass::TestClass()
{
	cout << "TestClass" << endl;
	exit(0);
}


TestClass Ts; /*global object*/



int main(int argc, char *argv[])
{
	cout << "main" << endl;
	return 0;
}
