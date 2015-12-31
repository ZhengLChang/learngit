#ifndef __TTY_H__
#define __TTY_H__
#include <termios.h> /* POSIX terminal control definitions */



extern int server_creat_unsock(char *path_name,int *ttyS1);
extern int client_creat_unsock(char *path_name,int *ttyS0);
extern int open_port(unsigned char *path);
extern int set_port(int fd);	
extern int write_port(int fd, const void *buf, int len);
extern int read_port(int fd,void *buf, int len);
extern void close_port(int fd);
#endif
