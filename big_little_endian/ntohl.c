#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	unsigned int i = 1;
	printf("0x%x\n", htonl(i));
	printf("0x%x\n", htonl(htonl(i)));
	return 0;
}
