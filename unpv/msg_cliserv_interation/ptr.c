#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char p[] = "123 /root";
	char *ptr = strchr(p, ' ');
	*ptr++ = 0;
	printf("%s\n", ptr);
	return 0;
}
