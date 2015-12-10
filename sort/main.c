#include <sort.h>

void (*sortProc)(int*, int) = NULL;

int main(int argc, char **argv)
{
	int oc;
	int arr[] = {34, 8, 64, 51, 32, 21};
	if(argc != 2)
	{
		printf("usage: %s sortFunction(-i(insertion sort))\n", 
				argv[0]);
		return -1;
	}
	while((oc = getopt(argc, argv, "i")) != -1)
	{
		switch(oc)
		{
			case 'i':
				sortProc = InsertionSort;
				break;
			default:
				printf("option is valid!!!\n");
				break;
		}
	}
	if(InsertionSort != NULL)
	{
		sortProc(arr, sizeof(arr)/sizeof(int));
	}
	return 0;
}








