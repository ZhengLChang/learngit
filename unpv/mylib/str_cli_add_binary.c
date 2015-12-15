#include <unp.h>
#include <sum.h>

void str_cli_binary(FILE *fp, int sockfd)
{
	char sendline[MAXLINE];
	struct args args;
	struct result result;
	int i = 0;
	while(fgets(sendline, MAXLINE, fp) != NULL)
	{
		if(sscanf(sendline, "%ld%ld", &args.arg1, &args.arg2) != 2)
		{
			printf("invalid input: %s", sendline);
			continue;
		}
		for(i = 0; i < sizeof(struct args);)
		{
			printf("%02x ", ((unsigned char*)&args)[i]);
			i++;
			if(i % 4 == 0)
				printf("\n");
		}
		Writen(sockfd, &args, sizeof(args));
		if(Readn(sockfd, &result, sizeof(result)) <= 0)
			err_quit("str_cli: server terminated prematurely");
		printf("%ld\n", result.sum);
	}
}
