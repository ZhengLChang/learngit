#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

/* Simple error handling functions */

#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

void sig_handler(int signo)
{
	printf("[FUNCTION %s]\n", __FUNCTION__);
}

static void *sig_thread(void *arg)
{
	sigset_t set;
	int s, sig;
	struct sigaction act;
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	pthread_sigmask(SIG_UNBLOCK, &set, NULL);
	bzero(&act, sizeof(act));
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = sig_handler;
	sigaction(SIGALRM, &act, NULL);
	alarm(1);
	printf("system is begin from child\n");
	system("tftp 172.16.0.2 -c get phonebook3.xml");
	printf("system is end from child\n");
		/*
	for (;;) 
	{
		s = sigwait(set, &sig);
		if (s != 0)
			handle_error_en(s, "sigwait");
		printf("Signal handling thread got signal %d\n", sig);
		printf("I am alive\n");
		sleep(1);
	}
*/
}

int main(int argc, char *argv[])
{
	pthread_t thread;
	sigset_t set;
	int s;
	/* Block SIGINT; other threads created by main() will inherit
	 * *               a copy of the signal mask. */
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGALRM);
	sigaddset(&set, SIGCHLD);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);
	if (s != 0)
		handle_error_en(s, "pthread_sigmask");
//	sigpending(&set);
	s = pthread_create(&thread, NULL, &sig_thread, (void *) &set);
	if (s != 0)
		handle_error_en(s, "pthread_create");
	/* Main thread carries on to create other threads and/or do
	 * *               other work */
#if 0
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);
	if (s != 0)
		handle_error_en(s, "pthread_sigmask");
#endif
	printf("system is begin\n");
	system("tftp 172.16.0.23 -c get phonebook.xml");
	printf("system is end\n");
	sleep(10);
	pause();            /* Dummy pause so we can test program */
}

