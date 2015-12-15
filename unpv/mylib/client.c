#include <unp.h>

void client(int readfd, int writefd)
{
	size_t len;
	ssize_t n;
	struct mymesg mesg;

	if(NULL == fgets(mesg.mesg_data, MAXMESGDATA, stdin))
	{
		printf("fgets error: %s\n", strerror(errno));
		return;
	}
	len = strlen(mesg.mesg_data);
	if(mesg.mesg_data[len - 1] == '\n')
		len--;
	mesg.mesg_len = len;
	mesg.mesg_type = 1;

	Mesg_send(writefd, &mesg);

	while((n = Mesg_recv(readfd, &mesg)) > 0)
	{
		if(n != write(STDOUT_FILENO, mesg.mesg_data, n))
		{
			printf("write error: %s\n", strerror(errno));
			return;
		}
	}
	return;
}
