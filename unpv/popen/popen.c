#include <unp.h>

int main(int argc, char **argv)
{
	size_t n;
	char buf[MAXLINE], command[MAXLINE];

	FILE *fp;

	if(fgets(buf, MAXLINE, stdin) == NULL)
	{
		printf("fgets error: %s\n", strerror(errno));
		return -1;
	}
	n = strlen(buf);
	buf[n - 1] == '\n' ? n-- : n;
	snprintf(command, sizeof(command), "cat %s", buf);
	if((fp = popen(command, "r")) == NULL)
	{
		printf("popen error: %s\n", strerror(errno));
		return -1;
	}
	while(fgets(buf, MAXLINE, fp) != NULL)
	{
		fputs(buf, stdout);
	}
	pclose(fp);
	exit(0);
}















