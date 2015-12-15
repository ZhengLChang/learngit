#include <unp.h>

void server(int readfd, int writefd)
{
	FILE *fp = NULL;
	ssize_t n;
	struct mymesg mesg;

	mesg.mesg_type = 1;
	if((n = Mesg_recv(readfd, &mesg)) == 0)
		err_quit("pathname missing");
	mesg.mesg_data[n] = '\0';
	if((fp = fopen(mesg.mesg_data, "r")) == NULL)
	{
		snprintf(mesg.mesg_data + n, sizeof(mesg.mesg_data) - n,
				" : can't open, %s\n", strerror(errno));
		mesg.mesg_len = strlen(mesg.mesg_data);
		Mesg_send(writefd, &mesg);
	}
	else
	{
		while(fgets(mesg.mesg_data, MAXMESGDATA, fp) != NULL)
		{
			mesg.mesg_len = strlen(mesg.mesg_data);
			Mesg_send(writefd, &mesg);
		}
		if(NULL != fp)
		{
			fclose(fp);
			fp = NULL;
		}
	}
	mesg.mesg_len = 0;
	Mesg_send(writefd, &mesg);
}
