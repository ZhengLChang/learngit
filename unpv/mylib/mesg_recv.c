#include <unp.h>

#define ISPIPE 0

#if ISPIPE
ssize_t mesg_recv(int fd, struct mymesg *mptr)
{
	size_t len;
	ssize_t n;

	if((n = read(fd, mptr, MESGHDRSIZE)) == 0)
	{
		return 0;
	}
	else if(n != MESGHDRSIZE)
	{
		err_quit("message header: expected %d, got %d", MESGHDRSIZE, n);
	}
	if((len = mptr->mesg_len) > 0)
	{
		if((n = read(fd, mptr->mesg_data, len)) != len)
		{
			err_quit("message data: expected %d, got %d", len, n);
		}
	}
	return len;
}

ssize_t Mesg_recv(int fd, struct mymesg *mptr)
{
	return(mesg_recv(fd, mptr));
}
#else
ssize_t mesg_recv(int id, struct mymesg *mptr)
{
	ssize_t n;
	n = msgrcv(id, &(mptr->mesg_type), MAXMESGDATA, mptr->mesg_type, 0);
	mptr->mesg_len = n;
	return n;
}
ssize_t Mesg_recv(int id, struct mymesg *mptr)
{
	ssize_t n;
	if((n = mesg_recv(id, mptr)) == -1)
	{
		printf("mesg_recv error: %s\n", strerror(errno));
	}
	return n;
}
#endif
