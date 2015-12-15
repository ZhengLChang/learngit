#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#define MAXLINE 80
#define SERV_PORT 8000

char *reverseStr(char *str);
char *splitStr(const char const* sourceStr,         /*-I, source string*/
		const char const* flag,              /*-I, split flag*/
		int num);   /*-I, want to get the first n str*/

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "usage: %s [filename] [server ip]\n",
				argv[0]);
		return -1;
	}
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
	int sockfd, n;
	struct stat statbuf;
	char *filename = NULL;
	int filesize = 0;
	FILE *fp = fopen(argv[1], "r");
	if(NULL == fp)
	{
		fprintf(stderr, "open file error: %s\n", strerror(errno));
		return -1;
	}
	if(-1 == stat(argv[1], &statbuf))
	{
		fprintf(stderr, "stat file %s error: %s\n", argv[1], strerror(errno));
		return -1;
	}
	filesize = statbuf.st_size;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[2], &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);

	if(-1 == connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
	{
		fprintf(stderr, "connect error: %s\n", strerror(errno));
		return -1;
	}
	filename  = splitStr(argv[1], "/", 0);
	printf("write %s\n", filename);
	snprintf(buf, MAXLINE-1, "%03d%s", strlen(filename), filename);
	if((n = write(sockfd, buf, strlen(buf))) < 0)
	{
		fprintf(stderr, "write error: %s\n", strerror(errno));
		return -1;
	}
	printf("filesize %d\n", filesize);
	n = snprintf(buf, MAXLINE - 1, "%09d", filesize);
	printf("filesize %s\n", buf);
	do
	{
		if((n = write(sockfd, buf, n)) < 0)
		{
			fprintf(stderr, "write error: %s\n", strerror(errno));
			return -1;
		}
		usleep(10);
//		printf("write %s\n", buf);
	}while((n = fread(buf, 1, MAXLINE, fp)) > 0);
	return 0;
}



char *splitStr(const char const* sourceStr,         /*-I, source string*/
		const char const* flag,              /*-I, split flag*/
		int num)			    /*-I, want to get the first n str*/
{
	char *splitedString = strdup(sourceStr);
	char *splitingFlag = strdup(flag);
	char *storeDesStr = NULL;
	int i = 0, reverseflag = 0;
	if(num <= 0)
	{
		reverseStr(splitedString);
		reverseStr(splitingFlag);
		num = 0 - num + 1;
		reverseflag = 1;
	}
	storeDesStr = strtok(splitedString, splitingFlag);
	if(storeDesStr != NULL && num != 1)
	{
		for(i = 1; i < num && storeDesStr != NULL; i++)
		{
			storeDesStr = strtok(NULL, splitingFlag);
		}
	}
	if(storeDesStr == NULL)
		return NULL;

	storeDesStr = strdup(storeDesStr);
	if(reverseflag != 0)
		reverseStr(storeDesStr);
	free(splitedString);
	splitedString = NULL;
	free(splitingFlag);
	splitingFlag = NULL;
	return storeDesStr;
}

char *reverseStr(char *str)
{
	char swap = 0;
	int i = 0, strLen = 0;

	if(str == NULL || strlen(str) == 1)
	{
		return str;
	}
	strLen = strlen(str);
	for(i = 0; i < strLen / 2; i++)
	{
		swap = str[i];
		str[i] = str[strLen - i - 1];
		str[strLen - i - 1] = swap;
	}
	return str;
}










