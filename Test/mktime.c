#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv)
{
	struct tm local_time;
	memset(&local_time, 0, sizeof(local_time));
	local_time.tm_year = 2017 - 1900;
	local_time.tm_mon = 3;
	local_time.tm_mday = 1;
	printf("%ld\n", mktime(&local_time));
	return 0;
}
