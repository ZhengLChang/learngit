#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
	printf("%d\n", system("cd ..; tftp 172.16.0.2 -c get phonebook8.xml; cd -"));
	printf("%d\n", system("tftp 172.16.0.23 -c get phonebook8.xml"));
	return 0;
}
