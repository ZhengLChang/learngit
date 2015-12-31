#include <termios.h> /* POSIX terminal control definitions */
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>


#include "tty_proto.h"


int server_creat_unsock(char *path_name,int *ttyS1)
{
		int ret;
		int ss,sc;
		int serv_len,clnt_len;
		struct sockaddr_un serv_addr,clnt_addr;

		ss = socket(AF_UNIX,SOCK_STREAM,0);
		if(ss < 0)
		{
				printf("Failed to create server unix socket\n");
				return -1;
		}
		serv_addr.sun_family = AF_UNIX;
		strcpy(serv_addr.sun_path, path_name); 
		unlink(path_name);
		//ret = bind(ss,(struct sockaddr *)&serv_addr,sizeof(serv_addr.sun_family) + strlen(serv_addr.sun_path));  // OK
		serv_len = offsetof(struct sockaddr_un,sun_path) + strlen(path_name);
		ret = bind(ss,(struct sockaddr *)&serv_addr,serv_len);
		if(ret < 0)
		{
				printf("Failed to bind unix socket to %s\n",path_name);
				close(ss);
				return -1;
		}
		listen(ss,1);
		memset(&clnt_addr,0,sizeof(clnt_addr));
		clnt_len = sizeof(clnt_addr);
		sc = accept(ss,(struct sockaddr *)&clnt_addr,&clnt_len);
		if(sc < 0)
		{
				printf("Failed to accept client socket. \n");
				close(ss);
				return -1;
		}

		*ttyS1 = sc;

		return 0;
}


int client_creat_unsock(char *path_name,int *ttyS0)
{
		int ret;
		int ss;
		int serv_len;
		struct sockaddr_un serv_addr;

		ss = socket(AF_UNIX,SOCK_STREAM,0);
		if(ss == -1)
		{
				printf("Failed to create client unix socket\n");
				return -1;
		}
		memset((void *)&serv_addr,0,sizeof(serv_addr));
		serv_addr.sun_family = AF_UNIX;
		strcpy(serv_addr.sun_path, path_name); 
		serv_len = offsetof(struct sockaddr_un,sun_path) + strlen(path_name);
		//unlink(path_name);
		ret = connect(ss,(struct sockaddr *)&serv_addr,serv_len);
		if(ret < 0)
		{
				printf("Failed to connect server. \n");
				// close(ss);
				return -2;
		}

		*ttyS0 = ss;
		return 0;
}


/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */
//pthread_mutex_t mtx_rw = PTHREAD_MUTEX_INITIALIZER; 
int open_port(unsigned char *path)
{
		int fd; /* File descriptor for the port */

		fd = open(path, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK); 
		if (fd == -1)  {
				printf("%s %s %d : Unable to open %s\n", __FILE__,__func__,__LINE__,path);
				return -1;
		}  
		else
		{
				fcntl(fd, F_SETFL, 0); /*restore blocking mode */
		}
		//fcntl(fd, F_SETFL, FNDELAY);
		tcflush(fd, TCIOFLUSH);
		usleep(1000);
		tcflush(fd, TCIOFLUSH);
		return (fd);
}

int set_port(int fd)
{
		int ret;
		struct termios options, ori_options;
		/*struct termios options, ori_options; */

		/* Get the current options for the port...  */
		ret = tcgetattr(fd, &ori_options);
		options = ori_options;	
		if (ret < 0) {
				printf("tcgerattr failed\n");
				return -1;
		}

		/* Enable the receiver and set local mode... */
		options.c_cflag |= (CLOCAL | CREAD);

		/* Set the baud rates to 115200... */
#if 1
		cfsetispeed(&options, B115200);
		cfsetospeed(&options, B115200);
#else
		cfsetispeed(&options, B57600);
		cfsetospeed(&options, B57600);
#endif

		/* Setting the Character Size */
		/*	options.c_cflag &= ~CSIZE; * Mask the character size bits */

		/*Setting Parity Checking */
		/*No parity (8N1): */
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		options.c_cflag &= ~CSIZE;
		options.c_cflag |= CS8;

#if 1
		/* Setting Input Parity Options 
		   options.c_iflag |= (INPCK | ISTRIP); */

		/* Setting Hardware Flow Control */
		/* options.c_cflag |= CRTSCTS;    * Also called CRTSCTS */
		options.c_cflag &= ~CRTSCTS;  /* to disable hardware flow control */

		/* Setting Software Flow Control */
		/* options.c_iflag |= (IXON | IXOFF | IXANY); *enable*/
		options.c_iflag &= ~(IXON | IXOFF | IXANY); /*disable */
		options.c_iflag &= ~(INLCR|ICRNL);          // 避免无法接受到oxOD的问题了

		/* options.c_lflag |= (ICANON | ECHO | ECHOE); * Choosing Canonical Input */
		options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* Choosing Raw Input */
		options.c_oflag &= ~OPOST; /* Choosing Raw Output */
#else
	   /* raw input */
	   options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	   /* raw ouput */
	   options.c_oflag &= ~(OPOST | ONLCR | OCRNL | ONOCR | ONLRET);
       options.c_iflag &= ~(IXON | IXOFF);
       options.c_iflag |= (IXON | IXOFF); 
       options.c_iflag &= ~(ISTRIP | IGNCR | INLCR | ICRNL | IUCLC);
       options.c_cflag &= ~CRTSCTS;
#endif
		tcflush(fd, TCIOFLUSH);

		/* setting Control Characters  */
		options.c_cc[VINTR] = 0; 
		options.c_cc[VQUIT] = 0; 
		options.c_cc[VERASE] = 0; 
		options.c_cc[VKILL] = 0; 
		options.c_cc[VEOF] = 0; 
		options.c_cc[VSWTC] = 0; 
		options.c_cc[VSTART] = 0; 
		options.c_cc[VSTOP] = 0; 
		options.c_cc[VSUSP] = 0; 
		options.c_cc[VEOL] = 0; 
		options.c_cc[VREPRINT] = 0; 
		options.c_cc[VDISCARD] = 0; 
		options.c_cc[VWERASE] = 0; 
		options.c_cc[VLNEXT] = 0; 
		options.c_cc[VEOL2] = 0;

		options.c_cc[VTIME] = 0; /* no time out */
		options.c_cc[VMIN] = 0; /*unsigned char : minimum number of characters to read */ 

		tcflush(fd, TCIOFLUSH);

		/* Set the new options for the port... */
		ret = tcsetattr(fd, TCSANOW, &options); /*updata setting for ttys1 */
		if (ret < 0) {
				printf("tcsetattr failed\n");
				return -2;
		}

		return 0;
}


int write_port(int fd, const void *buf, int len)
{
		int times = 3;
		int ret = -1;
		while(times--)
		{
				ret = write(fd, buf, len);
				if ((ret < 0) || (ret < len))
				{
						tcflush(fd, TCOFLUSH);
						usleep(1000);
						continue;
				}
				break;
		}
		if( 0 ==  times)
		{
			printf(" =========ERROR: write failed  \n");
			tcflush(fd, TCOFLUSH);
			usleep(1000);
			return -1;
		}
		else
		{
			return 0;
		}
}


int read_port(int fd,void *buf, int len)
{
		int ret = -1;
		ret = read(fd, buf, len);
		return ret;
}

void close_port(int fd)
{
		/* restore options */
		close(fd);
		return ;
}


