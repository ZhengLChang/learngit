#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define BUFLEN 128
#define TIMEOUT 20

void sigalrm(int signo)
{
}
void print_uptime(int sockfd, struct addrinfo *aip);

int main(int argc, char *argv[])
{
	struct addrinfo *ailist, *aip;
	struct addrinfo hint;
	int sockfd, err;
	struct sigaction sa;

	if(argc != 2)
	{
		printf("usage: ruptime hostname");
		return -1;
	}
	sa.sa_handler = sigalrm;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if(sigaction(SIGALRM, &sa, NULL) < 0)
	{
		printf("sigaction error: %s\n", strerror(errno));
		return -1;
	}
	bzero(&hint, sizeof(hint));
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_flags = AI_PASSIVE;
	if((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0)
	{
		printf("getaddrinfo error: %s", gai_strerror(err));
		return -1;
	}
	for(aip = ailist; aip != NULL; aip = aip->ai_next)
	{
		if((sockfd = socket(aip->ai_family, SOCK_DGRAM, 0)) < 0)
		{
			err = errno;
		}
		else
		{
			print_uptime(sockfd, aip);
			exit(0);
		}
	}
	fprintf(stderr, "can't contact %s: %s\n", argv[1], strerror(errno));
	exit(0);
}

void print_uptime(int sockfd, struct addrinfo *aip)
{
	int n;
	char buf[BUFLEN];

	buf[0] = 0;
	printf("addr port: %d\n", 
		ntohs(((struct sockaddr_in *)(aip->ai_addr))->sin_port));
	if(sendto(sockfd, buf, 1, 0, aip->ai_addr, aip->ai_addrlen) < 0)
	{
		printf("sendto %s error: %s\n", aip->ai_addr,
				strerror(errno));
		return;
	}
	alarm(TIMEOUT);
	if((n = recvfrom(sockfd, buf, BUFLEN, 0, NULL, NULL)) < 0)
	{
		if(errno != EINTR)
			alarm(0);
		printf("recvfrom error: %s\n",
				strerror(errno));
		return;
	}
	alarm(0);
	write(STDOUT_FILENO, buf, n);
}
















