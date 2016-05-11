#include <iostream>
using namespace std;

class TestClass
{
	public:
		TestClass();
};

TestClass::TestClass()
{
	cout << "TestClass" << endl;
}

TestClass Ts;
int main(int argc, char *argv[])
{
	cout << "main" << endl;
	return 0;
}
