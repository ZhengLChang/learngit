#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

int main(void)
{
	char str[] = "start=10/1/7/2:00:00;end=4/1/7/2:00:00;save=+1";
	int start_month, stop_month;
	int n = -1;
	n = sscanf(str, "start=%d/%*d/%*d/%*d:%*d:%*d;end=%d/", &start_month,
			&stop_month);
#if 0
	char s[100], c;
	sscanf(str, "%[^=]%c%d", s, &c, &start_month);
	sscanf(str + 10, "%[^=]%c%d", s, &c, &stop_month);
	printf("ch = %s\n", s);
#endif
	printf("start_month = %d, stop_month = %d\n", start_month, 
			stop_month);
	printf("n = %d\n", n);
	return 0;
}
