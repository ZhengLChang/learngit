#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <utime.h>

#include "app_proto.h"
#include "../link/link_proto.h"

#ifdef GUI
#include "lcd.h"
#include "mmi_data.h"
#include "mmi.h"
#include "mainwindow.h"
#include "phone.h"
#include "led.h"
#include "mmi_common.h"
#include "lcdinfo.h"
#include "lcd_layout.h"
#include "extenkey.h"
#include "key.h"
#include "mmi.h"
#endif

#define  MAX_EXT_NUMBER        10    //ADDED by yuemalin

#define LOGIN_SUCCESS     1

#define MAX_MAIN_LED_INDEX  	18
#define MAX_MAIN_LED_STATUS 	9

typedef struct login_s{

		struct timespec  last_login_time;
		int 			status;     // //1 : has logined  0: don't login  
} logintime,*pLogintime;

logintime clients_login_times[MAX_EXT_NUMBER] ={0};    //added by yuemalin for login

unsigned char proto_verison;
unsigned char fw_version;
unsigned char bcf_version;
#define PROTOCOL_VERSION  proto_version 
#define FIRMWARE_VERSION  fw_version 
#define BCF_VERSION       bcf_version 

unsigned char clients_proto_verison[10] = {0};
unsigned char clients_fw_version[10] = {0};
unsigned char host_proto_version = 0;
unsigned char host_fw_version = 0;
int files_absence = 0;
char init_flag = -1;

int blf_files_num =0;
int bcf_files_num =0;

pthread_mutex_t mutex_files = PTHREAD_MUTEX_INITIALIZER;

core_to_gui_callback_t *to_gui_callback;

int req_image(unsigned char *name,unsigned char flag);
int core_gui_start_gui(void)
{
		//printf("fcw...%s  %s() %d\n",__FILE__,__func__,__LINE__);
		if( NULL != to_gui_callback->to_gui_start_gui )
		{
			to_gui_callback->to_gui_start_gui();
		}
		else
		{
			printf("ERROR: to_gui_start_gui is NULL !\n");
			return -1;
		}
		return 0;
}


static int core_to_gui_status(unsigned char key_id, unsigned char status)
{
		//printf("%s  %s() %d\n",__FILE__,__func__,__LINE__);
		if( NULL != to_gui_callback->to_gui_status)
		{
			to_gui_callback->to_gui_status(key_id,status);
		}
		else
		{
			printf("ERROR: to_gui_status is NULL !\n");
			return -1;
		}

		return 0;
}

static int core_to_gui_login(int dev_id,int fw_version)
{
		//printf("%s  %s() %d\n",__FILE__,__func__,__LINE__);
		if( NULL != to_gui_callback->to_gui_login)
		{
			to_gui_callback->to_gui_login(dev_id,fw_version);
		}
		else
		{
			printf("ERROR: to_gui_login is NULL !\n");
			return -1;
		}
		return 0;
}


static int core_to_gui_image(unsigned char *name)
{
		//printf("%s  %s() %d\n",__FILE__,__func__,__LINE__);
		if( NULL == to_gui_callback->to_gui_image )
		{
			to_gui_callback->to_gui_image(name);
		}
		else
		{
			printf("ERROR: to_gui_login is NULL !\n");
			return -1;
		}
		return 0;
}	

static int core_to_gui_reset(void)
{
		//printf("%s  %s() %d\n",__FILE__,__func__,__LINE__);
		if( NULL != to_gui_callback->to_gui_reset )
		{
			to_gui_callback->to_gui_reset();
		}
		else
		{
			printf("ERROR: to_gui_reset is NULL !\n");
			return -1;
		}
		return 0;
}

static int core_to_gui_upgrade(void )
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		if( NULL != to_gui_callback->to_gui_upgrade)
		{
			to_gui_callback->to_gui_upgrade();
		}
		else
		{
			printf("ERROR: to_gui_upgrade is NULL !\n");
			return -1;
		}
		return 0;
}
static int core_to_gui_upgrade_start(void )
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		if( NULL != to_gui_callback->to_gui_upgrade)
		{
			to_gui_callback->to_gui_upgrade_start();
		}
		else
		{
			printf("ERROR: to_gui_upgrade is NULL !\n");
			return -1;
		}
		return 0;
}

static int core_to_gui_update_bcf(void)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		if( NULL != to_gui_callback->to_gui_update_bcf)
		{
			to_gui_callback->to_gui_update_bcf();
		}
		else
		{
			printf("ERROR: to_gui_update_bcf is NULL !\n");
			return -1;
		}
		return 0;
}

static int core_to_gui_update_blf(void)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		if( NULL != to_gui_callback->to_gui_update_blf)
		{
			to_gui_callback->to_gui_update_blf();
		}
		else
		{
			printf("ERROR: to_gui_update_blf is NULL !\n");
			return -1;
		}
		return 0;
}
static int core_to_gui_key(unsigned char dev_id,unsigned char key_id,unsigned char info)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		if( NULL !=  to_gui_callback->to_gui_key )
		{
			to_gui_callback->to_gui_key(dev_id,key_id,info);
		}
		else
		{
			printf("ERROR: to_gui_key is NULL !\n");
			return -1;
		}
		return 0;
}

static int core_to_gui_login_status(int status)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		if( to_gui_callback->change_login_status )
		{
			to_gui_callback->change_login_status(status);
		}
		else
		{
			printf("ERROR: change_login_status is NULL !\n");
			return -1;
		}
}	

int gui_to_core_key(unsigned char key_id,unsigned char info)
{
		//printf("%s  %s() %d %d %d\n",__FILE__,__func__,__LINE__,key_id,info);
		app_send_key(key_id,info);

		return 0;
}

//added by yuemalin for exchang extern lcd backlight
int core_to_gui_change_extern_lcd_backlight(lcd_backlight_t extern_lcd_bl)
{
	////printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
	if( NULL != to_gui_callback->change_extern_lcd_backlight)
	{
		to_gui_callback->change_extern_lcd_backlight(extern_lcd_bl);
	}
	else
	{
			printf("ERROR: change_extern_lcd_backlight is NULL !\n");
			return -1;
	}
	return 0;
		
}


//added by yuemalin for exchang extern led status
int core_to_gui_change_extern_led_status(led_status_info led_info)
{
		////printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		if (  NULL != to_gui_callback->change_extern_led_status )
		{
			to_gui_callback->change_extern_led_status(led_info);
		}
		else
		{
			printf("ERROR: change_extern_led_status is NULL !\n");
			return -1;
		}	
		return 0;
}

//added by yuemalin for ext get all blf's image ,and notice app 
int notice_app_ext_get_all_blf_image(void)
{
		if(NULL !=  to_gui_callback->notice_get_all_blf_image_done)
		{
				to_gui_callback->notice_get_all_blf_image_done();
		}
		else
		{
			printf("ERROR: notice_get_all_blf_image_done is NULL !\n");
			return -1;
		}
		return 0;
}


