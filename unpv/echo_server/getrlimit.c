#include <unp.h>

int main(void)
{
	struct rlimit limit;

	if(getrlimit(RLIMIT_NOFILE, &limit) < 0)
	{
		printf("getrlimit error: %s\n", strerror(errno));
		return -1;
	}
	if(limit.rlim_cur == RLIM_INFINITY)
	{
		printf("(infinite)\t");
	}
	else
	{
		printf("%10ld\n", limit.rlim_cur);
	}
	if(limit.rlim_max == RLIM_INFINITY)
	{
		printf("(infinite)\t");
	}
	else
	{
		printf("%10ld\n", limit.rlim_max);
	}
	return 0;
}
