#include <unp.h>

int main(int argc, char **argv)
{
	char *ptr, **pptr;
	struct hostent *hptr;
	char str[INET_ADDRSTRLEN];
	while(--argc > 0)
	{
		ptr = *++argv;
		if((hptr = gethostbyname(ptr)) == NULL)
		{
			err_msg("gethostbyname error for host: %s: %s", 
					ptr, hstrerror(h_errno));
			continue;
		}
		printf("official hostname: %s\n", hptr->h_name);

		for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
			printf("\talias: %s\n", *pptr);
		switch(hptr->h_addrtype)
		{
			case AF_INET:
				pptr = hptr->h_addr_list;
				for(; *pptr != NULL; pptr++)
				{
					printf("\t%d address: %s\n", 
						((struct in_addr *)(*pptr))->s_addr,
						inet_ntop(AF_INET, *pptr, str, INET_ADDRSTRLEN));
				}
				break;
			default:
				err_ret("unknown address type");
				break;
		}
	}
	exit(0);
}
