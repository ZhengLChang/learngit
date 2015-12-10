#include <stdio.h>

void *memcpy(void *dest, void *src, int len)
{
	unsigned long int _ul_dest = (unsigned long)dest;
	unsigned long int _ul_src = (unsigned long)src;

	len -= (-_lu_sest) % 4;
	return NULL;
}

int main(void)
{
	char dest[100];
	char src[100] = "zheng";
	memcpy(dest, src, strlen(src));
}
