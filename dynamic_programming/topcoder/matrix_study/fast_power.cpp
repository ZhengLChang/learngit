#include <iostream>
#include <string>
#include <sstream>

typedef long long LL;

using namespace std;
#define Max 10000001
class fast_power
{
public:
	LL fun(LL x, LL n)
	{
		LL res = 1;
		while(n > 0)
		{
			if(n & 1)
			{
				res = (res * x) % Max;
			}
			x = (x * x) % Max;
			n >>= 1;
		}
		return res;
	}
};

int main(int argc, char *argv[])
{
	LL x = 1, n = 1;
	stringstream stream;
	fast_power power;
	stream << argv[1];
	stream >> x;
	stream.clear();
	stream << argv[2];
	stream >> n;
	cout << x << "^" << n << " = " << power.fun(x, n) << endl;
	return 0;	
}



















