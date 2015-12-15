#include <filetransfer.h>

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

struct mymesg msgStore;

int main(void)
{
	int sock, connfd;
	socklen_t cliaddr_len; 
	struct sockaddr_in bindaddr, cliaddr;
	int n = 0, i, readyNum = 0, maxfd;
	fd_set readfds_all, readfds_temp;
	struct cliMessageRecord clientfds[FD_SETSIZE]; 

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
					{
						break;
					}
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
				if((n = read(clientfds[i].sockDesc, &msgStore, sizeof(msgStore))) > 0)
				{
					if(0 == msgStore.mesg_type)
					{
						/*
						if(clientfds[i].getsize != 0)
						{
							printf("[FUNCTION %s][LINE %d]\n", __FUNCTION__, __LINE__);
							goto CLOSEFILE;
						}
						*/
						if((clientfds[i].fileDesc = fopen(msgStore.mesg_attribute.fileName, "w")) == NULL)
						{
							printf("[FUNCTION %s][LINE %d]\n", __FUNCTION__, __LINE__);
							printf("len = %ld\n", msgStore.mesg_len);
							printf("fopen %s error: %s\n", msgStore.mesg_attribute.fileName, strerror(errno));
							goto CLOSEFILE;
						}
						else
						{
							printf("len = %ld\n", msgStore.mesg_len);
							printf("filename %s\n", msgStore.mesg_attribute.fileName);
						}
						memcpy(clientfds[i].filename, msgStore.mesg_attribute.fileName, strlen(msgStore.mesg_attribute.fileName));
						clientfds[i].filesize = msgStore.mesg_attribute.fileLength;
						--readyNum; 
					}
					else if(1 == msgStore.mesg_type)
					{
						if(msgStore.mesg_len == 0)
						{
							if(clientfds[i].getsize == clientfds[i].filesize)
								printf("all size get\n");
							printf("file receive success: client Ip %s, port %d, file name %s\n",
									clientfds[i].clientIp,
									clientfds[i].clientPort,
									clientfds[i].filename);
							fclose(clientfds[i].fileDesc);
							goto CLOSEFILE;
						}
						write(fileno(clientfds[i].fileDesc), msgStore.mesg_data, n);
						clientfds[i].getsize += n;
						readyNum--;

					}
					else
					{
						printf("unknow file type\n");
						readyNum--;
					}
					continue;
			CLOSEFILE:
					close(clientfds[i].sockDesc);
					FD_CLR(clientfds[i].sockDesc, &readfds_all);
					bzero(&clientfds[i], sizeof(clientfds[i]));
					clientfds[i].sockDesc = -1;
					readyNum--;
				}

			}
		}
	}
	return 0;
}











