#include <iostream>
#include <string>
#include <cstdio>
using namespace std;

int main(int argc, char **argv)
{
	string sa = "Copy on write";
	string sb = sa;
	string sc = sb;
	printf("sa char buffer address: 0x%08x\n", sa.c_str());
	printf("sb char buffer address: 0x%08x\n", sb.c_str());
	printf("sc char buffer address: 0x%08x\n", sc.c_str());

	sc = "Now writing...";
	printf("After writing sc: \n");
	printf("sa char buffer address: 0x%08x\n", sa.c_str());
	printf("sb char buffer address: 0x%08x\n", sb.c_str());
	printf("sc char buffer address: 0x%08x\n", sc.c_str());

	return 0;
}
