#include <stdio.h>
int main()
{
	char str[] = "commonValidMsg = {";
	char str_name[1024 + 1], str_value[1024 + 1];
	if(sscanf(str, "%[^:]: '%[^\']'", str_name, str_value) == 2)
	{
		printf("name = %s\nvalue = %s\n", str_name, str_value);
	}
	else if(sscanf(str, "%[^=] = %s\n", str_name, str_value) == 2)
		printf("name = %s\nvalue = %s\n", str_name, str_value);
	return 0;
}
