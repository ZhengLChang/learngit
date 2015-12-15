#include <unp.h>

int main(void)
{
#ifdef INET_ADDRSTRLEN
	printf("INET_ADDRSTRLEN value: %d\n", INET_ADDRSTRLEN);
#else
	printf("INET_ADDRSTRLEN is not defined\n");
#endif
	return 0;
}
