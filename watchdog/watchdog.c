#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <errno.h>

#define WDT "/dev/watchdog"

int main(int argc, char **argv)
{
	char *isreboot = NULL;
	int wd = -1, timeout = 10;

	if((wd = open(WDT, O_WRONLY)) < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	isreboot = getenv("isreboot");
	printf("isreboot flag is: %s\n", isreboot);
	if(ioctl(wd, WDIOC_SETTIMEOUT, &timeout) < 0)
	{
		printf("set timeout ioctl error: %s\n", strerror(errno));
		return -1;
	}

	switch(*isreboot)
	{
		case 'Y':
		case 'y':
		{
			while(1)
			{
				printf("will reboot, after %d seconds\n", timeout--);
				sleep(1);
			}
			break;
		}
		default:
		{
			int opt = WDIOS_DISABLECARD;
			printf("cancel watchdog\n");
			if(ioctl(wd, WDIOC_SETOPTIONS, &opt))
			{
				printf("ioctl error: %s\n", strerror(errno));
				return -1;
			}
			break;
		}
	}
	return 0;
}



















