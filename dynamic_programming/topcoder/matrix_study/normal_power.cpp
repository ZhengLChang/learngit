#include <iostream>
#include <string>
#include <sstream>

typedef long long LL;

using namespace std;
#define Max 10000001
class normal_power 
{
public:
	LL fun(LL x, LL n)
	{
		if(n == 0)
			return 1;
		if(n == 1)
			return x;
		return (x * fun(x, n - 1));
	}
};

int main(int argc, char *argv[])
{
	LL x = 1, n = 1;
	stringstream stream;
	normal_power power;
	stream << argv[1];
	stream >> x;
	stream.clear();
	stream << argv[2];
	stream >> n;
	cout << x << "^" << n << " = " << power.fun(x, n) << endl;
	return 0;	
}



















