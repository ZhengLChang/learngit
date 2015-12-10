#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int _system_timeout(const char *cmdstring, int timeout); /*one hundred of a second*/
int system_timeout(const char *cmdstring, int timeout); 

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("usage: %s [command]\n", argv[0]);
	}
	if(0 != system_timeout(argv[1], 5))
	{
		printf("system_timeout error\n");
		return -1;
	}
	printf("system_timeout success\n");
	return 0;
}


int _system_timeout(const char *cmdstring, int timeout)
{
	pid_t pid;
	int status;
	struct sigaction ignore, saveintr, savequit;
	sigset_t chldmask, savemask;

	if(cmdstring == NULL)
		return 1;
	ignore.sa_handler = SIG_IGN;
	sigemptyset(&ignore.sa_mask);
	ignore.sa_flags = 0;
	if(sigaction(SIGINT, &ignore, &saveintr) < 0)
	{
		return -1;
	}
	if(sigaction(SIGQUIT, &ignore, &savequit) < 0)
	{
		return -1;
	}
	sigemptyset(&chldmask);
	sigaddset(&chldmask, SIGCHLD);
	if(sigprocmask(SIG_BLOCK, &chldmask, &savemask) < 0)
	{
		return -1;
	}
	if((pid = fork()) < 0)
	{
		return -1;
	}
	else if(pid == 0)
	{
		sigaction(SIGINT, &saveintr, NULL);
		sigaction(SIGQUIT, &savequit, NULL);
		sigprocmask(SIG_SETMASK, &savemask, NULL);
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		_exit(127);
	}
	else
	{
		usleep(timeout * 10000);
		kill(pid, SIGKILL);
		waitpid(pid, &status, 0);
	}
	if(sigaction(SIGINT, &saveintr, NULL) < 0)
	{
		return -1;
	}
	if(sigaction(SIGQUIT, &savequit, NULL) < 0)
		return -1;
	if(sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
		return -1;
	return status;
}


int system_timeout(const char *cmdstring, int timeout) 
{
	int ret = 0;
	struct sigaction chldnew, chldold;
	if(cmdstring == NULL)
		return -1;
	bzero(&chldnew, sizeof(struct sigaction));
	bzero(&chldold, sizeof(struct sigaction));
	chldnew.sa_handler = SIG_DFL;
	sigemptyset(&chldnew.sa_mask);
	chldnew.sa_flags = 0;
	if(sigaction(SIGCHLD, &chldnew, &chldold) < 0)
	{
		return -1;
	}
	if(timeout * 10000 < 0)
	{
		return -1;
	}
	else if(timeout == 0)
	{
		ret = system(cmdstring);
	}
	else
	{
		ret = _system_timeout(cmdstring, timeout);
	}
	if(sigaction(SIGCHLD, &chldold, NULL) < 0)
	{
		return -1;
	}
	return ret;
}






















