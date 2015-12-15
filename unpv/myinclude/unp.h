#ifndef UNP_H_
#define UNP_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdarg.h>		
#include <syslog.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <mqueue.h>
#include <semaphore.h>

struct mymesg;
#define	MAXLINE		4096
#define SERVPORT 	9999	
#define BUFFSIZE         56
#define SA struct sockaddr
#define Sigfunc sighandler_t
#define PIPE_BUF 4096
#define MAXMESGDATA (PIPE_BUF - 2 * sizeof(long))
#define MESGHDRSIZE (sizeof(struct mymesg) - MAXMESGDATA)

#ifndef MSG_W
#define MSG_W 0200
#endif
#ifndef MSG_R
#define MSG_R 0400
#endif
#define MQ_KEY1 1234L
#define MQ_KEY2 2345L

typedef void (*Sigfunc)(int);
void	 err_dump(const char *, ...);
void	 err_msg(const char *, ...);
void	 err_quit(const char *, ...);
void	 err_ret(const char *, ...);
void	 err_sys(const char *, ...);
int Sock_cmp_addr(const struct sockaddr *sa1, const struct sockaddr *sa2,
		                         socklen_t salen);
char *Sock_ntop(const struct sockaddr *sa, socklen_t salen);
int Sock_bind_wild(int sockfd, int family);
int Sock_get_port(const struct sockaddr *sa, socklen_t salen);
char *Sock_ntop_host(const struct sockaddr *sa, socklen_t salen);
void Sock_set_addr(struct sockaddr *sa, socklen_t salen, const void *addr);
void Sock_set_port(struct sockaddr *sa, socklen_t salen, int port);
void sock_set_wild(struct sockaddr *sa, socklen_t salen);
ssize_t Readn(int fd, void *ptr, size_t nbytes);
void Writen(int fd, void *ptr, size_t nbytes);
ssize_t Readline(int fd, void *vptr, size_t maxlen);
ssize_t Readlinebuf(void **vptrptr);
void Listen(int fd, int backlog);
void str_echo(int sockfd);
void str_cli(FILE *fp, int sockfd);
Sigfunc Signal(int signo, Sigfunc func);
void str_cli_binary(FILE *fp, int sockfd);
void str_echo_binary(int sockfd);
void str_cli_select(FILE *fp, int sockfd);
int max(int num1, int num2);
void str_cli_select_shutdown(FILE *fp, int sockfd);
ssize_t mesg_send(int, struct mymesg *);
void Mesg_send(int, struct mymesg *);
ssize_t mesg_recv(int, struct mymesg *);
ssize_t Mesg_recv(int, struct mymesg *);
void Sleep_us(unsigned int nusecs);
/*
void client(int readfd, int writefd);
void server(int readfd, int writefd);
*/

#ifndef __USE_GNU
struct msgbuf
{
	long int mtype;
	char mtext[1];
};
#endif

struct mymesg
{
	long mesg_len;
	long mesg_type;
	char mesg_data[MAXMESGDATA];
};
#endif
