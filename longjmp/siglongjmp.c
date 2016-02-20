#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>
#include <unistd.h>
#include <time.h>

static void sig_usr1(int);
static void sig_alrm(int);
void pr_mask(const char *str);
static sigjmp_buf jmpbuf;
static volatile sig_atomic_t canjump;

int main(int argc, char **argv)
{
	struct sigaction act_usr1, act_alrm;

	if(sigemptyset(&act_usr1.sa_mask) < 0)
	{
		printf("sigemptyset error: %s\n", strerror(errno));
		return -1;
	}
	act_usr1.sa_flags = 0;
	act_usr1.sa_handler = sig_usr1;
	if(sigaction(SIGUSR1, &act_usr1, NULL) < 0)
	{
		printf("sigaction error: %s\n", strerror(errno));
		return -1;
	}
	

	if(sigemptyset(&act_alrm.sa_mask) < 0)
	{
		printf("sigemptyset error: %s\n", strerror(errno));
		return -1;
	}
	act_alrm.sa_flags = 0;
	act_alrm.sa_handler = sig_alrm;
	if(sigaction(SIGALRM, &act_alrm,NULL) < 0)
	{
		printf("sigaction error: %s\n", strerror(errno));
		return -1;
	}
	
	pr_mask("starting main: ");
	if(sigsetjmp(jmpbuf, 0))
	{
		pr_mask("ending main: ");
		exit(0);
	}
	canjump = 1;
	for(;;)
		pause();
}

static void sig_usr1(int signo)
{
	time_t starttime;
	if(canjump == 0)
		return;
	pr_mask("starting sig_usr1: ");
	alarm(3);
	starttime = time(NULL);
	for(;;)
	{
		if(time(NULL) > starttime + 5)
			break;
	}
	pr_mask("finishing sig_usr: ");
	canjump = 0;
	siglongjmp(jmpbuf, 0);
}

static void sig_alrm(int signo)
{
	pr_mask("in sig_alrm: ");
}

void pr_mask(const char *str)
{
	sigset_t sigset;
	int errno_save;
	
	errno_save = errno;
	if(sigprocmask(0, NULL, &sigset) < 0)
	{
		printf("sigprocmask error\n");
		return ;
	}
	printf("%s", str);
	if(sigismember(&sigset, SIGINT) == 1)
	{
		printf("SIGINT ");
	}
	if(sigismember(&sigset, SIGQUIT) == 1)
	{
		printf("SIGQUIT ");
	}
	if(sigismember(&sigset, SIGUSR1) == 1)
	{
		printf("SIGUSR1 ");
	}
	if(sigismember(&sigset, SIGALRM) == 1)
	{
		printf("SIGALRM ");
	}
	printf("\n");
	errno = errno_save;
}













