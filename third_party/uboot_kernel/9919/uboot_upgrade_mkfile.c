#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct appliction_updata_info{
	char mtd_src[128]; 			//	mtd part info which is ubifs and restore updating source file
	char mtd_src_path[128];		//	updating source file path which is relation to mtd src
	char mtd_src_name[128];		//	soucre file name 
	char mtd_dst[128]; 			//	mtd part info which is ubifs and will be updataed
}atcom_app_updata_info_t,*patcom_app_updata_info_t;

#define UPDATA_FILE_NAME "/usr/local/data/uboot_updata_configure"
#define UPDATA_VERSION 	 "ATCOM updata configre v 0.0.1"

void printf_help(void)
{
	printf("run cmd like:\n updata_test [mtd_src mtd_src_path mtd_src_name mtd_dst][..]...\n");
	printf("\tmtd_src:\tname of mtd part which store updata source file,name can run \"cat /proc/mtd\" to get\n");
	printf("\tmtd_src_path:\tsoure file path which is stored in mtd_src,defaut value is '/' \n");
	printf("\tmtd_src_name:\tname of  updata source file\n");
	printf("\tmtd_dst:\tname of mtd part which will be updata,it's name can run  \"cat /proc/mtd\" to get else\n");
}


void printf_config_info(atcom_app_updata_info_t *psrc_in, int count)
{
	atcom_app_updata_info_t * psrc = psrc_in;
	int 	i  = 0;
	for( i = 0; i < count ;i++){
		printf("index[%d]:\n",i);
		printf("\t mtd_src=%s\n"	,	psrc->mtd_src	);		
		printf("\t mtd_src_path=%s\n",	psrc->mtd_src_path);
		printf("\t mtd_src_name=%s\n",	psrc->mtd_src_name);
		printf("\t mtd_dst=%s\n"	,	psrc->mtd_dst	);		
		printf("\n");
		psrc++;
	}
	return ;
}


int main(int argc,char ** argv)
{
	atcom_app_updata_info_t *pupdata_config = NULL;
	atcom_app_updata_info_t *psrc 			= NULL;
	int fd_conf 		= 0;
	int ret  			= 0;
	int count 			= 0;	
	int  i 	 			= 0;
	int j				= 0;
	
	if( 1 == argc){
		printf_help();
		return 0;
	}
	
	if( 2 == argc && (!strcasecmp("--help",argv[1])||!strcasecmp("-h",argv[1]))){
		printf_help();
		return 0;
	}

	if( 2 == argc && (!strcasecmp("--version",argv[1])||!strcasecmp("-V",argv[1]))){
		printf("\n%s\n",UPDATA_VERSION);
		return 0;
	}


	if( 0 != (argc-1)%4 ){
		printf("argc=%d\n",argc);
		printf_help();
		return -1;
	}
	
	count = (argc-1)/4;
	printf("\nThere are %d mtd part  need updata :\n",count);
	pupdata_config = (atcom_app_updata_info_t *)malloc(count*sizeof(*pupdata_config));
	if( NULL == pupdata_config ){
		printf("Error: malloc failed !\n");
		return -1;
	}
	memset(pupdata_config,0x00,count*sizeof(*pupdata_config));
	
	psrc = pupdata_config;
	for( i = 0,j=1 ; i< count ;i++){
		memcpy(psrc->mtd_src,argv[j],strlen(argv[j]));
		j++;

		if(!strcmp(argv[j],"/")){
			memcpy(psrc->mtd_src_path,"NULL",strlen("NULL"));
			j++;
		}else{
			memcpy(psrc->mtd_src_path,argv[j],strlen(argv[j]));
			j++;
		}
		memcpy(psrc->mtd_src_name,argv[j],strlen(argv[j]));
		j++;
		memcpy(psrc->mtd_dst,argv[j],strlen(argv[j]));
		j++;
		psrc++;
	}

	fd_conf = open(UPDATA_FILE_NAME,O_RDWR|O_CREAT|O_TRUNC);
	if( fd_conf < 0 ){
		printf("open file %s failed !\n",UPDATA_FILE_NAME);
		goto error;
	}

	psrc = pupdata_config;
		
	ret = write(fd_conf,psrc,count*sizeof(*pupdata_config));
	if( ret != count*sizeof(*pupdata_config)){
		printf("Error: write data length[%d] is short than need length[%d]  !\n",ret,count*sizeof(*pupdata_config));
		goto error;
	}

	//Print updata configure information
	printf_config_info(psrc,count);
	


	if( NULL != pupdata_config){
		free(pupdata_config);
		pupdata_config = NULL;
	}
	close(fd_conf);
	return  0;

error:
	if( NULL != pupdata_config){
		free(pupdata_config);
		pupdata_config = NULL;
	}
	close(fd_conf);
	return -1;
}