//added by yuemalin for ext get all blf's image ,and notice app 
int notice_app_ext_get_all_bcf_image(void)
{
		if(	NULL !=  to_gui_callback->notice_get_all_bcf_image_done)
		{
				to_gui_callback->notice_get_all_bcf_image_done();
		}
		else
		{
			printf("ERROR: notice_get_all_bcf_image_done is NULL !\n");
			return -1;
		}
		return 0;
}


//added by yuemain for ext ask key status
int notice_app_ext_ask_key_status(int id) 
{
		if( NULL != to_gui_callback->to_gui_ask_status)
		{
				to_gui_callback->to_gui_ask_status(id);
		}
		else
		{
			printf("ERROR: to_gui_ask_status is NULL !\n");
			return -1;
		}
		return 0;
}




/* delete files under directory specified by dir if their modification time is later
 * than the modification time of reference specified by ref */
int delete_older_files(unsigned char *ref,unsigned char *dir)
{
		////printf("%s  %s() %d\n",__FILE__,__func__,__LINE__);
		int ret;
		time_t trm; // time of reference file's modication
		struct stat *statbuf;
		DIR *dp;
		struct dirent *de;
		char *curdir;

		statbuf = (struct stat *)malloc(sizeof(struct stat));	
		if(statbuf == NULL)
		{
				printf("%s %d: failed to malloc()\n",__FILE__,__LINE__);
				return -1;
		}
		memset(statbuf,0,sizeof(struct stat));
		ret = stat(ref,statbuf);
		if(ret)
		{
				printf("%s %d: failed to get status attribute of file named %s\n",__FILE__,__LINE__,ref);
				return -2;
		}
		trm = statbuf->st_mtime;
		dp = opendir(dir);
		if(dp == NULL)
		{
				printf("%s %d: Failed to open directory %s\n",__FILE__,__LINE__,dir);
				return -3;
		}
		curdir = (char*)malloc(512);
		memset(curdir,0,512);
		getcwd(curdir,512);
		if(chdir(dir))
		{
				printf("%s %d: failed to change directory %s\n",__FILE__,__LINE__,dir);
				return -3;
		}
		while((de = readdir(dp)) != NULL)
		{
				memset(statbuf,0,sizeof(struct stat));
				ret = stat(de->d_name,statbuf);
				if(ret)
				{
						printf("%s %d: failed to get status attribute of file named %s\n",__FILE__,__LINE__,de->d_name);
						chdir(curdir);
						return -4;
				}
				if(statbuf->st_mtime < trm)
						unlink(de->d_name);
		}
		free(statbuf);
		chdir(curdir);
		free(curdir);

		return 0;
}

/* find and check if the images which are contained in file specified by dst_file exist.
 * If it does not exist, update it from mainboard.
 */
int  images_check_and_update(int fd,unsigned char *image_dir,unsigned char flag)
{
		int ret;
		unsigned char *pch;
		struct dirent *de;
		DIR *dp;
		struct stat stat;
		unsigned char oneline[MAX_ONELINE_LENGTH] = {0};
		unsigned char image_path[MAX_PATH_LENGTH] = {0};
		unsigned char image_name[MAX_NAME_LENGTH] = {0};
		unsigned int ui;

		dp = opendir(image_dir);
		if(dp == NULL)
		{
				printf("Failed to open directory %s\n",image_dir);
				return -1;
		}

		while(1)
		{
				memset(oneline,0,MAX_ONELINE_LENGTH);
				for(ui=0;ui<MAX_ONELINE_LENGTH-1;ui++)
				{
						ret = read(fd,&oneline[ui],1);
						if(ret == 0)
						{
								//added by yuemalin for notice image get done 
								if(!strcmp(BLF_IMAGES_PATH,image_dir))
								{
										if( 0 == blf_files_num )
										{
											//	printf("........................................................................image_dir = %s\n",image_dir);
												core_gui_start_gui();
										} 
										//notice_app_ext_get_all_blf_image();
								}
								return 0;
						}
						if(oneline[ui] == ' ' || oneline[ui] == '\r' || oneline[ui] == '\n' || oneline[ui] == '\t') // if the char is blank space, skip it.
						{
								oneline[ui] = 0;
								ui--;
								continue;
						}
						if(oneline[ui] == '>')
								break;
				}
				pch = strstr(oneline,"Image");
				if(pch)
				{
						pch += 7; 		 // Image="name_num.xxx"......(num : 000~999; xxx : suffix)
						memset(image_name,0,MAX_NAME_LENGTH);
						for(ui = 0; ui < MAX_NAME_LENGTH -1; ui++,pch++)
						{
								if(*pch == '"')
								{
										break;
								}
								image_name[ui] = *pch;
						}
						memset(image_path,0,MAX_PATH_LENGTH);
						sprintf(image_path,"%s/%s",image_dir,image_name);
						if(access(image_path,F_OK) != 0)
						{
								if(init_flag < 0)
								{
										pthread_mutex_lock(&mutex_files);
										files_absence++;
										pthread_mutex_unlock(&mutex_files);
								}
								if(!strcmp(BLF_IMAGES_PATH,image_dir))
								{
										pthread_mutex_lock(&mutex_files);
										blf_files_num++;
										pthread_mutex_unlock(&mutex_files);
								}
								if(!strcmp(BCF_IMAGES_PATH,image_dir))
								{
										pthread_mutex_lock(&mutex_files);
										bcf_files_num++;
										pthread_mutex_unlock(&mutex_files);
								}


								/* file not exists */
								req_image(image_name,flag);
								pch = &image_name[MAX_NAME_LENGTH-1];
								while(pch >= image_name)
								{
										pch--;
										if(*pch == '_')
										{
												break;
										}
								}
								if(pch == image_name)
								{
										/*e.g. image_name = "rose.jpg", don't need to remove old file*/
										continue;
								}
								else
								{
										/*e.g. image_name = "peter_123", need to remove old file */
										memset(pch,0,(&image_name[MAX_NAME_LENGTH-1] - pch + 1));
								}
								while((de = readdir(dp)) != NULL)
								{
										if(strncmp(de->d_name,image_name,strlen(image_name)))
												unlink(de->d_name);
										break;
								}
						}
						else // if image exists, update its changed time to now. Later, we will delete images that are older than blf.xml.
						{
								utime(image_path,NULL);
						}
				}
		}
		return 0;
}


//added by yuemalin for login  //每个外设两分钟之内只允许登录1次

int depth_deal_login(int id )
{
		int ret = 0;
		time_t  loginde_times=0;
		struct timespec now;

		clock_gettime(CLOCK_MONOTONIC, &now);

		if( id < MAX_EXT_NUMBER && id > 0 )
		{
				if( 0 == clients_login_times[id-1].status)  //第一次登录，记录时间即可
				{
						printf(" ==============first_time login=================\n");
						memcpy(&(clients_login_times[id-1].last_login_time),&now,sizeof(struct timespec));
						clients_login_times[id-1].status =  1;
				}
				else
				{
						loginde_times = now.tv_sec - (clients_login_times[id-1].last_login_time).tv_sec ;

						if( loginde_times > 2 * 60 )   //超过2分钟允许重新登录
						{
								printf(" ==============2 minis  login=================\n");
								memcpy(&(clients_login_times[id-1].last_login_time),&now,sizeof(struct timespec));
								clients_login_times[id-1].status =  1;
						}
						else   //2分钟之内部同一个ID不允许重复的登录
						{
								printf(" ============== login else is not promis=================\n");
								return -1 ;
						}

				}
		}
		else
		{
				return -2;

		}

		return 0;
}


