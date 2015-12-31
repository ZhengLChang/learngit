#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define KEYPAD_NAME "/dev/atcom_keypad"
typedef enum{
	KEY_RELEASE = 0,
	KEY_SHORT_PRESSED,
	KEY_LONG_PRRESSED,
}atcom_press_status;

typedef struct _atcom_key_t {
    unsigned short  		key_code;  	//current key code
    atcom_press_status      key_state; 	//Key state [KEY_PRESSED, KEY_RELEASED] 
    unsigned long 			time_stamp; //Value of jiffies at time of event 
} atcom_key_t,*patcom_key_t;

unsigned char * keypad_status[]={
	"key is been released",
	"key is been short pressed",
	"key is been long pressed"
};

void printf_key_info(atcom_key_t * keypad)
{
	printf("======get information======\n"\
		"key_code=%d\n"\
		"key_state[%d]=%s\n"\
		"time_stamp=0x%x\n",keypad->key_code,keypad->key_state,keypad_status[keypad->key_state],keypad->time_stamp);
	return;
}

int main(int argc,char **argv)
{
	int fd;
	fd_set  readfds ;
	int 	ret	=0;
	struct timeval tv;
	atcom_key_t keypad;
	fd = open(KEYPAD_NAME,O_RDWR);
	if (fd < 0) {
		printf("Error :open device %s failed \n",KEYPAD_NAME);
		return -1;
	}
	
	FD_ZERO(&readfds);
	FD_SET(fd,&readfds);
	
	// Wait up to five seconds.
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	for (;;) {

		ret = select(fd+1,&readfds, NULL,NULL,NULL);
		if (ret == -1){
			perror("select()");
		}
		else if (ret){
			printf("Data is available now.\n");
			if( FD_ISSET(fd, &readfds)){
				memset(&keypad,0x00,sizeof(keypad));
				ret = read(fd,&keypad,sizeof(keypad));
				if( ret < 0 ){
					perror("read keypad value failed");
				}
				else{
					printf_key_info(&keypad);
				}
			}
		}
		else{
			printf("No data within five seconds.\n");
		}
	}
	close(fd);
	return 0;
}

