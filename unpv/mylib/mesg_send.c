#include <unp.h>

#define ISPIPE 0

#if ISPIPE
ssize_t mesg_send(int fd, struct mymesg *mptr)
{
	return(write(fd, mytr, MESGHDRSIZE + mptr->mesg_len));
}
void Mesg_send(int fd, struct mymesg *mptr)
{
	ssize_t n;
	if((n = mesg_send(fd, mptr)) != mptr->mesg_len)
		err_quit("mesg_send error");
}
#else
ssize_t mesg_send(int id, struct mymesg *mptr)
{
	return(msgsnd(id, &(mptr->mesg_type), mptr->mesg_len, 0));
}
void Mesg_send(int id, struct mymesg *mptr)
{
	ssize_t n;
	if((n = mesg_send(id, mptr)) == -1)
		err_quit("mesg_send error");
}
#endif
