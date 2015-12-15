#ifndef FILETRANSFER_H_
#define FILETRANSFER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>	
#include <error.h>
#include <arpa/inet.h>
#define MAXLINE 512
#define MAXDATAGRAMLENTH 512
#define SERV_PORT 8000
#define MAX(a, b) ((a)>(b)?(a):(b))
/*
 * if type is 0,data is filename;
 * if type is 1,data is filesize;
 * if type is 2, data is simple data
 * */
struct fileattribute
{
	long fileLength;
	char fileName[512];
};
struct mymesg
{
	long mesg_type;               
	long mesg_len;
	char mesg_data[MAXDATAGRAMLENTH];
	struct fileattribute mesg_attribute;
};
#endif