int handle_login(app_proto_login_t lg)
{
		//printf("%s  %s() %d\n",__FILE__,__func__,__LINE__);
		static int handle_login_flag =0;
		if( 0 == handle_login_flag )
		{
			handle_login_flag = 1;
			unsigned char buf[3];
			int ret  = 0;    //added by yuemalin 
			ret = depth_deal_login(lg.dev_id);
			if( ret < 0 )
			{
					printf(" it has logined ");
					handle_login_flag =  0;
					return 0;
			}
			core_to_gui_login(lg.dev_id,lg.fw_version);
			clients_proto_verison[lg.dev_id] = lg.proto_version;
			clients_fw_version[lg.dev_id] = lg.fw_version;
			buf[0] = 0x80;
			buf[0] |= e2m_login;
			buf[1] = BCF_VERSION;
			buf[2] = 0xFF;
			ret = link_send(lg.dev_id,buf,3);
			if( ret < 0 )
			{
				printf("send message e2m_login failed [ret = %d]!\n",ret);
				handle_login_flag =  0;
				return ret;
			}
			handle_login_flag =  0;
		}
		return 0;
}
# if 0
int handle_ack_login(app_proto_ack_login_t alg)
{
		int ret = -1,fd = -1;
		unsigned char cmd;
		DIR *dp;

		if((alg.bck_version > BCF_VERSION) || access(APP_PROTO_BCF_PATH,F_OK))
		{
				cmd = e2m_req_bcf;
				link_send(0x01,&cmd,1);
		}

		return 0;
}
#else
int handle_ack_login(app_proto_ack_login_t alg)
{
		int ret = -1,fd = -1;
		unsigned char cmd;
		DIR *dp;

		//printf("%s  %s() %d\n",__FILE__,__func__,__LINE__);

		core_to_gui_login_status(LOGIN_SUCCESS);    //修改注册成功标志

		if((alg.bck_version > BCF_VERSION) || access(APP_PROTO_BCF_PATH,F_OK))
		{
				cmd = e2m_req_bcf;
				ret = link_send(0x01,&cmd,1);
				if( ret < 0 )
				{
					printf("send message e2m_req_bcf failed [ret = %d]!\n",ret);
					return ret;
				}
		}

		return 0;
}
#endif


int handle_req_bcf(unsigned char dev_id)
{
		//printf("%s  %s() %d\n",__FILE__,__func__,__LINE__);
		int fd;
		int len;
		int ret = 0;
		unsigned char *buf;

		buf = (unsigned char *)malloc(4096);
		if(buf == NULL)
		{
				printf("failed to allocate new memory\n");
				return -1;
		}
		fd = open(APP_PROTO_BCF_PATH, O_RDONLY);
		if(fd < 0)
		{
				printf("failed to open %s \n ",APP_PROTO_BCF_PATH);
				free(buf);
				return -2;
		}

		buf[0] = 0x80;
		buf[0] |= e2m_req_bcf;
		buf[1] = 0x0F;
		ret  = read(fd,&buf[2],4094);
		len = ret + 2;
		if(ret < 0)
		{
				printf("failed to read data from %s\n",APP_PROTO_BCF_PATH);
				ret = -3;
				goto OUT;
		}
		ret = link_send(dev_id,buf, len);
		if(ret < 0)
		{
				ret = -4;
				goto OUT;
		}

		buf[1] = 0xF0;
		while(1)
		{
				ret = read(fd,&buf[2],4094);
				if(ret < 0)
				{
						printf("failed to read data from %s\n",APP_PROTO_BCF_PATH);
						ret = -3;
						goto OUT;
				}
				len = ret + 2;
				if(ret == 0)
				{
						buf[1] = 0xF1; // to inform the end of file
						if(link_send(dev_id,buf, len) < 0)
						{
								ret = -4;
								goto OUT;
						}
						break;
				}
				if(link_send(dev_id,buf, len) < 0)
				{
						ret = -4;
						goto OUT;
				}
		}
OUT:
		free(buf);
		close(fd);

		return ret;
}

int handle_ack_req_bcf(app_proto_fixed_file_t *bcf)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		int fd;
		int ret;
		unsigned char swname[512] = {0};

		sprintf(swname,"%s",APP_PROTO_BCF_PATH);
		if(bcf->flag_cs == 0x0F)
		{
				unlink(swname);
				fd = open(swname, O_CREAT |O_RDWR );
				fchmod(fd,0666);
		}
		else if(bcf->flag_cs == 0xF0)
		{
				fd = open(swname, O_WRONLY );
		}
		else  // 0xF1 end of file
		{
				core_to_gui_update_bcf();
				unlink(APP_PROTO_BCF_PATH);
				ret = rename(swname,APP_PROTO_BCF_PATH);
				if(ret)
				{
						printf("%s %s %d : failed to rename file %s\n",__FILE__,__func__,__LINE__,swname);
						return -2;
				}
				fd = open(APP_PROTO_BCF_PATH,O_RDONLY);
				if(fd < 0)
				{
						printf("failed to open %s as the mean of read only\n ",APP_PROTO_BCF_PATH);
						return -3;
				}
				ret = images_check_and_update(fd,BCF_IMAGES_PATH,0x02);
				if(ret != 0)
				{
						close(fd);
						return -4;
				}
				close(fd);
				// delete old images that is older than bcf.xml
				ret = delete_older_files(APP_PROTO_BCF_PATH,BCF_IMAGES_PATH);
				if(ret < 0)
				{
						printf("failed to delete older images\n");
				}

				return 0;
		}
		if(fd < 0)
		{
				printf("failed to open %s \n ",APP_PROTO_BCF_PATH);
				return -1;
		}
		if(bcf->flag_cs == 0xF0)
		{
				if(lseek(fd,0,SEEK_END) < 0)
				{
						close(fd);
						return -2;
				};
		}
		ret = write(fd,bcf->content,bcf->len_content);
		if(ret != bcf->len_content)
		{
				close(fd);
				return -2;
		}
		close(fd);



		return 0;

}

