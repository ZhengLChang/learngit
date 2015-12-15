#include <unp.h>

int main(int argc, char **argv)
{
	struct in_addr dst;
	struct hostent *phost;
	char **ppstr;
	char str[INET_ADDRSTRLEN];

	for(;--argc > 0; )
	{
		if(inet_pton(AF_INET, *++argv, &dst) <= 0)
		{
			printf("inet_pton error: %s\n", strerror(errno));
			return -1;
		}
		if((phost = gethostbyaddr(&dst, sizeof(dst), AF_INET)) == NULL)
		{
			printf("gethostbyaddr error: %s\n", hstrerror(h_errno));
			return -1;
		}
		printf("official name is %s\n", phost->h_name);
		for(ppstr = phost->h_aliases; *ppstr != NULL; ppstr++)
			printf("alias: %s\n", *ppstr);
		switch(phost->h_addrtype)
		{
			case AF_INET:
				for(ppstr = phost->h_addr_list; *ppstr != NULL; ppstr++)
				{
					printf("addr_list: %s\n", inet_ntop(AF_INET, *ppstr,
								str, INET_ADDRSTRLEN));
				}
				break;
			default:
				err_ret("unknow address");
				break;
		}

	}
	return 0;
}
