#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char *get_system_ret(char *cmd);

int main(int argc, char *argv[])
{
	char *ret_str = NULL;
	if(argc < 2)
	{
		printf("usage: ./%s [commond]\n", argv[0]);
		return -1;
	}
	if(NULL != (ret_str = get_system_ret(argv[1])))
	{
		printf("%s", ret_str);
		free(ret_str);
		ret_str = NULL;
	}
	return 0;
}

char *get_system_ret(char *cmd)
{
	FILE *fp = NULL;
	char buf[1024];
	char *ret_str = NULL;
	if(NULL == cmd)
	{
		return NULL;
	}
	if(NULL  == (fp = popen(cmd, "r")))
	{
		return NULL;
	}
	if(fgets(buf, sizeof(buf), fp) != NULL)
	{
		ret_str = strdup(buf);
	}
	pclose(fp);
	fp = NULL;
	return ret_str;
}







