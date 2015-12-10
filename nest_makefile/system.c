#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
	printf("return: %d\n", system("tftp 172.16.0.194 -c get phonebook1.xml"));
	return 0;
}
