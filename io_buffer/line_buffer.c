#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	FILE *fp = NULL;

	if((fp = fopen("./a.txt", "w+")) == NULL)
	{
		perror("file to fopen");
		exit(EXIT_FAILURE);
	}
	putc('a', fp);
	printf("IO_cache = %d\n", fp->_IO_buf_end - fp->_IO_buf_base);
	return 0;
}
