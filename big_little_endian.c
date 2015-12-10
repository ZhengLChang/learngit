#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
void sigalrm(int signo);
int never_ending_program();

int endflag = 0;
#define TIMEOUT 1
int main(void)
{
	uint32_t i;
	unsigned char *cp;
	struct sigaction act;

	i = 0x04030201;
	cp = (unsigned char *)&i;
	if(*cp == 1)
		printf("little-endian\n");
	else if(*cp == 4)
		printf("big-endian\n");
	else
		printf("who knows\n");
	
	bzero(&act, sizeof(act));
	act.sa_handler = sigalrm;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	if(0 != sigaction(SIGALRM, &act, NULL))
	{
		printf("sigaction: %s\n", strerror(errno));
		return -1;
	}	
	alarm(TIMEOUT);
	never_ending_program();
	alarm(0);
	printf("%s is ending\n", "never_ending_program");
	
	exit(0);
}

void sigalrm(int signo)
{
	printf("signal of alarm\n");
	endflag = 1;
	sleep(1);
}


int never_ending_program()
{
	int i = 0;
	for(i = 0; !endflag; i++)
	{
		printf("in %s: print %d\n", __FUNCTION__, i);
		
		usleep(1000 * 30);
	}
	return 0;
}















