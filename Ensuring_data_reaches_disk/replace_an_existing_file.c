#include "sync-samples.h"

#define TEMPLATE "mynewfileXXXXXX"
char *template;
int template_len;

const char *message1 = "Version 1 of my data.\n";
const char *message2 = "Version 2 of my data.\n";

int main(int argc, char **argv)
{
	int ret;
	size_t message_len;
	int fd, new_fd, dir_fd;
	mode_t old_mode;
	char *path, *containing_dir;
	if(argc < 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", basename(argv[0]));
		exit(USER_ERR);
	}
	path = strdup(argv[1]);
	if(!path)
	{
		perror("strdup");
		exit(LIB_ERR);
	}
	containing_dir = dirname(path);
	old_mode = umask((mode_t)0);
	fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd < 0)
	{
		perror("open");
		exit(SYS_ERR);
	}
	umask(old_mode);
	dir_fd = open(containing_dir, O_RDONLY);
	if(dir_fd < 0)
	{
		perror("open");
		exit(SYS_ERR);
	}
	message_len = strlen(message1);
	ret = full_write(fd, message1, message_len);
	if(ret != (int)message_len)
	{
		if(ret < 0)
		{
			perror("write");
			exit(SYS_ERR);
		}
		if(unlink(argv[1]) < 0)
			perror("unlink");
		exit(SYS_ERR);
	}
	if(fsync(fd) < 0)
	{
		perror("fsync");
		exit(SYS_ERR);
	}
	if(fsync(dir_fd) < 0)
	{
		perror("fsync2");
		exit(SYS_ERR);
	}
	if(close(fd) < 0)
	{
		perror("close");
		exit(SYS_ERR);
	}
	/*version 1 over*/
	/*version 2 begin*/
	template_len = strlen(containing_dir) + strlen(TEMPLATE) + 2;
	template = malloc(template_len);
	if(!template)
	{
		perror("malloc");
		exit(SYS_ERR);	
	}
	ret = snprintf(template, template_len, "%s/%s", 
			containing_dir, TEMPLATE);
	if(ret >= template_len)
	{
		fprintf(stderr, "Internal Error\n");
		exit(INTERNAL_ERR);
	}
	new_fd = mkstemp(template);
	if(new_fd == -1)
	{
		perror("mkstemp");
		exit(SYS_ERR);
	}
	message_len = strlen(message2);
	ret = full_write(new_fd, message2, message_len);
	if(ret != (int)message_len)
	{
		if(ret < 0)
		{
			perror("write");
			exit(SYS_ERR);
		}
		if(unlink(template) < 0)
			perror("unlink");
		exit(SYS_ERR);
	}
	if(fsync(new_fd) < 0)
	{
		perror("fsync");
		exit(SYS_ERR);
	}
	if(close(new_fd) < 0)
	{
		perror("close");
		exit(SYS_ERR);
	}
	if(rename(template, argv[1]) < 0)
	{
		perror("rename");
		exit(SYS_ERR);
	}
	free(template);
	if(fsync(dir_fd) < 0)
	{
		perror("fsync dir_fd");
		exit(SYS_ERR);
	}
	if(close(dir_fd) < 0)
	{
		perror("close dir_fd");
		exit(SYS_ERR);
	}
	free(path);
	return 0;
}



















