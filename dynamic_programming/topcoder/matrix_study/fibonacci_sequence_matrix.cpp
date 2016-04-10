#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;
class Matrix
{
public:
	long matr[2][2];
	Matrix(const Matrix &rhs);
	Matrix(long a, long b, long c, long d);
	Matrix& operator=(const Matrix &);
	long *operator[](const int n)
	{
		return this->matr[n];
	}
	Matrix operator*(const Matrix &rhs);
};

Matrix::Matrix(long a, long b, long c, long d)
{
	this->matr[0][0] = a;
	this->matr[0][1] = b;
	this->matr[1][0] = c;
	this->matr[1][1] = d;
}

Matrix::Matrix(const Matrix &rhs)
{
	this->matr[0][0] = rhs.matr[0][0];
	this->matr[0][1] = rhs.matr[0][1];
	this->matr[1][0] = rhs.matr[1][0];
	this->matr[1][1] = rhs.matr[1][1];
}

Matrix& Matrix::operator=(const Matrix &rhs)
{
	this->matr[0][0] = rhs.matr[0][0];
	this->matr[0][1] = rhs.matr[0][1];
	this->matr[1][0] = rhs.matr[1][0];
	this->matr[1][1] = rhs.matr[1][1];
}

Matrix Matrix::operator*(const Matrix &rhs)
{
	Matrix ret(0, 0, 0, 0);
	ret.matr[0][0] = (this->matr[0][0] * rhs.matr[0][0] + this->matr[0][1]*rhs.matr[1][0]) % 10000;
	ret.matr[0][1] = (this->matr[0][0] * rhs.matr[0][1] + this->matr[0][1]*rhs.matr[1][1]) % 10000;
	ret.matr[1][0] = (this->matr[1][0] * rhs.matr[0][0] + this->matr[1][1]*rhs.matr[1][0]) % 10000;
	ret.matr[1][1] = (this->matr[1][0] * rhs.matr[0][1] + this->matr[1][1]*rhs.matr[1][1]) % 10000;
	return ret;
}

Matrix power(const Matrix &m, int n)
{
	Matrix a = m;
	Matrix ans(1, 0, 0, 1);
	while(n)
	{
		if(n & 1)
		{
			ans = ans * a;
		}
		a = a * a;
		n >>= 1;
	}
	return ans;
}

int getAnswer(int n)
{
	Matrix ans(1, 1, 1, 0);
	if(n == 0)
		cout << 0 << endl;
	else if(n < 0)
		cout << -1 << endl;
	else if(n == 1 || n == 2)
		cout << 1 << endl;
	else
	{
		ans = power(ans, n - 2);
		cout << (ans[0][0] + ans[0][1]) << endl;
	}
	return 0;
}
int main(int argc, char **argv)
{
	int n = 0;
	for(;;)
	{
		stringstream stream;
		cin >> n;
		if(n == -1)
			break;
		else
			getAnswer(n);
	}

	return 0;
}