int handle_req_blf(unsigned char dev_id)
{
		int fd;
		int len;
		int ret = 0;
		unsigned char *buf;

		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		buf = (unsigned char *)malloc(4096);
		if(buf == NULL)
		{
				printf("failed to allocate new memory\n");
				return -1;
		}
		fd = open(APP_PROTO_BLF_PATH, O_RDONLY);
		if(fd < 0)
		{
				printf("failed to open %s \n ",APP_PROTO_BLF_PATH);
				free(buf);
				return -2;
		}

		buf[0] = 0x80;
		buf[0] |= e2m_req_blf;
		buf[1] = 0x0F;
		ret  = read(fd,&buf[2],4094);
		len = ret + 2;
		if(ret < 0)
		{
				printf("failed to read data from %s\n",APP_PROTO_BLF_PATH);
				ret = -3;
				goto OUT;
		}
		ret = link_send(dev_id,buf, len);
		printf("send blf = %s\n",buf);
		if(ret < 0)
		{
				ret = -4;
				printf("0 ..send blf = %s\n",buf);

				goto OUT;
		}

		buf[1] = 0xF0;
		while(1)
		{
				ret = read(fd,&buf[2],4094);
				if(ret < 0)
				{
						printf("failed to read data from %s\n",APP_PROTO_BLF_PATH);
						ret = -3;
						goto OUT;
				}
				len = ret + 2;
				if(ret == 0)
				{
						buf[1] = 0xF1; // to inform the end of file
						if(link_send(dev_id,buf, len) < 0)
						{
								printf("2 ..send blf = %s\n",buf);
								ret = -4;
								goto OUT;
						}
						break;
				}
				if(link_send(dev_id,buf, len) < 0)
				{
						printf("1 ..send blf = %s\n",buf);
						ret = -4;
						goto OUT;
				}
		}
OUT:
		free(buf);
		close(fd);

		return ret;
}

int handle_ack_req_blf(app_proto_fixed_file_t *blf)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		int fd = -1;
		int ret; 
		unsigned char swname[512] = {0};

		sprintf(swname,"%s.sw",APP_PROTO_BLF_PATH);
		if(blf->flag_cs == 0x0F)/* first segment */
		{
				unlink(swname);
				fd = open(swname, O_CREAT |O_RDWR);
				fchmod(fd,0666);
		}
		else if(blf->flag_cs == 0xF0) // continue sending
		{
				fd = open(swname, O_WRONLY);
		}
		else                                         // end of sending
		{
				core_to_gui_update_blf();
				unlink(APP_PROTO_BLF_PATH);
				ret = rename(swname,APP_PROTO_BLF_PATH);
				if(ret)
				{
						printf("%s %s %d : failed to rename file %s\n",__FILE__,__func__,__LINE__,swname);
						return -2;
				}
				fd = open(APP_PROTO_BLF_PATH,O_RDONLY);
				if(fd<0)
				{
						printf("failed to open file named %s \n",APP_PROTO_BLF_PATH);
						return -1;
				}
				ret = images_check_and_update(fd,BLF_IMAGES_PATH,0x01);
				if(ret < 0)
				{
						printf("failed to chech images belong to blf.xml \n");
						close(fd);
						return -2;
				}
				close(fd);
				/* delete old images that is older than blf.xml */
				ret = delete_older_files(APP_PROTO_BLF_PATH,BLF_IMAGES_PATH);
				if(ret < 0)
				{
						printf("failed to delete older images\n");
						return -3;
				}
				return 0;
		}
		if(fd < 0)
				return -1;
		if(blf->flag_cs == 0xF0)
		{
				if(lseek(fd,0,SEEK_END) < 0)
				{
						close(fd);
						return -2;
				};
		}
		printf("fcw .....%s,%d",blf->content,blf->len_content);
		ret = write(fd,blf->content,blf->len_content);
		if(ret != blf->len_content)
				return -2;
		close(fd);

		return 0;
}

int handle_req_image(app_proto_req_image_t *rImage)
{
		printf("%s:%d, handle_req_image.\n",__FILE__,__LINE__);
		int fd;
		int ret;
		int len;
		unsigned char *buf = NULL, *pch = NULL;
		unsigned char flag_cl = 0; // image under blf_iamges, = 1; under bcf_images, = 2.

		flag_cl = rImage->flag;
		buf = (unsigned char *)malloc(4096);
		if(buf == NULL)
		{
				printf("failed to allocate new memory\n");
				return -1;
		}
		memset(buf,0,4096);
		if(flag_cl == 0x01)
				sprintf(buf,"%s/%s",BLF_IMAGES_PATH,rImage->image_name);
		else if(flag_cl == 0x02)
				sprintf(buf,"%s/%s",BCF_IMAGES_PATH,rImage->image_name);
		else
		{
				printf("invalid image flag =  \n",flag_cl);
				return -2;
		}
		fd = open(buf, O_RDONLY);
		if(fd < 0)
		{
				printf("%s %s() %d failed to open image %s\n",__FILE__,__func__,__LINE__,buf);
				return -3;
		}
		memset(buf,0,4096);
		pch = buf;
		*pch = 0x80;
		*pch |= e2m_req_image;
		pch++;
		*pch = (flag_cl << 4) & 0xF0;
		pch++;
		len = strlen(rImage->image_name);
		*pch = len;
		pch++;
		memcpy(pch,rImage->image_name,len);
		len = strlen(buf); // length of image's name and header of package.
		pch = buf + len;  // point to the content of image file.
		ret = read(fd,pch,(4096 - len));
		if(ret < 0)
		{
				printf("failed to read data from %s\n",rImage->image_name);
				ret = -3;
				goto OUT;
		}
		if(link_send(rImage->dev_id,buf, (len + ret)) < 0)
		{
				close(fd);
				free(buf);
				ret = -4;
				goto OUT;
		}
		buf[1] |= 0x01;
		while(1)
		{
				ret = read(fd,pch,(4096 - len));
				if(ret < 0)
				{
						printf("failed to read data from %s\n",rImage->image_name);
						ret = -3;
						goto OUT;
				}
				if(ret == 0)
				{
						buf[1] |= 0x02; // to inform the end of file
						if(link_send(rImage->dev_id,buf, len) < 0)
						{
								ret = -4;
								goto OUT;
						}
						break;
				}
				if(link_send(rImage->dev_id,buf, (len + ret)) < 0)
				{
						ret = -4;
						goto OUT;
				}
				memset(pch,0,(4096 - len));
		}

OUT:
		free(buf);
		close(fd);
		return ret;
}

