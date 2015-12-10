#include <editDistance.h>


int main(int argc, char **argv)
{
	if(argc != 3)
	{
		printf("usage: %s string1 string2\n", argv[0]);
		return -1;
	}
	printf("edit distance between %s and %s : %d\n",
			argv[1], argv[2],
			EditDistance(argv[1], strlen(argv[1]),
			             argv[2], strlen(argv[2])));
	return 0;
}
