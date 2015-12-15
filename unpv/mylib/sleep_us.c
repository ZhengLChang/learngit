#include <unp.h>

int sleep_us(unsigned int nusecs)
{
	struct timeval tval;

	if(nusecs == 0)
		return 0;
	for(; ;)
	{
		tval.tv_sec = nusecs / 1000000;
		tval.tv_usec = nusecs % 1000000;
		if(select(0, NULL, NULL, NULL, &tval) == 0)
		{
			return 0;
		}
		if(errno != EINTR)
			return (-1);
	}
}

void Sleep_us(unsigned int nusecs)
{
	if(sleep_us(nusecs) == -1)
	{
		printf("sleep_us error\n");
		exit(1);
	}
}
