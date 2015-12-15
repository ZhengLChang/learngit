#include <unp.h>
struct mem
{
	char a;
};
int main(void)
{
	struct mem *ptr;
	if((ptr = calloc(1, 100 * sizeof(char))) == NULL)
	{
		printf("calloc error: %s\n", strerror(errno));
		return -1;
	}
	if(ptr != NULL)
	{
		free(ptr);
		ptr = NULL;
	}
	return 0;
}
