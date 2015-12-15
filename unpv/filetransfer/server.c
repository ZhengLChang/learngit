#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#define MAXLINE 80
#define SERV_PORT 8000
int main(void)
{
	int sock, listenfd, connfd;
	socklen_t cliaddr_len; 
	struct sockaddr_in bindaddr, cliaddr;
	pid_t mypid;
	int n = 0;
	char filename[100] = "";
	char buf[MAXLINE] = "";
	int filesize = 0, getsize = 0;

printf("%s %d\n", __FUNCTION__, __LINE__);

	memset(&bindaddr, 0, sizeof(bindaddr));
	sock = socket(AF_INET, SOCK_STREAM, 0);

	bindaddr.sin_family = AF_INET;
	bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bindaddr.sin_port = htons(SERV_PORT);
printf("%s %d\n", __FUNCTION__, __LINE__);

	if(-1 == bind(sock, (struct sockaddr *)&bindaddr, sizeof(bindaddr)))
	{
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		return -1;
	}

printf("%s %d\n", __FUNCTION__, __LINE__);
	listen(sock, 20);

	while(1)
	{
		cliaddr_len = sizeof(cliaddr);
		printf("waiting....\n");
		connfd = accept(sock, (struct sockaddr*)&cliaddr, &cliaddr_len);
		if(connfd == -1)
		{
			printf("accept error\n");
			continue;
		}
		mypid = fork();
		if(mypid < 0)
		{
			return -1;
		}
		else if(mypid == 0)
		{
			printf("is connected\n");
			close(sock);
			FILE *fp = NULL;
			/*get the file name size*/
			n = read(connfd, buf, 3);
			buf[n] = '\0';
			if(atoi(buf) <= 0 )
			{
				fclose(fp);
				close(connfd);
				return -1;
			}
			n = read(connfd, filename, atoi(buf));
			printf("%s\n", filename);
			if(n > 0)
				fp = fopen(filename, "w");
			else 
			{
				fclose(fp);
				close(connfd);
				return -1;
			}
			/*get the file size*/
			n = read(connfd, buf, 9);
			buf[n] = '\0';
			filesize = atoi(buf);
			printf("get string is %s, file size is %d\n", buf, filesize);
			if(filesize <= 0)
			{
				fclose(fp);
				close(connfd);
				return -1;
			}
			printf("get file, filename: %s, filesize = %d\n", 
					filename, filesize);
			getsize = 0;
			while(getsize < filesize)
			{
				n = read(connfd, buf, MAXLINE);
				if(n < 0)
				{
					fclose(fp);
					close(connfd);
					return -1;
				}
			//	buf[n] = '\0';
			//	printf("%s\n", buf);
				fwrite(buf, 1, n, fp);
				getsize += n;
			}
			printf("file receive success\n");
			fclose(fp);
			close(connfd);
			return 0;
		}
	}
	return 0;
	
}