int handle_ack_req_image(app_proto_ack_image_t *aImage)
{
		//printf("%s %s() %d\n",__FILE__,__func__,__LINE__);
		int fd;
		int ret;
		unsigned char swname[MAX_PATH_LENGTH + 5] = {0};
		unsigned char path[MAX_PATH_LENGTH] = {0};

		if(aImage->flag_cl == 0x01)           //image belongs to blf.xml
		{
				sprintf(path,"%s/%s",BLF_IMAGES_PATH,aImage->image_name);
		}
		else if(aImage->flag_cl == 0x02)      //image belongs to bcf.xml
		{
				sprintf(path,"%s/%s",BCF_IMAGES_PATH,aImage->image_name);
		}
		else
		{
				printf("%s %s %d invalid flag = %d .......\n",__FILE__,__func__,__LINE__,aImage->flag_cl);
				return -1;
		}
		sprintf(swname,"%s.sw",path);
		if(aImage->flag_cs == 0x00)
		{
				unlink(swname);
				fd = open(swname, O_CREAT |O_RDWR );
		}
		else if(aImage->flag_cs == 0x01)
		{
				fd = open(swname, O_WRONLY );
		}
		else if(aImage->flag_cs == 0x03)
		{
				unlink(path);
				ret = rename(swname,path);
				if(ret)
				{
						printf("%s %s %d : failed to rename file %s\n",__FILE__,__func__,__LINE__,swname);
						return -2;
				}
				core_to_gui_image(aImage->image_name);

				if(init_flag < 0)
				{
						pthread_mutex_lock(&mutex_files);
						files_absence--;
						if(files_absence == 0)
						{
								init_flag = 1;
								//core_gui_start_gui();
						}
						pthread_mutex_unlock(&mutex_files);
				}

				if(aImage->flag_cl == 0x01)           //image belongs to blf.xml
				{
						pthread_mutex_lock(&mutex_files);
						blf_files_num--;
						if( 0 == (blf_files_num+bcf_files_num))
						{
								//core_gui_start_gui();
						}
						if( 0 == blf_files_num )	
						{		
								core_gui_start_gui();
								//notice_app_ext_get_all_blf_image();
						}
						pthread_mutex_unlock(&mutex_files);
				}
				else if(aImage->flag_cl == 0x02)      //image belongs to blf.xml
				{
						pthread_mutex_lock(&mutex_files);
						bcf_files_num--;	
						if(0 == bcf_files_num)
						{
								notice_app_ext_get_all_bcf_image();
						}
						pthread_mutex_unlock(&mutex_files);

				}
				return 0;
		}
		else
		{
				printf("%s %d invalid translation flag\n",__FILE__,__LINE__);
				return -1;
		}
		if(fd < 0)
		{
				printf("%s %d failed to open iamge %s\n",__FILE__,__LINE__,path);
				return -1;
		}
		fchmod(fd,0666);
		if(aImage->flag_cs == 0x01)
		{
				if(lseek(fd,0,SEEK_END) < 0)
				{
						close(fd);
						return -2;
				};
		}
		ret = write(fd,aImage->content,aImage->len_content);
		if(ret != aImage->len_content)
		{
				close(fd);
				return -3;
		}

		close(fd);

		return 0;
}

int handle_send_key(app_proto_e_send_key_t pk)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		core_to_gui_key(pk.dev_id,pk.key_id,pk.info);

		return 0;
}

int handle_send_status(app_proto_send_status_t status)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		core_to_gui_status(status.key_id,status.status);

		return 0;
}


int handle_send_blf(app_proto_fixed_file_t *blf)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		handle_ack_req_blf(blf);

		return 0;

}


int handle_send_image(app_proto_ack_image_t *aImage)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		handle_ack_req_image(aImage);
}


int handle_reset(void)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		core_to_gui_reset();

		return 0;
}


int handle_upgrade_fw(app_proto_fixed_file_t *fw)
{
		static int fd;
		static time_t start_time;
		static time_t end_time;
		int ret; 
		unsigned char swname[512] = {0};
//		sprintf(swname,"%s","/usr/local/data/tmp/exp.tar.gz");
		sprintf(swname,"%s","../data/tmp/exp.tar.gz");
		if(fw->flag_cs == TRANS_START)
		{
				unlink(swname);
				time(&start_time);
				fd = open(swname, O_CREAT |O_RDWR);
				if(fd < 0)
						return -1;
				fchmod(fd,0666);
				core_to_gui_upgrade_start();
		}
		else if(fw->flag_cs == TRANS_CONTINUE)
		{
				fd = open(swname, O_WRONLY);
				if(fd < 0)
						return -1;
				if(lseek(fd,0,SEEK_END) < 0)
				{
						close(fd);
						return -2;
				};
		}
		else
		{
				unlink(APP_PROTO_FW_PATH);
				core_to_gui_upgrade();
				time(&end_time);
				printf("total time = %d\n",end_time-start_time);
				return 0;
		}
		ret = write(fd,fw->content,fw->len_content);
		if(ret != fw->len_content)
				return -2;
		close(fd);

		return 0;
}

//added by yuemalin for exchang extern lcd backlight
int handle_lcd_bl(lcd_backlight_t extern_lcd_bl)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		//printf("lcd_type=%d,lcd_backlight=%d\n",extern_lcd_bl.lcd_type,extern_lcd_bl.lcd_bl_value);
		core_to_gui_change_extern_lcd_backlight(extern_lcd_bl);
		return 0;
}

//added by yuemalin for exchang extern led_status
int handle_led_status(led_status_info led_status)
{			
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		//printf("led_num=%d\n",     	led_status.led_num);     
		//printf("led_color=%d\n",    led_status.led_color);   
		//printf("led_twinkle=%d\n",  led_status.led_twinkle); 
		//printf("led_interval=%d\n", led_status.led_interval);
		core_to_gui_change_extern_led_status(led_status);
		return 0;
}

//added by yuemalin for handle
int handle_expend_cmd(pcmd_tlv pcmd)
{
	if (  NULL != to_gui_callback->to_gui_dealwith_cmd )
	{
		to_gui_callback->to_gui_dealwith_cmd(pcmd);
	}
	else
	{
		printf("ERROR: deal with cmd function  is NULL !\n");
		return -1;
	}	
	return 0;

}



