#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

class GCD
{
public:
	int gcd(int a, int b)
	{
		if(!b)
			return a;
		return gcd(b, a % b);
	}
};

int main()
{
	GCD mygcd;
	std::cout << mygcd.gcd(4, 6) << std::endl;
	return 0;
}
