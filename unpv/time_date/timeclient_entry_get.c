#include <unp.h>

int main(int argc, char **argv)
{
	int sockfd, n;
	struct sockaddr_in servaddr;
	struct in_addr *inetaddrp[2];
	struct in_addr **pptr;
	struct hostent *hp;
	struct servent *sp;
	struct in_addr inetaddr;
	socklen_t len;
	char recvline[MAXLINE];
	char buf[INET_ADDRSTRLEN];
	if(argc != 3)
	{
		printf("usage: %s [hostname] [service]\n", argv[0]);
		return -1;
	}
	if((hp = gethostbyname(argv[1])) == NULL)
	{
		if(inet_pton(AF_INET, argv[1], &inetaddr) != 1)
		{
			printf("inet_pton error: %s\n", hstrerror(h_errno));
			return -1;
		}
		else
		{
			inetaddrp[0] = &inetaddr;
			inetaddrp[1] = NULL;
			pptr = inetaddrp;
		}
	}
	else
	{
		pptr = (struct in_addr **)hp->h_addr_list;
	}
	if((sp = getservbyname(argv[2], "tcp")) == NULL)
		err_quit("getsevbyname error for %s", argv[2]);
	for(; *pptr != NULL; pptr++)
	{
		if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("socket error: %s\n", strerror(errno));
			return -1;
		}
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = sp->s_port;
		memmove(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
		len = sizeof(buf);
		printf("trying %s:%d\n", 
			inet_ntop(AF_INET, &servaddr.sin_addr, buf, len),
			servaddr.sin_port);
		if(connect(sockfd, (struct sockaddr *)&servaddr,
					sizeof(servaddr)) == 0)
			break;
		else
			printf("connect error: %s\n", strerror(errno));
		close(sockfd);
	}
	if(*pptr == NULL)
		err_quit("unable to conncet");
	while((n = read(sockfd, recvline, MAXLINE)) > 0)
	{
		recvline[n] = 0;
		fputs(recvline, stdout);
	}
	exit(0);
}