int app_proto_process(pro_data_info_t *pdata)
{
		int ret;
		unsigned char cmd;
		unsigned char *pch;

		cmd = *(pdata->data);
		pch = pdata->data + 1;
		if((cmd &0x80) != 0x80) //�������������������
		{
				switch(cmd)
				{
						case e2m_login :
								{
										app_proto_login_t lg;
										memset((void*)&lg,0,sizeof(lg));
										lg.dev_id = pdata->dev_id;
										lg.proto_version = *(pch);
										lg.fw_version = *(pch+1);
										lg.info_reserved = *(pch+2);
										handle_login(lg);
										break;
								}
						case e2m_req_bcf :
								{
										handle_req_bcf(pdata->dev_id);
										break;
								}
						case e2m_req_blf :
								{
										handle_req_blf(pdata->dev_id);
										break;
								}
						case e2m_req_image :
								{
										app_proto_req_image_t rImage;
										memset((void*)&rImage,0,sizeof(rImage));
										rImage.flag = *pch;
										pch++;
										memcpy(rImage.image_name,pch,pdata->length-2);
										rImage.dev_id = pdata->dev_id;
										handle_req_image(&rImage);
										break;
								}
						case e2m_send_key :
								{
										app_proto_e_send_key_t ekey;
										ekey.dev_id = pdata->dev_id;
										ekey.key_id = *pch;
										pch++;
										ekey.info = *pch;
										handle_send_key(ekey);
										break;
								}
						case m2e_send_stat :
								{
										app_proto_send_status_t status;
										status.key_id = *pch;
										status.status= *(pch+1);
										handle_send_status(status);
										break;
								}
						case m2e_send_blk :
								{
										app_proto_fixed_file_t *blf;
										blf = (app_proto_fixed_file_t *)malloc(sizeof(app_proto_fixed_file_t));
										memset((void *)blf,0,sizeof(app_proto_fixed_file_t));
										blf->flag_cs = *pch;
										pch++;  // points to content
										blf->len_content = pdata->length - 2;   //except for cmd and flag_cs
										blf->content = (unsigned char *)malloc(blf->len_content);
										memset(blf->content,0,blf->len_content);
										memcpy(blf->content,pch,blf->len_content);
										handle_send_blf(blf);
										free(blf->content);
										free(blf);
										break;
								}
						case m2e_reset_ex :
								{
										handle_reset();
										break;
								}
						case m2e_up_fw :
								{
										app_proto_fixed_file_t *fw;
										fw = (app_proto_fixed_file_t *)malloc(sizeof(app_proto_fixed_file_t));
										memset((void *)fw,0,sizeof(app_proto_fixed_file_t));
										fw->flag_cs = *pch;
										pch++;  // points to content
										fw->len_content = pdata->length - 2;   //except for cmd and flag_cs
										fw->content = (unsigned char *)malloc(fw->len_content);
										memset(fw->content,0,fw->len_content);
										memcpy(fw->content,pch,fw->len_content);
										handle_upgrade_fw(fw);
										free(fw->content);
										free(fw);
										break;
								}
						case m2e_send_image :
								{
										app_proto_ack_image_t *aImage;
										aImage = (app_proto_ack_image_t *)malloc(sizeof(app_proto_ack_image_t));
										if(aImage == NULL)
												break;
										memset((void *)aImage,0,sizeof(app_proto_ack_image_t));
										aImage->flag_cs = *pch;
										pch++;

										aImage->len_name = *pch;
										aImage->len_content = pdata->length - aImage->len_name - 3;
										pch++;
										memcpy(aImage->image_name,pch,aImage->len_name);
										pch += aImage->len_name;
										aImage->content = (unsigned char *)malloc(aImage->len_content);
										if(aImage->content == NULL)
												break;
										memset(aImage->content,0,aImage->len_content);
										memcpy(aImage->content,pch,aImage->len_content);
										handle_send_image(aImage);
										free(aImage->content);
										free(aImage);
										break;
								}
						case e2m_ask_status:  //added by yuemalin for ext ask for key status information from main board
								{
										notice_app_ext_ask_key_status(pdata->dev_id);
										break;
								}
						case e2m_cmd:
								{
									pcmd_tlv pcmd = (pcmd_tlv)pch;
									unsigned char cmd_len = pcmd->cmd_len;
									handle_expend_cmd(pcmd);
								}

						default :
								{
										break;
								}
				}
		}

		if((cmd & 0x80) == 0x80)   //��Ҵ�����������������
		{
				/* ack */
				cmd &= 0x7F;
				printf("%s:%d, ack cmd: %d\n",__FILE__, __LINE__, cmd);
				switch(cmd)
				{
						case e2m_login :
								{
										app_proto_ack_login_t alg;
										memset((void*)&alg,0,sizeof(alg));
										alg.bck_version = *(pch);
										alg.reserved = *(pch+1);
										handle_ack_login(alg);
										break;
								}
						case e2m_req_bcf :
								{	
										app_proto_fixed_file_t *bcf;
										bcf = (app_proto_fixed_file_t *)malloc(sizeof(app_proto_fixed_file_t));
										memset((void *)bcf,0,sizeof(app_proto_fixed_file_t));
										bcf->flag_cs = *pch;
										pch++;  // points to content
										bcf->len_content = pdata->length - 2;   //except for cmd and flag_cs
										bcf->content = (unsigned char *)malloc(bcf->len_content);
										memset(bcf->content,0,bcf->len_content);
										memcpy(bcf->content,pch,bcf->len_content);
										handle_ack_req_bcf(bcf);
										free(bcf->content);
										free(bcf);
										break;
								}
						case e2m_req_blf :
								{
										app_proto_fixed_file_t *blf;
										blf = (app_proto_fixed_file_t *)malloc(sizeof(app_proto_fixed_file_t));
										memset((void *)blf,0,sizeof(app_proto_fixed_file_t));
										blf->flag_cs = *pch;
										pch++;  // points to content
										blf->len_content = pdata->length - 2;   //except for cmd and flag_cs
										blf->content = (unsigned char *)malloc(blf->len_content);
										memset(blf->content,0,blf->len_content);
										memcpy(blf->content,pch,blf->len_content);
										handle_ack_req_blf(blf);
										free(blf->content);
										free(blf);
										break;
								}
						case e2m_req_image :
								{
										app_proto_ack_image_t *aImage;
										aImage = (app_proto_ack_image_t *)malloc(sizeof(app_proto_ack_image_t));
										if(aImage == NULL)
												break;
										memset((void *)aImage,0,sizeof(app_proto_ack_image_t));
										aImage->flag_cl = (*pch >> 4) & 0x0F;
										aImage->flag_cs = *pch & 0x0F;
										pch++;

										aImage->len_name = *pch;
										aImage->len_content = pdata->length - aImage->len_name - 3;
										pch++;
										memcpy(aImage->image_name,pch,aImage->len_name);
										pch += aImage->len_name;
										aImage->content = (unsigned char *)malloc(aImage->len_content);
										if(aImage->content == NULL)
												break;
										memset(aImage->content,0,aImage->len_content);
										memcpy(aImage->content,pch,aImage->len_content);
										handle_ack_req_image(aImage);
										free(aImage->content);
										free(aImage);
										break;
								}
								//added by yuemalin for extern lcd backlight 
						case m2e_send_lcd_backlight:
								{
										lcd_backlight_t extern_lcd_bl;
										extern_lcd_bl.lcd_type     = *(pch);
										extern_lcd_bl.lcd_bl_value = *(pch+1);
										handle_lcd_bl(extern_lcd_bl);
										break;
								}
						case m2e_send_led_info:
								{
										led_status_info led_status;
										led_status.led_num 		= *(pch) ;       
										led_status.led_color	= *(pch+1);     
										led_status.led_twinkle  = *(pch+2);   
										led_status.led_interval = *(pch+3);
										handle_led_status(led_status);
										break;
								}
						case e2m_send_key :
								{
										//break;
								}
						case m2e_send_stat :
								{
										//break;
								}
						case m2e_send_blk :
								{
										//break;
								}
						case m2e_reset_ex :
								{
										//break;
								}
						case m2e_up_fw :
								{
										app_proto_fixed_file_t *fw;
										fw = (app_proto_fixed_file_t *)malloc(sizeof(app_proto_fixed_file_t));
										memset((void *)fw,0,sizeof(app_proto_fixed_file_t));
										fw->flag_cs = *pch;
										pch++;  // points to content
										fw->len_content = pdata->length - 2;   //except for cmd and flag_cs
										fw->content = (unsigned char *)malloc(fw->len_content);
										memset(fw->content,0,fw->len_content);
										memcpy(fw->content,pch,fw->len_content);
										handle_upgrade_fw(fw);
										free(fw->content);
										free(fw);
										break;
								}
						case m2e_send_image :
							{
										//break;
							}
						case m2e_cmd:   //deal with main machine expand cmd
							 {
								pcmd_tlv pcmd =(pcmd_tlv)pch;
								handle_expend_cmd(pcmd);
							 }
						default :
							{
										break;
							}

				}
		}

		free(pdata->data);
		free(pdata);

		return 0;
}


