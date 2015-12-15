#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>	
#include <error.h>
#include <arpa/inet.h>
#define MAXLINE 80
#define SERV_PORT 8000
#define MAX(a, b) ((a)>(b)?(a):(b))

struct cliMessageRecord
{
	char clientIp[100];
	int clientPort;
	char filename[100];
	FILE *fileDesc;
        int sockDesc;
	int filesize;
	int getsize;
};

int main(void)
{
	int sock, listenfd, connfd;
	socklen_t cliaddr_len; 
	struct sockaddr_in bindaddr, cliaddr;
	int n = 0, i, readyNum = 0, maxfd;
	fd_set readfds_all, readfds_temp;
	struct cliMessageRecord clientfds[FD_SETSIZE]; 
	char buf[MAXLINE] = "";
	
	memset(&bindaddr, 0, sizeof(bindaddr));
	sock = socket(AF_INET, SOCK_STREAM, 0);

	bindaddr.sin_family = AF_INET;
	bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bindaddr.sin_port = htons(SERV_PORT);
	if(-1 == bind(sock, (struct sockaddr *)&bindaddr, sizeof(bindaddr)))
	{
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		return -1;
	}
	listen(sock, 20);


	memset(&clientfds, 0, sizeof(clientfds));
	for(i = 0; i < FD_SETSIZE; i++)
		clientfds[i].sockDesc = -1;
	FD_ZERO(&readfds_all);
	FD_SET(sock, &readfds_all);
	maxfd = sock;

	for(;;)
	{
		readfds_temp = readfds_all;
		readyNum = select(maxfd + 1, &readfds_temp, NULL, NULL, NULL);
		if(readyNum < 0)
			continue;
		if(FD_ISSET(sock, &readfds_temp))
		{
			cliaddr_len = sizeof(cliaddr);
			if((connfd = accept(sock, (struct sockaddr*)&cliaddr, &cliaddr_len)) < 0)
			{
				printf("accept error: %s\n", strerror(errno));
			}
			else
			{
				for(i = 0; i < FD_SETSIZE; i++)
				{ 
					if(clientfds[i].sockDesc == -1)
						break;
				}
				if(i >= FD_SETSIZE)
				{
					printf("too many linkers, server cannot deal with them");
					continue;
				}
				if(NULL == inet_ntop(AF_INET, &(cliaddr.sin_addr), clientfds[i].clientIp, sizeof(clientfds[i].clientIp)))
				{
					printf("inet_ntop error: %s\n", strerror(errno));
					continue;
				}
				else
				{
					clientfds[i].clientPort = ntohs(cliaddr.sin_port);
					printf("build new link: client ip %s, port %d\n", clientfds[i].clientIp, clientfds[i].clientPort);
				}
				clientfds[i].sockDesc = connfd;
				maxfd = MAX(maxfd, connfd);
				FD_SET(connfd, &readfds_all);
			}
			if(--readyNum <= 0)
				continue;
		}
		for(i = 0; i < FD_SETSIZE && readyNum > 0; i++)
		{
			if(clientfds[i].sockDesc == -1)
				continue;
			if(FD_ISSET(clientfds[i].sockDesc, &readfds_temp))
			{
				if(clientfds[i].getsize == 0)
				{
					n = read(clientfds[i].sockDesc, buf, 3);
					buf[n] = '\0';
					if(atoi(buf) <= 0)
					{
						close(clientfds[i].sockDesc);
						FD_CLR(clientfds[i].sockDesc, &readfds_all);
						bzero(&clientfds[i], sizeof(clientfds[i]));
						clientfds[i].sockDesc = -1;
						continue;
					}
					if((n = read(clientfds[i].sockDesc, clientfds[i].filename, atoi(buf))) == atoi(buf))
					{
						if((clientfds[i].fileDesc = fopen(clientfds[i].filename, "w")) == NULL)
						{
							close(clientfds[i].sockDesc);
							fclose(clientfds[i].fileDesc);
							FD_CLR(clientfds[i].sockDesc, &readfds_all);
							bzero(&clientfds[i], sizeof(clientfds[i]));
							clientfds[i].sockDesc = -1;
							readyNum--;
							continue;
						}
					}
					else 
					{
						close(clientfds[i].sockDesc);
						FD_CLR(clientfds[i].sockDesc, &readfds_all);
						bzero(&clientfds[i], sizeof(clientfds[i]));
						clientfds[i].sockDesc = -1;
						readyNum--;
						continue;
					}
					clientfds[i].getsize = n;	
					/*get file size*/
					n = read(clientfds[i].sockDesc, buf, 9);
					buf[n] = '\0';
					clientfds[i].filesize = atoi(buf) + clientfds[i].getsize;
					if(clientfds[i].filesize <= clientfds[i].getsize)
					{
						close(clientfds[i].sockDesc);
						fclose(clientfds[i].fileDesc);
						FD_CLR(clientfds[i].sockDesc, &readfds_all);
						bzero(&clientfds[i], sizeof(clientfds[i]));
						clientfds[i].sockDesc = -1;
						readyNum--;
						continue;
					}
					--readyNum; 
				}
				else
				{
						if((n = read(clientfds[i].sockDesc, buf, MAXLINE)) < 0)
						{
							continue;
						}
						else if(n == 0)
						{
							if(clientfds[i].getsize == clientfds[i].filesize)
								printf("all size get\n");
							printf("file receive success: client Ip %s, port %d, file name %s\n",
									clientfds[i].clientIp,
									clientfds[i].clientPort,
									clientfds[i].filename);
							close(clientfds[i].sockDesc);
							fclose(clientfds[i].fileDesc);
							FD_CLR(clientfds[i].sockDesc, &readfds_all);
							bzero(&clientfds[i], sizeof(clientfds[i]));
							clientfds[i].sockDesc = -1;
							readyNum--;
							continue;
						}
						write(fileno(clientfds[i].fileDesc), buf, n);
						clientfds[i].getsize += n;
				}
			}
		}
	}
	return 0;
}











