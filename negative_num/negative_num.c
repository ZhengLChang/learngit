#include <stdio.h>
/*remeber the num which is stored in the memery is complement*/
int main(void)
{
	printf("%lu\n", (unsigned long)-22); 
	printf("%lu\n", (unsigned long)-1000L);
	printf("%d\n", (unsigned long)-22 > (unsigned long)-1000L);
	return 0;
}