key_t q_key;
int msg_id;

int app_proto_init(core_to_gui_callback_t *callback)
{
		int ret;
		//printf("%s  %s() %d\n",__FILE__,__func__,__LINE__);
		ret = access(APP_PROTO_WORK_PATH, F_OK);  
		if(ret != 0)
		{
				ret = mkdir(APP_PROTO_WORK_PATH,0777); 
				if(ret == -1)
						return -1;
		}

		ret = access(TOP_IMAGES_PATH, F_OK);  
		if(ret != 0)
		{
				ret = mkdir(TOP_IMAGES_PATH,0777); 
				if(ret == -1)
						return -1;
		}
		ret = access(BLF_IMAGES_PATH, F_OK);  
		if(ret != 0)
		{
				ret = mkdir(BLF_IMAGES_PATH,0777); 
				if(ret == -1)
						return -1;
		}
		ret = access(BCF_IMAGES_PATH, F_OK);  
		if(ret != 0)
		{
				ret = mkdir(BCF_IMAGES_PATH,0777); 
				if(ret == -1)
						return -1;
		}

#if 0
		if((q_key = ftok(".",'p')) == -1)
		{
				perror("Failed to ftok");
				return -1;
		}
		if((msg_id = msgget(q_key,IPC_CREAT | 0666)) == -1)
		{
				perror("Failed to msgget");
		}
#endif
		to_gui_callback = callback;
		//added by yuemalin 
		memset(	&clients_login_times,0x00,sizeof(clients_login_times));
}





int wait_and_handle_events(void)
{
		//printf("%s  %s() %d\n",__FILE__,__func__,__LINE__);
		int ret;
		unsigned short rset = 0xFF;
		pro_data_info_t *pdata = NULL;
		unsigned int len;
		char dev_id;
		int qid;

		while(1)
		{
				// every 0.05 second, check one time if there have received events and key events.
				rset = 0xFF;
				ret = link_listen (NULL,  &rset, NULL);
				if(ret <= 0)
						continue;
				if(ttyS0 < 0)
				{
						for(dev_id=1; dev_id <= MAX_EXTBOARD; dev_id++)
						{
								if(rset & (1<< dev_id))
								{
										while(1)
										{
												ret = link_recv(dev_id,&pdata);
												if(ret < 0)
														break;
												app_proto_process(pdata);
										}
								}
						}
				}
				else
				{
						while(1)
						{
								ret = link_recv(0x01,&pdata);
								if(ret == -1)
										break;
								app_proto_process(pdata);
						}
				}
		}
		return 0;
}

int app_send_key(unsigned char key_id, unsigned char info)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		unsigned char buf[3] = {0};
		int ret = 0;

		buf[0] = e2m_send_key;
		buf[1] = key_id;
		buf[2] = info;
		ret = link_send(0x01,buf,3);
		if( ret < 0 )
		{
			printf("send message e2m_send_key failed [ret = %d]!\n",ret);
			return ret;
		}

		return 0;
}

int app_send_status(unsigned char dev_id,unsigned char key_id,unsigned char status)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		unsigned char buf[3] = {0};
		int ret = 0;
		buf[0] = m2e_send_stat;
		buf[1] = key_id;
		buf[2] = status;
		ret = link_send(dev_id,buf,3);
		if( ret < 0 )
		{
			printf("send message m2e_send_stat failed [ret = %d]!\n",ret);
			return ret;
		}
		return 0;
}

int app_send_image(unsigned char dev_id, unsigned char *image_name)
{
		int ret = -1;
		app_proto_req_image_t sImage;
		memset((void*)&sImage,0,sizeof(sImage));
		sImage.dev_id = dev_id;
		memcpy(sImage.image_name,image_name,strlen(image_name));
		ret = handle_req_image(&sImage);
		if(ret < 0)
		{
				printf("%s %d failed to send image named %s \n",__FILE__,__LINE__,image_name);
				return -1;
		}
		return 0;
}


int app_send_blf(unsigned char dev_id)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		int ret = 0;
		ret = handle_req_blf(dev_id);
		if( ret < 0 )
		{
			printf(" call handle_req_blf failed !\n");
			return ret;
		}
		return 0;
}

int app_send_reset(unsigned char dev_id)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		unsigned char buf[1] = {0};
		int ret = 0;
		buf[0] = m2e_reset_ex;
		ret = link_send(dev_id,buf,1);
		if( ret < 0 )
		{
			printf("send message m2e_reset_ex failed [ret = %d]!\n",ret);
			return ret;
		}
		return 0;
}

int app_send_upgrade(unsigned char dev_id)
{
		printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		int fd;
		int len;
		int ret = 0;
		unsigned char *buf;

		buf = (unsigned char *)malloc(4096);
		if(buf == NULL)
		{
				printf("failed to allocate new memory\n");
				return -1;
		}
		fd = open(APP_PROTO_FW_PATH, O_RDONLY);
		if(fd < 0)
		{
				printf("failed to open %s \n ",APP_PROTO_FW_PATH);
				free(buf);
				return -2;
		}

		buf[0] = (m2e_up_fw|0x80);
		buf[1] = TRANS_START;
		ret  = read(fd,&buf[2],4094);
		len = ret + 2;
		if(ret < 0)
		{
				printf("failed to read data from %s\n",APP_PROTO_FW_PATH);
				ret = -3;
				goto OUT;
		}
		ret = link_send(dev_id,buf, len);
		if(ret < 0)
		{
				ret = -4;
				goto OUT;
		}

		buf[1] = TRANS_CONTINUE;
		while(1)
		{
				ret = read(fd,&buf[2],4094);
				if(ret < 0)
				{
						printf("failed to read data from %s\n",APP_PROTO_FW_PATH);
						ret = -3;
						goto OUT;
				}
				len = ret + 2;
				if(ret == 0)
				{
						buf[1] = TRANS_END; // to inform the end of file
						if(link_send(dev_id,buf, len) < 0)
						{
								ret = -4;
								goto OUT;
						}
						break;
				}
				if(link_send(dev_id,buf, len) < 0)
				{
						ret = -4;
						goto OUT;
				}
		}

OUT:
		free(buf);
		close(fd);

		return ret;
}



int login_to_mainboard(unsigned char proto_version, unsigned char fw_version)
{
		int ret;
		unsigned char buf[4] = {0};
		buf[0] = e2m_login;
		buf[1] = proto_version;
		buf[2] = fw_version;
		buf[3] = 0xFF;
		ret = link_send(0x01,buf,4);
		if(ret < 0)
		{
			printf("send message login_to_mainboard failed [ret = %d]!\n",ret);
			return ret;
		}
		return 0;
}




int req_blf(void)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		int ret;
		unsigned char buf[1] = {0};

		buf[0] = e2m_req_blf;
		ret = link_send(0x01,buf,1);
		if(ret < 0)
		{
			printf("send message e2m_req_blf failed [ret = %d]!\n",ret);
			return ret;
		}

		return 0;
}



