#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

using namespace std;

const int mod = 1000000007;
const long long mod2 = (long long)mod * mod;

class Matrix
{
public:
	int n;
	int m;
	int *data;
	
	Matrix(int n, int m)
	{
		this->n = n;
		this->m = m;
		data = (int *)calloc(n * m, sizeof(int));
	}
	Matrix(const Matrix &a)
	{
		n = a.n;
		m = a.m;
		data = (int *)malloc(sizeof(int) * n * m);
		for(int i = 0; i < n * m; i++)
			data[i] = a.data[i];
	}
	~Matrix()
	{
		free(data);
	}
	Matrix operator=(const Matrix &a)
	{
		free(data);

		n = a.n;
		m = a.m;
		data = (int *)malloc(sizeof(int) * n * m);
		for(int i = 0; i < n * m; i++)
			data[i] = a.data[i];
		return *this;
	}
	int *operator[](int x)
	{
		return (data + x * m);
	}
	void identify(void)
	{
		for(int i = 0, j = 0; i < n; i++, j += m + 1)
			data[j] = 1;
	}
	int *reversal()
	{
		int *tmp = (int *)malloc(sizeof(int) * n * m);
		for(int i = 0; i < m; i++)
		{
			for(int j = 0; j < n; j++)
			{
				tmp[i * n + j] = data[j * m + i];
			}
		}
		free(data);
		data = tmp;
		return data;
	}
	const Matrix operator*(const Matrix &a) const
	{
		int *tmp = (int *)malloc(sizeof(int) * a.n * a.m);
		Matrix ans(n, a.m);
		for(int i = 0; i < a.m; i++)
		{
			for(int j = 0; j < a.n; j++)
			{
				tmp[i * a.n + j] = a.data[j * a.m + i];
			}
		}
		for(int i = 0, l = 0; i < n; i++)
		{
			int *p1 = data + i * m;
			int *p2 = tmp;

			for(int j = 0; j < a.m; j++, l++, p2 += m)
			{
				int *pa = p1;
				int *pb = p2;
				long long sum = 0;
				for(int k = 0; k < m; k++, pa++, pb++)
				{
					sum += (long long)*pa * *pb;
					if(sum >= mod2)
						sum -= mod2;
				}
				ans.data[l] = sum % mod;
			}
		}
		free(tmp);
		return ans;
	}
	void operator*=(const Matrix &a)
	{
		int *tmp = (int *)malloc(sizeof(int) * n * n);
		int *ans = (int *)malloc(sizeof(int) * n * n);
		for(int i = 0; i < n; i++)
		{
			for(int j = 0; j < n; j++)
			{
				tmp[i * n + j] = a.data[j * n + i];
			}
		}
		for(int i = 0, l = 0; i < n; i++)
		{
			int *p1 = data + i * n;
			int *p2 = tmp;

			for(int j = 0; j < n; j++, l++, p2 += n)
			{
				int *pa = p1;
				int *pb = p2;
				long long sum = 0;
				for(int k = 0; k < n; k++, pa++, pb++)
				{
					sum += (long long)*pa * *pb;
					if(sum >= mod2)
						sum -= mod2;
				}
				ans[l] = sum % mod;
			}
		}
		for(int i = 0; i < n * n; i++)
			data[i] = ans[i];
		free(tmp);
		free(ans);
	}
	const Matrix operator^(long long x) const
	{
		Matrix a = *this;
		Matrix ans(n, m);
		ans.identify();

		while(x)
		{
			if(x & 1)
				ans *= a;
			a *= a;
			x >>= 1;
		}
		return ans;
	}
};


class PowerPartition
{
public:
	int count(int M, long long X)
	{
		int n, i, j;
		long long x = 1, ans = 0;
		vector<long long> v;
		vector<Matrix> w;

		v.push_back(x);
		while(1)
		{
			if(X / x < M)
			{
				break;
			}
			x *= M;
			v.push_back(x);
		}
		n = v.size();
		Matrix a(n, n);
		a.identify();
		w.push_back(a);
		for(i = 1; i < n; i++)
		{
			Matrix a(n, n);
			a.identify();
			for(j = 0; j < i; j++)
			{
				a[j][i] = 1;
			}
			w.push_back(a * (w.back() ^ M));
		}
		Matrix b(n, n);
		b.identify();
		for(i = n - 1; i >= 0; i--)
		{
			long long x = X / v[i];
			X %= v[i];
			b = (w[i] ^ x) * b;
		}
		for(i = 0; i < n; i++)
			ans += b[0][i];
		return ans % mod;
	}
};


int main(void)
{
	PowerPartition p;
	cout << p.count(1000, 1000001) << endl;
	return 0;
}
















