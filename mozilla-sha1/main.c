#include "sha1.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	SHA_CTX c;
	unsigned char real_sha1[20];
	SHA1_Init(&c);
	SHA1_Update(&c, argv[1], strlen(argv[1]));
	SHA1_Final(real_sha1, &c);
	printf("%s\n", sha1_to_hex(real_sha1));
	return 0;
}