//added by yuemalin for extern ask status from main board
int req_status_info(void)
{
		int ret;
		unsigned char buf[1] = {0};
		buf[0] = e2m_ask_status;
		ret = link_send(0x01,buf,1);
		if( ret < 0)
		{
			printf("send message e2m_ask_status failed [ret = %d]!\n",ret);
			return ret;
		}
		return 0 ;
}


int req_image(unsigned char *name,unsigned char flag)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		int ret;
		unsigned char len;
		unsigned char buf[128] = {0};

		buf[0] = e2m_req_image;
		buf[1] = flag;
		len = strlen(name);
		memcpy(&buf[2],name, len);
		ret = link_send(0x01,buf,len+2);
		if(ret < 0)
		{
			printf("send message e2m_req_image failed [ret = %d]!\n",ret);
			return ret;
		}

		return 0;
}

//added  by yuemalin for main send extern lcd and led status info  2014.05.19 
int app_send_lcd_bl(unsigned char dev_id,unsigned char lcd_type,unsigned char lcd_bl)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		int ret = 0;
		unsigned char send_buff[10]={0};

		if( lcd_type >= 2)
		{
				printf("extren lcd type shulled be [0/1],0:main lcd , 1: sub lcd\n");
				return -1;
		}
		if( lcd_bl > 9)
		{
				printf("extren lcd backlight  shulled be [0...9]\n");
				return -1;
		}

		//printf("LED TYPE = %d , lcd_bl = %d [%s:%d]\n",lcd_type,lcd_bl,__func__,__LINE__);

		send_buff[0] = 0x80;
		send_buff[0] |=m2e_send_lcd_backlight;
		send_buff[1]=lcd_type;
		send_buff[2]=lcd_bl;

		ret = link_send(dev_id,send_buff,3);
		if(ret < 0)
		{
			printf("send message m2e_send_lcd_backlight failed [ret = %d]!\n",ret);
			return ret;
		}
		return 0;
}

//app change extren machine led status   yuemalin 2014.05.19
int app_send_led_status(unsigned char dev_id,unsigned char led_num,
				unsigned char led_color,unsigned char led_twinkle,unsigned char led_interval)
{
		//printf("%s	%s() %d\n",__FILE__,__func__,__LINE__);
		int ret = 0;
		unsigned char send_buff[10]={0};
		if( led_num > 17 || led_color > 3 || led_twinkle >1 || led_interval > 1 )
		{
				printf(" led param is misstake ,please refrence led_status_info \n");
				return -1;
		}
		send_buff[0]= 0x80;
		send_buff[0]|=m2e_send_led_info;
		send_buff[1]=led_num;
		send_buff[2]=led_color;
		send_buff[3]=led_twinkle;
		send_buff[4]=led_interval;
		ret = link_send(dev_id,send_buff,5);
		if(ret < 0)
		{
			printf("send message m2e_send_led_info failed [ret = %d]!\n",ret);
			return ret;
		}
		return 0;

}

//����˷��͵���չ����ӻ��
int app_main_send_cmd(unsigned char dev_id,unsigned char cmd,unsigned char * pdata,int pdata_len )
{
	int data_len = 0;
	pcmd_tlv pcmd = NULL;
	unsigned char *pbuf = NULL;
	int ret = 0;

	if( NULL != pdata )
	{
		data_len = sizeof(pcmd_tlv)+pdata_len+2;
	}
	else
	{	
		data_len = sizeof(pcmd_tlv)+2;
	}
	
	pbuf = (unsigned char *)malloc( data_len);
	if( NULL == pbuf )
	{
		printf("malloc cmd buffer failed[%s;%d] ! \n",__func__,__LINE__);
		return -1;
	}

	pcmd = (pcmd_tlv)(pbuf+1);
	memset(pbuf,0x00,data_len);

	pbuf[0] = (0x80|m2e_cmd);
	pcmd->cmd =cmd;
	if( NULL != pdata )
	{
		pcmd->cmd_len=pdata_len;
		memcpy(pcmd->pvalue,pdata,pdata_len);
	}
	else
	{
		pcmd->cmd_len=0;
	}
	ret = link_send(dev_id,pbuf,data_len);
	if(ret < 0)
	{
		printf("send message m2e_cmd failed [ret = %d]!\n",ret);
		return ret;
	}
	return 0;
}

//�ӻ�˷�����չ��������
int app_extern_send_cmd(unsigned char dev_id,unsigned char cmd,unsigned char * pdata,int pdata_len )
{
	int data_len = 0;
	pcmd_tlv pcmd = NULL;
	unsigned char *pbuf = NULL;
	int ret = 0;

	if( NULL != pdata )
	{
		data_len = sizeof(pcmd_tlv)+pdata_len+2;
	}
	else
	{	
		data_len = sizeof(pcmd_tlv)+2;
	}
	
	pbuf = (unsigned char *)malloc( data_len);
	if( NULL == pbuf )
	{
		printf("malloc cmd buffer failed[%s;%d] ! \n",__func__,__LINE__);
		return -1;
	}

	pcmd = (pcmd_tlv)(pbuf+1);
	memset(pbuf,0x00,data_len);

	pbuf[0] = e2m_cmd;
	pcmd->cmd =cmd;
	if( NULL != pdata )
	{
		pcmd->cmd_len=pdata_len;
		memcpy(pcmd->pvalue,pdata,pdata_len);
	}
	else
	{
		pcmd->cmd_len=0;
	}
	ret = link_send(dev_id,pbuf,data_len);
	if(ret < 0)
	{
		printf("send message e2m_cmd failed [ret = %d]!\n",ret);
		return ret;
	}
	return 0;
}

#if 0
//added a app for set main board led status 
int  app_change_main_board_led_status(unsigned char led_index, unsigned char led_color)
{
		int ret = 0;

		if( led_index > MAX_MAIN_LED_INDEX || led_color > MAX_MAIN_LED_STATUS || led_index == 0 ) 
		{
				printf("app set led param is misstake led_index=%d,led_color=%d [%s:%d]\n",led_index,led_color,__func__,__LINE__);
				return -1;
		}
		if( 0 == led_color)
		{
				ret = hw_led_off(led_index);
		}
		else
		{
				ret = hw_led_on(led_index,led_color);
		}

		return ret ;

}
#else
//added a app for set main board led status 
int  app_change_main_board_led_status(unsigned char led_index, unsigned char led_color)
{
		int ret = 0;

		if( led_index > MAX_MAIN_LED_INDEX || led_color > MAX_MAIN_LED_STATUS || led_index == 0 ) 
		{
//				printf("app set led param is misstake led_index=%d,led_color=%d [%s:%d]\n",led_index,led_color,__func__,__LINE__);
				return -1;
		}
		if( 0 == led_color)
		{
				ret = hw_led_off(led_index);
		}
		else
		{
				ret = hw_led_on(led_index,led_color);
		}

		return ret ;

}
#endif

