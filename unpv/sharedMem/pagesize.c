#include <unp.h>

int main(int argc, char **argv)
{
	printf("pagesize = %ld\n", sysconf(_SC_PAGESIZE));
	return -1;
}
