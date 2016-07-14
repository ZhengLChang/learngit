#ifndef SYNC_SAMPLES_H
#define SYNC_SAMPLES_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define USER_ERR 1
#define LIB_ERR 2
#define SYS_ERR 3
#define INTERNAL_ERR 4
static inline 
ssize_t
full_write(int fd, 
		const char *buf, 
		size_t len)
{
	ssize_t written = 0;
	size_t to_write = len;
	ssize_t ret;
	int got_zero = 0;
	while(to_write)
	{
		ret = write(fd, buf, to_write);
		switch(ret)
		{
			case 0:
				if(got_zero)
					return written;
				got_zero = 1;
				continue;
			case -1:
				if(errno == EINTR)
					continue;
				return written ? written : -1;
			default:
				written += ret;
				to_write -= ret;
				buf += ret;
				break;
		}
	}
	return written;
}
#endif









