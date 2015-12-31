#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <string.h>


#include "test_amp.h"


#define DEV_NAME "/dev/amp_control"


struct atcom_amp{
	int 	fd;
	shutdown_pin_status 	amp_status;
};

const char * ampstatus[]={
	"PULL DOWN",
	"PULL UP",
};

int main(void)
{
	int ret = 0;
	int choose = 0;
	struct atcom_amp info;
	unsigned int status =0;
	memset(&info,0x00,sizeof(struct atcom_amp));
	info.fd = open(DEV_NAME,O_RDWR);
	if( info.fd < 0 )
	{
		printf("open %s failed !\n",DEV_NAME);
		goto error;
	}
	
	while(1){
		printf("\nplease choose acting for amplifire: 0)pull down ext device reset pin 1)pull up ext device reset pin 4) exit test\n");
		scanf("%d",&choose);
		getchar();
		if( PIN_DOWN == choose ){
			printf("\n you choose pull down!\n");
			choose = PIN_DOWN;
		}else if(PIN_UP ==choose){
			printf("\n you choose pull up!\n");
			choose = PIN_UP;

		}else if( 4 == choose){
			printf("\n you choose exit test!\n");
			goto out;

		}

		switch(choose)
		{	
			default:
				ret = ioctl(info.fd,ATCOM_EXT_RESET_GET_STATUS,&info.amp_status);
				if(ret < 0 ){
					printf("get status failed !\n");
				}else{
					status = info.amp_status == 0?0:1;
					printf("\namp current status[%d]=%s\n",info.amp_status,ampstatus[status]);
				}
				break;
			case PIN_DOWN:	
			case PIN_UP:
				ret = ioctl(info.fd,ATCOM_EXT_RESET_GET_STATUS,&info.amp_status);
				if(ret < 0 ){
					printf("ERROR:get status failed [ret=%d]!\n",ret);
				}else{
					status = (info.amp_status == PIN_DOWN?PIN_DOWN:PIN_UP);
					printf("\namp last status[%d]=%s\n",info.amp_status,ampstatus[status]);
				}

				status = choose;
				ret = ioctl(info.fd,ATCOM_EXT_RESET_SET_STATUS,&status);
				if( ret < 0){
					printf("Error:Set status failed [ret=%d]!\n",ret);
				}

				ret = ioctl(info.fd,ATCOM_EXT_RESET_GET_STATUS,&info.amp_status);
				if(ret < 0 ){
					printf("ERROR:get status failed [ret=%d]!\n",ret);
				}else{
					status = (info.amp_status == PIN_DOWN?PIN_DOWN:PIN_UP);
					printf("\namp current status[%d]=%s\n",info.amp_status,ampstatus[status]);
				}

				break;			
		}
		
	}
out:
	close(info.fd);
	return 0;
error:
	return -1;
}

