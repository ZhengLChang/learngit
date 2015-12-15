#include <unp.h>

int main(int argc, char *argv[])
{
	fd_set rfs;
/*system io*/
#if 0
	char buf[3] = {0};
	for(;;)
	{
		FD_ZERO(&rfs);
		FD_SET(0, &rfs);
		select(1, &rfs, 0, 0, 0);
		read(STDIN_FILENO, buf, 1);
		printf("get character: %c\n", buf[0]);
	}
#endif
/*std io*/
#if 1
	int r = 0;
	for(;;)
	{
		FD_ZERO(&rfs);
		FD_SET(0, &rfs);
		select(1, &rfs, 0, 0, 0);
		r = getc(stdin);
		printf("get character: %c\n", r);
	}
#endif
	return 0;
}
