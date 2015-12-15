#include <unp.h>

void client_pipe(int , int), server_pipe(int, int);

int main(int argc, char **argv)
{
	int pipe1[2], pipe2[2];
	pid_t childpid;

	if(pipe(pipe1) < 0)
	{
		printf("creat pipe1 error: %s\n", strerror(errno));
		return -1;
	}
	if(pipe(pipe2) < 0)
	{
		printf("creat pipe2 error: %s\n", strerror(errno));
		return -1;
	}
	if((childpid = fork()) < 0)
	{
		printf("fork error: %s\n", strerror(errno));
		return -1;
	}
	else if(childpid == 0)
	{
		close(pipe1[1]);
		close(pipe2[0]);
		server_pipe(pipe1[0], pipe2[1]);
		exit(0);
	}
	close(pipe1[0]);
	close(pipe2[1]);

	client_pipe(pipe2[0], pipe1[1]);
	waitpid(childpid, NULL, 0);
	exit(0);
}














