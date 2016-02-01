#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

int main(void)
{
	int sock;
	int rcvbuf = -1;
        socklen_t optlen;
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("socket error: %s\n", strerror(errno));
		return -1;
	}
	optlen = sizeof(rcvbuf);
	if(getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvbuf, &optlen) < 0)
	{
		printf("getsockopt error: %s\n", strerror(errno));
		return -1;
	}
	printf("rcvbuf: %d, len: %d\n", rcvbuf, optlen);
	return 0;
}
