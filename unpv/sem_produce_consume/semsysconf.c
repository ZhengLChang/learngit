#include <unp.h>

int main(int argc, char **argv)
{
	printf("SEM_NSEMS_MAX = %ld, SEM_VALUE_MAX = %ld\n",
			sysconf(_SC_SEM_NSEMS_MAX), sysconf(_SC_SEM_VALUE_MAX));
	return 0;
}
