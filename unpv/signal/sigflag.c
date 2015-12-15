#include <unp.h>

int main(void)
{
#ifdef SA_INTERRUPT
	printf("SA_INTERRUPT is defined\n");
#endif
#ifdef SA_RESTART
	printf("SA_RESTART is defined\n");
#endif
	return 0;
}
