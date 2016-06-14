#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	struct s
	{
		int i;
		char c;
		int d;
		char a[0];
	};
	printf("offsets: i=%ld; c=%ld; d=%ld; a=%ld\n",
			(long)offsetof(struct s, i),
			(long)offsetof(struct s, c),
			(long)offsetof(struct s, d),
			(long)offsetof(struct s, a));
	printf("sizeof(struct s) = %ld\n", (long)sizeof(struct s));
	exit(EXIT_SUCCESS);
}
