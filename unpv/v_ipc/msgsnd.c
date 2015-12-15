#include <unp.h>
#if 0
struct mymsgbuf
{	
	long int mtype;
	char mtext[200];
};
#endif
int main(int argc, char **argv)
{
	int mqid;
	size_t len;
	struct msgbuf *ptr;
	long type;
	key_t mykey;

	if(argc != 4)
	{
		err_quit("usage:%s <pathname> <#bytes> <type>", argv[0]);
	}
	len = atol(argv[2]);
	type = atoi(argv[3]);

	if((mykey = ftok(argv[1], 0)) < 0)	
	{
		err_quit("ftok error: %s", strerror(errno));
	}
	if((mqid = msgget(mykey, MSG_W)) < 0)
	{
		err_quit("msgget error: %s", strerror(errno));
	}
	if((ptr = calloc(sizeof(long) + len, sizeof(char))) == NULL)
	{
		err_quit("calloc error: %s", strerror(errno));
	}
	memset(ptr->mtext, 'a', len);
	ptr->mtype = type;
	if((msgsnd(mqid, (void *)ptr, len, 0)) < 0)
	{
		err_quit("msgsnd error: %s", strerror(errno));
	}
	
	if(NULL != ptr)
	{
		free(ptr);
		ptr = NULL;
	}

	return 0;
}
