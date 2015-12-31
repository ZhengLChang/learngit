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

#include <sys/prctl.h>

#include "link_proto.h"
#include "../tty/tty_proto.h"

#define MAX_MAIN_BOARD_LED_INDEX    18
#define MAX_MAIN_BOARD_LED_COLOR    9

unsigned char tx_alive = 0;
unsigned char in_listen = 0;
pro_packet_info_t pkt_info_table[2] = {0};
unsigned char proRxBuffer[PRO_MAX_BUFFER_SIZE] = {0};
unsigned char proTxBuffer[PRO_MAX_BUFFER_SIZE] = {0};
int ttyS0 = -1;   //port which xmit data through
int ttyS1 = -1; // another tty  port except for tx_tty


list_data_info_t list_tx[MAX_EXTBOARD];	// tx linked list of tx data info
list_data_info_t list_rx; // rx linked list of rx data info
unsigned short  mark_e = 0, mark_w = 0, mark_r = 0;
unsigned char *ptmp;
unsigned char dev_cnt_r[MAX_EXTBOARD] = {0};
unsigned short cnt_r = 0;

/* reserve one data item pointor for each possible extended board */
pro_data_info_t *DataItemList[MAX_EXTBOARD] = {NULL};
unsigned char PktNum[MAX_EXTBOARD] = {0};

static int debug_level 	= 0;   //��ӡ����
static int debug_waring = 4;   //��ӡ����
static int debug_err 	= 6;   //��ӡ����

#define SEND_OK 	 	0
#define SEND_FAILED  	-1
#define SEND_CANCLED    -2


#define TRANS_OK				0
#define TRANS_AGAIN 			1
#define	TRANS_OUT_TIME			2
#define TRANS_CANCLE_BY_OTHER	3
#define TRANS_ANSWER_ERROR		4
#define TRANS_FAILED			5

	
#define     YES		1
#define 	NO  	0


struct send_data_info{
	unsigned char dev_id:3;
	unsigned char cmd:3;
	unsigned char num:2;
};

struct send_data_info last_send_data_info;


char * packet_info[]=
{	
	"pro_cmd_ack", 
	"pro_cmd_nak", 
	"pro_cmd_can", 
	"pro_cmd_sof", 
	"pro_cmd_tx",  
	"pro_cmd_eof", 
};


#if 0   //modified by yuemalin for main board led 2014.05.19
int hw_led_on(unsigned char led_index, int ,int );

int hw_led_off(unsigned char led_index);
#else
int hw_led_on(unsigned char led_index, unsigned char led_color );
int hw_led_off(unsigned char led_index);
#endif

void send_answer_nack(unsigned char dev_id);

unsigned char calcrc_bytes(unsigned char *p,unsigned char len);
unsigned char pro_gen_crc8(unsigned char *data,unsigned char length);

void *thread_rx(void *arg);

void *thread_tx(void *arg);

unsigned char pro_pack_data(pro_packet_info_t *info,unsigned char *pbuf);
int pro_unpack_pkg(pro_packet_info_t *info,unsigned char * pbuffer,int data_len);

int tx_data_info(pro_data_info_t *pdata_info);
int rx_data_info( pro_packet_info_t *ppkt);


pro_data_info_t*  fetch_from_list_header(list_data_info_t *plist);

int link_init(void);
int link_exit(void);

int link_send( int dst,  unsigned char *buf, unsigned int len );

int link_recv( int dev_id,  pro_data_info_t **ppdata );

int link_listen ( unsigned short *w_devset,  unsigned short *r_devset,  unsigned short *e_devset);

int clear_tx_list(unsigned char idx);

/******To generate CRC check code **********/ 
unsigned char calcrc_1byte(unsigned char abyte) 
{     
		unsigned char i,crc_1byte; 
		crc_1byte=0;               
		for(i = 0; i < 8; i++)      
		{        
				if(((crc_1byte^abyte)&0x01))           
				{            
						crc_1byte^=0x18;              
						crc_1byte>>=1;             
						crc_1byte|=0x80;            
				}               
				else             
						crc_1byte>>=1;        
				abyte>>=1;             
		}      

		return crc_1byte; 
} 

unsigned char calcrc_bytes(unsigned char *p,unsigned char len)
{ 
		unsigned char crc=0; 
		while(len--) 
		{ 
				crc=calcrc_1byte(crc^*p++);  
		} 
		return crc;  
}

unsigned char pro_gen_crc8(unsigned char *data,unsigned char length)
{
		unsigned char crc = 0;
		crc = calcrc_bytes(data,length);

		return crc;
}


/*
 * pack data into a packet, and return the length of packet 
 * included the length of header of packet.
 */
unsigned char pro_pack_data(pro_packet_info_t *info,unsigned char *pbuf)
{	
		unsigned char *pdata;
		unsigned char len = 0, crc8 = 0;
		unsigned char ch = 0;
		unsigned char pTxBuf[PRO_MAX_BUFFER_SIZE] = {0};
		unsigned char 	total_len 	= 0;

		memset(pTxBuf,0x00,sizeof(pTxBuf));
		len = info->length;

		memset(pTxBuf,0,PRO_MAX_BUFFER_SIZE);

		pTxBuf[PRO_IDX_PKT_START] = TTY_DATA_HEADER;
		ch = ((info->dev_id) << 5 ) & 0xE0;
		pTxBuf[PRO_IDX_PKT_ID_CMD_NUM] = ch;
		ch = (info->cmd << 2) & 0x1C;
		pTxBuf[PRO_IDX_PKT_ID_CMD_NUM] |= ch;
		ch = info->num & 0x03;
		pTxBuf[PRO_IDX_PKT_ID_CMD_NUM] |= ch;

		pTxBuf[PRO_IDX_PKT_DATALEN] = len;

		memcpy(&pTxBuf[PRO_IDX_PKT_DATA_START],info->data,len);

		crc8 = pro_gen_crc8(info->data,info->length);

		if(crc8 == 0xFF)
		{
				pTxBuf[PR0_IDX_PKT_CRC_START ] = 0xFF;
				pTxBuf[PR0_IDX_PKT_CRC_START + 1 ] = 0xFF;
		}
		else if(crc8 == 0x00)
		{
				pTxBuf[PR0_IDX_PKT_CRC_START ] = 0xFF;
				pTxBuf[PR0_IDX_PKT_CRC_START + 1] = 0xF0;
		}
		else if (0xAA == crc8 )  //added by yuemalin for crc failed
		{
				pTxBuf[PR0_IDX_PKT_CRC_START ] = 0xFF;
				pTxBuf[PR0_IDX_PKT_CRC_START+1] = 0xFA;
		}
		else
		{
				pTxBuf[PR0_IDX_PKT_CRC_START] = crc8;
				pTxBuf[PR0_IDX_PKT_CRC_START+1] = crc8; 
		}

		//total_len = len + PRO_IDX_PKT_HEADER_LEN+3;
		total_len = len + PRO_IDX_PKT_HEADER_LEN;
		memset(pbuf,0x00,sizeof(pbuf));
		memcpy(pbuf,pTxBuf,total_len);
		return  total_len;
}


int pro_unpack_pkg(pro_packet_info_t *info,unsigned char * pbuffer,int data_len)
{
		unsigned char idx = 0, len = 0;
		unsigned char *pdat = NULL, *pcrc = NULL;
		unsigned char ch = 0;
		ch = pbuffer[PRO_IDX_PKT_ID_CMD_NUM];
		info->dev_id  = (( ch >> 5) & 0x07);
		info->cmd  = (( ch >> 2) & 0x07);
		info->num  = (ch & 0x03);

		pdat = &pbuffer[PRO_IDX_PKT_DATA_START];
		pcrc = &pbuffer[PR0_IDX_PKT_CRC_START];
//		printf(" get total len = %d [%s:%d]\n",data_len,__func__,__LINE__);

		//��ѹ���	
		for( idx =  0,len =  0; len < data_len; len++)
		{	
			if( 0xFF ==  pdat[len] )
			{
					len++;			
					switch(pdat[len])
					{
						case 0xFA:
							info->data[idx++] = 0xAA ;
							break;
						case 0XFF:
							info->data[idx++] = 0xFF;
							break;	
						case 0XF0:
							info->data[idx++] = 0x00;
							break;	
						default:
							info->data[idx++] = pdat[len];
							break;	
					}
			}
			else
			{
				info->data[idx++] = pdat[len] ;
			}			
		}	
	
		info->length =  idx ;
		
		if( 0xFF == *pcrc)
		{
			pcrc++;
			//printf(" 0 get crc = 0x%x\n",*pcrc);

			switch(*pcrc)
			{
				case 0xFA:
					info->crc8[0] = 0xAA;
					break;	
				case 0xFF:
					info->crc8[0] = 0xFF;
					break;
				case 0xF0:
					info->crc8[0] = 0x00;
					break;
				default:
					info->crc8[0] = *pcrc;
					break;
			}
		}
		else
		{
			//printf(" 1 get crc = 0x%x\n",*pcrc);
			info->crc8[0] = *pcrc;
		}
		
		return 0;

}
	
/*linux condition variables used to synchronize thread 
 * e.g. thread connect has sent attach packet, than it wait for alloted id.
 * when allot id packet comming, we use pthread_cond_signal() to to 
 * activated the thread who waited for this condition variable;
 */
pthread_cond_t cnd_wait_to_tx_data = PTHREAD_COND_INITIALIZER;
pthread_cond_t cnd_wait_to_rx_data = PTHREAD_COND_INITIALIZER;
pthread_cond_t cnd_send_new_data = PTHREAD_COND_INITIALIZER;

/* linux pthread mutex variable to protect the soure shared with other thread 
 * e.g. when one thread A uses proTxBuffer and it has not finished its operation,
 * thread shedule happens. After schedule, one thread B will be executed and it maybe use 
 * the proTxBuffer. 
 * Then it will happen that there is data from both of thead A and B.  */
pthread_mutex_t mutex_tx_buf = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_rx_buf = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_listen = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_rx_list = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_tx_list = PTHREAD_MUTEX_INITIALIZER;


/* linux pthread mutex variable be used by one thread who needs waiting for condition variable.
 * To avoid that it waited timeout,and it is being prepared for waiting again but the signal is comming.
 */
pthread_mutex_t tx_sec_wind = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t send_new_data_wind = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tx_data_mutex = PTHREAD_MUTEX_INITIALIZER;   //added by yuemalin

void *thread_tx(void *arg)
{
		int ret = 0;
		pro_data_info_t *pdata_info;
		unsigned char idx = 0, num = 0;
		while(1)
		{
				for(idx = 0;idx < MAX_EXTBOARD; idx++ )
				{
						while(1)
						{
								pthread_mutex_lock(&mutex_tx_list);
								pdata_info = fetch_from_list_header(&list_tx[idx]);
								pthread_mutex_unlock(&mutex_tx_list);
								if(pdata_info == NULL)
								{
									break;	
								}
					
								pthread_mutex_lock(&tx_data_mutex);
								pthread_mutex_lock(&tx_sec_wind);
								ret = tx_data_info(pdata_info);
								pthread_mutex_unlock(&tx_sec_wind);
								pthread_mutex_unlock(&tx_data_mutex);
								if(ret == -1)
								{
										/*if one connected device is disconnected,we should cancel all of data sended to it*/
										pthread_mutex_lock(&mutex_tx_list);
										clear_tx_list(idx);
										pthread_mutex_unlock(&mutex_tx_list);
										/* link layer error */
										pthread_mutex_lock(&mutex_listen);
										mark_e |= 1 << pdata_info->dev_id;
										mark_w |= 1 << pdata_info->dev_id;
										in_listen = 0;
										//pthread_cond_signal(&cnd_listen);
										pthread_mutex_unlock(&mutex_listen);
								}
								if(ret == -2)
								{
										/* be canceled by receiver */
										pthread_mutex_lock(&mutex_listen);
										if(ttyS0 <= 0)
												mark_w |= 1 << pdata_info->dev_id;
										//pthread_cond_signal(&cnd_listen);
										in_listen = 0;
										pthread_mutex_unlock(&mutex_listen);
										sleep(1);
								}
								
								/* destroy this data item if the data be sent out successfully or not */
								free(pdata_info->data);
								free(pdata_info);

						}
				}

				pthread_mutex_lock(&mutex_tx_list);
				for(num = 0;num < MAX_EXTBOARD; num++ )
				{
						if(list_tx[num].header != NULL)
						{
								break;
						}
				}

				if(num == MAX_EXTBOARD)
				{
						tx_alive = 0;
						pthread_mutex_unlock(&mutex_tx_list);
						while(1)  //�ȴ�������ǿ�
						{
								pthread_mutex_lock(&mutex_tx_list);
								if(!tx_alive)
								{
										pthread_mutex_unlock(&mutex_tx_list);
										usleep(50000); //50ms
								}
								else
								{
										pthread_mutex_unlock(&mutex_tx_list);
										break;
								}
						}
				}
				else
				{
						pthread_mutex_unlock(&mutex_tx_list);
				}
		}


		return (void *)0;
}
void get_pthread_info(void)
{
	#if 0
        int i,j;
        int policy;
        int priority;
        struct sched_param param;

        pthread_getschedparam(pthread_self(),&policy,&param);
        if(policy==SCHED_OTHER)
        {
         		//printf("thread 1 SCHED_OTHER\n");
         		priority = param.sched_priority;
               // printf("thread 1 SCHED_OTHER=%d \n",priority);
        }
        if(policy==SCHED_RR)
        {
	        // printf("thread 1 SCHED_RR\n");
	         priority = param.sched_priority;
		     //printf("thread 1 SCHED_RR=%d \n",priority);
        }
        if(policy==SCHED_FIFO)
        {
                //printf("thread 1 SCHED_FIFO\n");
                priority = param.sched_priority;
               // printf("thread 1 SCHED_FIFO=%d \n",priority);
        }
#endif
#if 0
	pthread_attr_t attr;
	struct sched_param param;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	param.sched_priority = 50;
	pthread_attr_setschedparam(&attr, &param);
	pthread_create(&threadid, &attr, &threadfunc, NULL);
#endif

}

#if 0

//added  by yuemalin for check by 
int inline check_buffer( unsigned char * pchar,int * len)
{
	int  i = 0;
	int  data_len = 0;

	//�ҵ�buffer�е�һ����Ϊ'0'����
	for(i < 0 ; i < *len; i++)
	{
		if( 0x00 !=  pchar[i] )
		{
			break;
		}	
	}	
	
	if( i != *len)  //buffer��ʼ���ֺ��е�'0'
	{
		if( 0 != i )
		{
			*len = *len-i;
		 	memcpy(pchar,pchar+i,*len);
		}
	}
	else  //ȫ'0'
	{
		*len  = 0;
		return -1;
	}
		
	data_len =  * len;

	if( data_len < MAIN_LED_CONTRL_LEN )
	{
		return -1;
	}
	
	//����������Ƿ��������LED��������
	if( MAIN_LED_CONTRL_CMD == pchar[0])
	{
		//printf("i get main len contrl cmd !\n");
		*len = *len-MAIN_LED_CONTRL_LEN;
		memcpy(pchar,pchar+MAIN_LED_CONTRL_LEN,*len);
	}
	
	if( 0 == *len )
	{
		return -1;
	}

	/*To check if received package is valid */
	if(pchar[0] != TTY_DATA_HEADER )
	{
			PRINTF(debug_err,"======INTFO_rx:i receied a unknow packet\n");
			printf("ERROR DATA IS len=%d:",*len); 
			for ( i = 0; i < *len;i++)
			{
				printf(" [%d]=0x%x\n",i,pchar[i]);
			}
			printf("\n");
			send_answer_nack(1); //��������һ��nack����ӿ���Ӧ�ٶ�
			*len	 = 0;
			return -1;
	}
	return  0;
}
#else
int inline check_buffer( unsigned char * pchar,int * len)
{
	int  i = 0;
	int  data_len = 0;
	unsigned char head=PRO_PKT_START_CHAR;
	char * phead = NULL;
	data_len =  *len ;
	//�ҵ�buffer�е�һ��0XAA���ֵĵط�
	for( i = 0; i<data_len ; i++)
	{
		if(PRO_PKT_START_CHAR == pchar[i])
		{
			break;
		}
	}

	if( i == data_len )
	{
		*len = 0;
		return -1;
	}
	
	*len = *len-i;
	memcpy(pchar,pchar+i,*len);
	return  0;
}
#endif

struct data_info_header{
	unsigned char header; 
	unsigned char dev_id:3;
	unsigned char cmd:3;
	unsigned char num:2;
	unsigned char length;
	unsigned char crc8[2]; //add by yuenalin
};


int  inline get_data_head(unsigned char *pbuffer,int len,struct data_info_header * phead)
{		
		int 	num 			= 0;
		unsigned char data		= 0;
		unsigned char dev_id 	= 0;
		unsigned char cmd_id 	= 0;
		unsigned char dev_id_back 	= 0;
		unsigned char get_data_len 	= 0;
		unsigned char crc8 			= 0;
		pro_packet_info_t *ppkt		= NULL;
		pro_packet_info_t data_info;


//		printf("=======%s:%d==========\n",__func__,__LINE__);	

		memset(&data_info,0x00,sizeof(data_info));

		//��ȡ��ǰ��ݵı䳤��ݵĳ���	
		get_data_len  = pbuffer[PRO_IDX_PKT_DATALEN];	
		data = pbuffer[PRO_IDX_PKT_ID_CMD_NUM];
		phead->dev_id  = (( data >> 5) & 0x07);
		phead->cmd  = (( data >> 2) & 0x07);
		phead->num  = (data & 0x03);
		phead->length =  get_data_len;

		cmd_id  = phead->cmd ;
		
		//���豸ID���Ƿ���ȷ
		if( phead->dev_id != 1 )
		{
			return -1;
		}		
		
		//�鿴��ı���Ƿ���ȷ
		if( phead->num  >= 4 )
		{
			return -1;
		}
		//�鿴������Ƿ���ȷ	
		switch(cmd_id)
		{
			case pro_cmd_ack: 			
			case pro_cmd_nak: 
			case pro_cmd_can: 
			case pro_cmd_sof: 
			case pro_cmd_tx: 
			case pro_cmd_eof:
				 break;
			default:     
				 return -1;
				 break;
		}
		PRINTF(debug_level,"dev_id =%d , cmd =%d ,num = %d , length = %d crc=%x04x\n",phead->dev_id,phead->cmd,phead->num ,phead->length,phead->crc8);
		return 0;
}

int deal_with_indata(unsigned char *pbuffer,int get_len)
{
		unsigned char data		= 0;
		unsigned char dev_id 	= 0;
		unsigned char cmd_id 	= 0;
		unsigned char dev_id_back 	= 0;
		unsigned char get_data_len 	= 0;
		unsigned char crc8 			= 0;
		pro_packet_info_t data_info;
		pro_packet_info_t *ppkt= NULL;
		int ret = 0;
		int i  = 0;

		//printf("=======%s:%d==========\n",__func__,__LINE__);	

		//�������
		get_data_len = pbuffer[PRO_IDX_PKT_DATALEN];

		//����CRC
		crc8 = pro_gen_crc8(&pbuffer[PRO_IDX_PKT_DATA_START],get_data_len);
		
		//��ѹ��ݰ�
		pro_unpack_pkg(&data_info,pbuffer,get_data_len);
		if( crc8 !=  data_info.crc8[0])
		{	
			
			PRINTF(debug_waring,"crc   get_data_len = %d check failed [0x%x:0x%x:0x%x]!\n",get_data_len,crc8,data_info.crc8[0],data_info.crc8[1]);
#if 0
			printf("get data is :");
			for( i = 0 ; i < get_len; i++ )
			{
				printf(" [%d]=0x%x",i,pbuffer[i]);
			}
			printf("\n");
#endif
			dev_id_back = 1;
			send_answer_nack(dev_id_back);
			return -1;
		}
		
		//����ݰ��е��������ݰ�
		cmd_id = pbuffer[PRO_IDX_PKT_ID_CMD_NUM];
		cmd_id = (cmd_id >> 2)&0x07;
		if(cmd_id < pro_cmd_sof) //��Ӧ������
		{
				ppkt = &pkt_info_table[0];
				memset((unsigned char*)ppkt,0,sizeof(pro_packet_info_t));
				memcpy((unsigned char*)ppkt,(unsigned char *)&data_info,sizeof(pro_packet_info_t));
		}
		else
		{
				ppkt = &pkt_info_table[1];
				memset((unsigned char*)ppkt,0,sizeof(pro_packet_info_t));
				memcpy((unsigned char*)ppkt,(unsigned char *)&data_info,sizeof(pro_packet_info_t));
		}

		switch(ppkt->cmd)
		{
				case pro_cmd_ack:
				case pro_cmd_nak:
				case pro_cmd_can:
						ret = 0;
						pthread_mutex_lock(&tx_sec_wind);
						pthread_cond_signal(&cnd_wait_to_tx_data);
						pthread_mutex_unlock(&tx_sec_wind);
						break;
				case pro_cmd_sof:
				case pro_cmd_tx:
				case pro_cmd_eof:
						ret = rx_data_info(ppkt);
						break;
				default:
						break;
		}
		
	return ret;
}





void * thread_rx(void *argc)
{
		fd_set rdfds;
		int fd_max;
		int ret,len;
		pro_packet_info_t *ppkt = NULL;
		unsigned char crc8 = 0;
		unsigned char dev_id = 0;
		unsigned char cmd_id = 0;
		int port_act;
		unsigned char get_data_len = 0;
		int i = 0;
		unsigned char read_buffer[CMD_BUFFER_LEN]={0};
		struct data_info_header head_info ;

		memset(read_buffer,0x00,sizeof(read_buffer));
		len = 0;
		while(1)    
		{
				FD_SET(ttyS1, &rdfds);
				ret = select (ttyS1+1, &rdfds, NULL, NULL, NULL);
				if(ret <= 0)
				{
						continue;
				}
				
				if(!FD_ISSET(ttyS1,&rdfds))
				{
					continue;
				}

				//pthread_mutex_lock(&mutex_rx_buf);
				ret = read_port(ttyS1,(read_buffer + len),PRO_MAX_BUFFER_SIZE-len);
				if( 0 == ret)
				{
					continue;
				}
				
				len += ret;

				ret = check_buffer(read_buffer,&len);
				if( ret < 0 )
				{
					continue;
				}
				
				if( len < sizeof(struct data_info_header))	//��Ϣ��ͷ��Ϣ��û�н�����
				{
					continue;
				}		
				
				memset(&head_info,0x00,sizeof(head_info));				
				

				ret = get_data_head(read_buffer,len,&head_info);
				if( ret < 0 )
				{
					read_buffer[0] = 0x00;
					continue;
				}
				
				//�������Ƿ�С��Ҫ����ĳ���
				
				if( len < head_info.length + sizeof(struct data_info_header ))
				{
					continue;
				}
				
				ret =  deal_with_indata(read_buffer,len);
				if( ret < 0 )
				{
					tcflush(ttyS1, TCIFLUSH);
					usleep(100);
					PRINTF(debug_level,"WARING: deal with data has misstake !");
				}
				len  = 0;
				memset(read_buffer,0x00,sizeof(read_buffer));

		}
		return NULL;
}

/* return value :
 *	     0 : normal return
 *	   -1 : link layer error.
 *     -2 : be canceled by receiver.
 */

int check_packet_info(pro_packet_info_t *get_info,pro_packet_info_t * src_info)
{
	if( src_info->dev_id == get_info->dev_id
		 && src_info->num == get_info->num )
	{
		return 0;
	}
	else
	{
		printf("SRC=[%d:%d],GET=[%d:%d]\n",src_info->dev_id,src_info->num,get_info->dev_id,get_info->num);
		return -1;
	}
}

int  feed_data_inpacket(pro_packet_info_t *ppkt_info,int *data_len,unsigned char *pdata)
{
	int  i = 0; 
	int  j = 0;
	int  src_len = *data_len;
	ppkt_info->header =  TTY_DATA_HEADER;
	for( i = 0 ,j = 0 ; j < PRO_MAX_DATADOMAIN_SIZE-1 && i < src_len ; i++)
	{
		if(0x00 == pdata[i] )
		{
			ppkt_info->data[j++] = 0xFF;
			ppkt_info->data[j++] = 0xF0;
		}
		else if (0xFF == pdata[i])
		{
			ppkt_info->data[j++] = 0xFF;
			ppkt_info->data[j++] = 0xFF;
		}
		else if (0xAA == pdata[i])
		{
			ppkt_info->data[j++] = 0xFF;
			ppkt_info->data[j++] = 0xFA;
		}
		else
		{
			ppkt_info->data[j++] = pdata[i];
		}
		
	}
	ppkt_info->length  = j ;
	//printf("send len = %d [%s:%d]\n",j,__func__,__LINE__);
	*data_len = *data_len - i ;  //����ʣ����ֽ���
	return  i;  //���ط����˶��ٸ��ֽ�
}


int send_data_to_other(pro_packet_info_t* pkt_info)
{
	int ret = 0;
	int send_len = 0;
	unsigned char TxBuffer[PRO_MAX_BUFFER_SIZE] = {0};
	memset(TxBuffer,0x00,sizeof(TxBuffer));
	//��䷢�ͻ��沢����
	pthread_mutex_lock(&mutex_tx_buf);
	send_len = pro_pack_data(pkt_info,TxBuffer);
	ret = write_port(ttyS1,TxBuffer,send_len);
	pthread_mutex_unlock(&mutex_tx_buf);
	if( ret < 0 )
	{
		PRINTF(debug_err,"send data failed !");
		return ret ;
	}
}


int wait_for_answer(pro_packet_info_t * psrc_packet_info,unsigned char cmd_id)
{	
	struct timeval send_time;
	struct timespec timeout;
	struct timeval tv_out ;  //added by yuemalin for printf time information
	pro_packet_info_t * pget_packet_info;
	int 	ret = 0;

	//�趨��ʱʱ��MAX_WAIT_SECONDS
	memset(&send_time,0x00,sizeof(send_time));
	gettimeofday(&send_time,NULL);
	timeout.tv_sec = send_time.tv_sec + MAX_WAIT_SECONDS;
	timeout.tv_nsec = send_time.tv_usec*1000;
	/* 经过测试,如果在wait中间改变系统时间,不会影响这里的超时 */
	ret = pthread_cond_timedwait(&cnd_wait_to_tx_data,&tx_sec_wind,&timeout);
	pget_packet_info = &pkt_info_table[0];
	if( 0 ==  ret) //�Զ�����Ӧ��
	{
		//�����Ӧ�������Ƿ���ȷ
		if(pget_packet_info->cmd == pro_cmd_nak)
		{	
				PRINTF(debug_waring,"WARING_rx: i received message pro_cmd_nak for (%s) ",cmd_id <= pro_cmd_eof ? packet_info[cmd_id]:"unknow message");
				return TRANS_AGAIN;
		}
		else if(pget_packet_info->cmd == pro_cmd_can)
		{
				PRINTF(debug_waring,"WARING_rx: i received message pro_cmd_nak for (%s) ",cmd_id <= pro_cmd_eof ? packet_info[cmd_id]:"unknow message");
				return TRANS_CANCLE_BY_OTHER;
		}	
		else if(pget_packet_info->cmd == pro_cmd_ack )
		{
			if(! check_packet_info(pget_packet_info,psrc_packet_info))
			{
		 		PRINTF(debug_level,"INFO_rx:SUCCESS:i has received an \"pro_cmd_ack\" answer for %s",cmd_id <= pro_cmd_eof ? packet_info[cmd_id]:"unkonw message");
				return TRANS_OK;
			}
			else
			{
				return TRANS_ANSWER_ERROR;
			}
		}
		else
		{
		 	PRINTF(debug_level,"INFO_rx:i has received an answer about %s",cmd_id <= pro_cmd_eof ? packet_info[cmd_id]:"unkonw message");
		}	
	}
	else if(ret == ETIMEDOUT)
	{
		 	PRINTF(debug_waring,"WARING_rx:wait for answer time out %s",cmd_id <= pro_cmd_eof ? packet_info[cmd_id]:"unkonw message");
			return TRANS_OUT_TIME;
	}
	else
	{
			PRINTF(debug_err,"some err happened while waiting .....");
			return TRANS_FAILED;
	}

	return 0;
}



int send_last_packet_to_other(pro_packet_info_t pkt_info,int packet_num )
{
	int resend  = 0;
	int ret 	= 0;
	//���ͽ��հ�,�Ա�Զ����ϲ�Э�鷢����ݰ�
	if(++packet_num == MAX_PACKET_NUM )
	{
			packet_num = MIN_PACKET_NUM;
	}

	pkt_info.cmd   = pro_cmd_eof;
	pkt_info.num = packet_num;
	pkt_info.length = 1;
	memset(pkt_info.data,0x00,PRO_MAX_DATADOMAIN_SIZE);
	pkt_info.data[0] = TTY_ANSWER_DATA;
	PRINTF(debug_level,"INFO: i will send pro_cmd_eof and wait for ask ");
	resend = 0;
	while( resend++ < MAX_TRY_SEND_NUM)
	{

		//��䷢�ͻ��沢����
		ret = send_data_to_other(&pkt_info);
		if( ret < 0 )
		{
			return SEND_FAILED;
		}
		//�ȴ�Զ�Ӧ��
		ret = wait_for_answer(&pkt_info,pkt_info.cmd);
		switch( ret )
		{
			case TRANS_OK:
				 return SEND_OK;
				 break;			
			case TRANS_ANSWER_ERROR:  //��Щ����Ҫ�ش�	
			case TRANS_AGAIN: 			
			case TRANS_OUT_TIME:
				 continue;
				 break;	
			case TRANS_FAILED:
				 return SEND_FAILED;
				 break;
			case TRANS_CANCLE_BY_OTHER:
				 return SEND_CANCLED;
				 break;
			default:
				break;
		}
		usleep(1000);
	}
	return  SEND_FAILED;
}



int inline send_data_out(pro_packet_info_t *pdata)
{	
	int ret = 0;
	int try_time = 3;
	while(try_time)
	{	
		try_time --;
		ret = send_data_to_other(pdata);
		if( ret < 0 )
		{
			return SEND_FAILED;
		}
		//�ȴ�Զ�Ӧ��
		ret = wait_for_answer(pdata,pdata->cmd);
		switch( ret )
		{
			case TRANS_OK:
				 return SEND_OK;
				 break;			
			case TRANS_ANSWER_ERROR:  //��Щ����Ҫ�ش�	
			case TRANS_AGAIN: 			
			case TRANS_OUT_TIME:
				 continue;
				 break;	
			case TRANS_FAILED:
				 return SEND_FAILED;
				 break;
			case TRANS_CANCLE_BY_OTHER:
				 return SEND_CANCLED;
				 break;
			default:
				break;
		}
		usleep(1000);
	}

	return  SEND_FAILED;
}











//����һ����ݰ�
int tx_data_info(pro_data_info_t *pdata_info)
{
	int data_len  = 0;
	char * pdata  =  NULL;  
	pro_packet_info_t pkt_info,*ppkt_info;
	int  packet_num	 = 0;
	int  ret  =  0;
	int  send_len = 0;
	int  resend = 0 ;
	int  send_ok = NO;
	unsigned char cmd_id  = 0;

		
	//��ʼ��tty���Ͱ����Ϣͷ
	memset((void*)&pkt_info,0x00,sizeof(pkt_info));

	pkt_info.dev_id = pdata_info->dev_id;
	pkt_info.cmd   = pro_cmd_sof;
	cmd_id  = pro_cmd_sof;
	pdata = pdata_info->data;
	data_len = 	pdata_info->length;
	packet_num = 0;

	while(data_len)
	{
		//�޸İ�ı��
		if( ++packet_num == MAX_PACKET_NUM)
		{
			packet_num = MIN_PACKET_NUM;
		}
		//�����ݰ���ԭ�е����
		memset(pkt_info.data,0x00,PRO_MAX_DATADOMAIN_SIZE);		

		//��װ�����
		ret = feed_data_inpacket(&pkt_info,&data_len,pdata);
		pkt_info.num = packet_num;
		pdata += ret;
		resend = 0;
		send_ok = NO;

		//������ݰ�
		ret = send_data_out(&pkt_info);
		if( ret != SEND_OK )
		{
			return ret;
		}
		
		//���ͳɹ��޸�cmd
		if(pkt_info.cmd == pro_cmd_sof)  //�޸���ݱ�ʶ����ʾ���͵����м����
		{
				pkt_info.cmd = pro_cmd_tx;
		}
	}
	
	//���ͽ����
	
	ret = send_last_packet_to_other(pkt_info,packet_num);
	if( ret != SEND_OK )
	{
		PRINTF(debug_err,"send packet[%s] fialed !",pkt_info.cmd <= pro_cmd_eof?packet_info[pkt_info.cmd]:"unknow cmd");
		return ret;
	}

	return 0;		
}



/* return value :
 *	     device_id : if one data item is received wholely,we return its device id which it came from.
 *	   	0 : has no whole item yet.
 *     -1 : failed to receive segment of one data item..
 */

void send_answer_packet(unsigned char dev_id,unsigned char packet_num,unsigned char cmd,unsigned char cmd_src)
{
	pro_packet_info_t pkt_info;
	int length =  0;
	unsigned char TxBuffer[PRO_MAX_BUFFER_SIZE] = {0};
	memset(TxBuffer,0x00,sizeof(TxBuffer));
	memset(&pkt_info,0x00,sizeof(pkt_info));
	/* answer transmitor */
	pkt_info.dev_id = dev_id;
	pkt_info.num 	= packet_num;
	pkt_info.cmd 	= cmd;
	pkt_info.length = TTY_ANSWER_DATA_LEN; 
	memset(pkt_info.data,TTY_ANSWER_DATA,PRO_MAX_DATADOMAIN_SIZE);
	pthread_mutex_lock(&mutex_tx_buf);
	length = (unsigned long)pro_pack_data(&pkt_info,TxBuffer);
	write_port(ttyS1,TxBuffer,length);
	pthread_mutex_unlock(&mutex_tx_buf);

	PRINTF(debug_level,"INTFO_rx:i has send  [%s] packet for [%s] !",pkt_info.cmd <= pro_cmd_eof?packet_info[pkt_info.cmd]:"unknow cmd",
			cmd_src <= pro_cmd_eof?packet_info[cmd_src]:"unknow cmd");
	return ;
}

void send_answer_nack(unsigned char dev_id)
{
	pro_packet_info_t pkt_info;
	int length =  0;
	unsigned char TxBuffer[PRO_MAX_BUFFER_SIZE] = {0};
	memset(TxBuffer,0x00,sizeof(TxBuffer));
	memset(&pkt_info,0x00,sizeof(pkt_info));
	/* answer transmitor */
	pkt_info.dev_id = dev_id;
	pkt_info.cmd 	= pro_cmd_nak;
	pkt_info.length = 1; 
	memset(pkt_info.data,TTY_ANSWER_DATA,PRO_MAX_DATADOMAIN_SIZE);
	pthread_mutex_lock(&mutex_tx_buf);
	length = pro_pack_data(&pkt_info,TxBuffer);
	write_port(ttyS1,TxBuffer,length);
	pthread_mutex_unlock(&mutex_tx_buf);
	
	return ;
}


//���ճ�ʼ��ݰ�
int deal_with_sof_packet(pro_packet_info_t *ppkt)
{
	int ret 	= 0;
	int dev_id 	= 0;
	int packet_num_index  =0;
	int data_len 	 = 0;
	
	pro_data_info_t *pItem 	= NULL;
	unsigned char *pch 		= NULL;

	dev_id= ppkt->dev_id;
	packet_num_index = dev_id -1;

	PRINTF(debug_level,"INTFO_rx:i received [%s] packet",ppkt->cmd <= pro_cmd_eof?packet_info[ppkt->cmd]:"unknow cmd");

	if(dev_id < MAX_EXTBOARD)
	{

		pItem = DataItemList[packet_num_index];
	}
	else
	{
		ret = 0;
		return ret;
	}
	
	//1.��ʼ����������
	if(pItem == NULL)
	{
		pItem = (pro_data_info_t *)malloc(sizeof(pro_data_info_t));
		if(pItem == NULL)
		{
			send_answer_packet(ppkt->dev_id,ppkt->num,pro_cmd_can,pro_cmd_sof);
			return -1;
		}
		DataItemList[packet_num_index] = pItem;
	}
	else
	{
		if(pItem->data)
		{
			free(pItem->data);
		}
	}

	memset((void*)pItem, 0,sizeof(pro_data_info_t));
	pItem->dev_id = dev_id;
	pItem->data = NULL;
	PktNum[packet_num_index] = MIN_PACKET_NUM;  

	//2.�����ݰ�ı���Ƿ���ȷ
	if(PktNum[packet_num_index] != ppkt->num)  
	{
		//��ݰ�����Ҫ����ݰ���Ҫ�������أ�����û��˵��
		PRINTF(debug_waring,"WARING_rx:[%s] packet but num[%d] is wrong!",ppkt->cmd <= pro_cmd_eof?packet_info[ppkt->cmd]:"unknow cmd",ppkt->num);
		send_answer_packet(ppkt->dev_id,ppkt->num,pro_cmd_ack,pro_cmd_sof);
		return ret;
	}
	
	//3. ��ȡ���
	pch = pItem->data;
	data_len = pItem->length + ppkt->length;
	pItem->data = realloc((void*)pch,data_len);
	if(pItem->data == NULL)
	{
		printf("%s %s %d: failed to allocate more memory for pItem->data\n",__FILE__,__func__,__LINE__);

		//������ݻ�����ʧ���ˣ�֪ͨ�Զ�
		if(pch != NULL)
		{
			free(pch);
		}
		free(pItem);
		DataItemList[packet_num_index] = NULL;
		send_answer_packet(ppkt->dev_id,ppkt->num,pro_cmd_can,pro_cmd_sof);
		return -1;
	}

	pch = pItem->data + pItem->length;
	memset(pch,0,ppkt->length);
	memcpy(pch, ppkt->data, ppkt->length );
	pItem->length = data_len;
	
	//4.�޸���ݱ��(������һ����ı��)
	PktNum[packet_num_index]++;
	if( MAX_PACKET_NUM == PktNum[packet_num_index]) 
	{
		PktNum[packet_num_index] = MIN_PACKET_NUM;
	}
			
	//5.������Ӧ
	send_answer_packet(ppkt->dev_id,ppkt->num,pro_cmd_ack,pro_cmd_sof);

	return 0;
}

//���ճ�ʼ��ݰ�
int deal_with_eof_packet(pro_packet_info_t *ppkt)
{
	int ret 	= 0;
	int dev_id 	= 0;
	int packet_num_index  =0;
	int data_len 	 = 0;
	
	pro_data_info_t *pItem 	= NULL;
	unsigned char *pch 		= NULL;

	
	dev_id= ppkt->dev_id;
	packet_num_index = dev_id -1;

	PRINTF(debug_level,"INTFO_rx:i received [%s] packet",ppkt->cmd <= pro_cmd_eof?packet_info[ppkt->cmd]:"unknow cmd");

	if(dev_id < MAX_EXTBOARD)
	{

		pItem = DataItemList[packet_num_index];
	}
	else
	{
		ret = 0;
		return ret;
	}

	if(pItem == NULL)
	{
		/* received eof times*/
		PktNum[packet_num_index] = MIN_PACKET_NUM;
		PRINTF(debug_waring,"WARING_rx:i received [%s] packet,but save buffer is NULL",ppkt->cmd <= pro_cmd_eof?packet_info[ppkt->cmd]:"unknow cmd");
		send_answer_packet(ppkt->dev_id,ppkt->num,pro_cmd_ack,pro_cmd_eof);
		return -1;
	}
	
	//1.�����������뵽����������	
	pthread_mutex_lock(&mutex_rx_list);
	
	append_to_list_tail(&list_rx,pItem);
	//2.���½�������Ľ�����Ŀ��
	pthread_mutex_lock(&mutex_listen);
	dev_cnt_r[dev_id]++;
	mark_r |= (1 << dev_id);
	in_listen = 0;
	pthread_mutex_unlock(&mutex_listen);
	DataItemList[packet_num_index] = NULL;
	ret = packet_num_index;
	pthread_mutex_unlock(&mutex_rx_list);

	//3.������Ӧ
	send_answer_packet(ppkt->dev_id,ppkt->num,pro_cmd_ack,pro_cmd_eof);
	return 0;
}

//���ճ�ʼ��ݰ�
int deal_with_tx_packet(pro_packet_info_t *ppkt)
{
	int ret 	= 0;
	int dev_id 	= 0;
	int packet_num_index  =0;
	int data_len 	 = 0;
	
	pro_data_info_t *pItem 	= NULL;
	unsigned char *pch 		= NULL;

	
	dev_id= ppkt->dev_id;
	packet_num_index = dev_id -1;

	PRINTF(debug_level,"INTFO_rx:i received [%s] packet",ppkt->cmd <= pro_cmd_eof?packet_info[ppkt->cmd]:"unknow cmd");

	if(dev_id < MAX_EXTBOARD)
	{

		pItem = DataItemList[packet_num_index];
	}
	else
	{
		return -1;
	}

	if(pItem == NULL)
	{
		/* received eof times*/
		PktNum[packet_num_index] = MIN_PACKET_NUM;
		PRINTF(debug_waring,"WARING_rx:i received [%s] packet,but save buffer is NULL",ppkt->cmd <= pro_cmd_eof?packet_info[ppkt->cmd]:"unknow cmd");
		send_answer_packet(ppkt->dev_id,ppkt->num,pro_cmd_ack,pro_cmd_tx);
		return -1;
	}


	//1.�����ݰ�ı���Ƿ���ȷ
	if(PktNum[packet_num_index] != ppkt->num)  
	{
		//��ݰ�����Ҫ����ݰ���Ҫ�������أ�����û��˵��
		PRINTF(debug_waring,"WARING_rx:[%s] packet ,i need %d but received is %d!",ppkt->cmd <= pro_cmd_eof?packet_info[ppkt->cmd]:"unknow cmd",PktNum[packet_num_index],ppkt->num);
		send_answer_packet(ppkt->dev_id,ppkt->num,pro_cmd_ack,pro_cmd_tx);
		return ret;
	}
	
	//2. ��ȡ���
	pch = pItem->data;
	data_len = pItem->length + ppkt->length;
	pItem->data = realloc((void*)pch,data_len);
	if(pItem->data == NULL)
	{
		//printf("%s %s %d: failed to allocate more memory for pItem->data\n",__FILE__,__func__,__LINE__);

		//������ݻ�����ʧ���ˣ�֪ͨ�Զ�
		if(pch != NULL)
		{
			free(pch);
		}
		free(pItem);
		DataItemList[packet_num_index] = NULL;
		send_answer_packet(ppkt->dev_id,ppkt->num,pro_cmd_can,pro_cmd_tx);
		return -1;
	}

	pch = pItem->data + pItem->length;
	memset(pch,0,ppkt->length);
	memcpy(pch, ppkt->data, ppkt->length );
	pItem->length = data_len;
	
	//3.�޸���ݱ��(������һ����ı��)
	PktNum[packet_num_index]++;
	if( MAX_PACKET_NUM == PktNum[packet_num_index]) 
	{
		PktNum[packet_num_index] = MIN_PACKET_NUM;
	}
	//4.������Ӧ
	send_answer_packet(ppkt->dev_id,ppkt->num,pro_cmd_ack,pro_cmd_tx);

	return ret;
}




/* return value :
 *	     device_id : if one data item is received wholely,we return its device id which it came from.
 *	   	0 : has no whole item yet.
 *     -1 : failed to receive segment of one data item..
 */
int rx_data_info( pro_packet_info_t *ppkt)
{
		int ret = 0;
		unsigned char cmd_id;	
		cmd_id = ppkt->cmd;
		switch(cmd_id)
		{
			case pro_cmd_sof:
				 ret = deal_with_sof_packet(ppkt);
				 break;
			case pro_cmd_eof:
				 ret = deal_with_eof_packet(ppkt);
				 break;	
			case pro_cmd_tx:
				 ret = deal_with_tx_packet(ppkt);
				 break;
			default:
				 ret = 1;
				 PRINTF(debug_err,"i has received unknow tty data packet !");
				 break;
		}

	return ret;
}



/* append one item to the tail of list */
int append_to_list_tail(list_data_info_t *plist, pro_data_info_t *pdata_info)
{
		if(plist->header == NULL)
		{/* The list is empty */
				plist->header = plist->tail = pdata_info;
				pdata_info->next = NULL;
				pdata_info->pre = NULL;
		}
		else
		{
				plist->tail ->next = pdata_info;
				pdata_info->pre = plist->tail;
				pdata_info->next = NULL;
				plist->tail = pdata_info;
		}

		return 0;
}

/* Discription:
 * fetch one item from the header of list. 
 * return value:
 *	NULL : It is said that the list is empty
 *	not NULL : one pointor points to the item fetched from list header.
 */
pro_data_info_t*  fetch_from_list_header(list_data_info_t *plist)
{
		pro_data_info_t *pdata_info = NULL;
		if(plist->header == NULL)
		{/* the link is empty */
				return pdata_info;
		}
		else
		{/* the link is not empty */
				pdata_info = plist->header;
				plist->header = pdata_info->next;
				if(plist->header == NULL)
				{/* the link has only one item */
						/* The list is empty now. And there was only one item on this linked list before.*/
						plist->header = plist->tail = NULL;
				}
				pdata_info->next = NULL;
				pdata_info->pre = NULL;
				return pdata_info;
		}
}

int clear_tx_list(unsigned char idx)
{
		pro_data_info_t *pdata_info = NULL;
		while(1)
		{
				pdata_info = fetch_from_list_header(&list_tx[idx]);
				if(pdata_info == NULL)
						return 0;
				free(pdata_info->data);
				pdata_info->data = NULL;
				free(pdata_info);
				pdata_info = NULL;
		}
		return 0;
}


//added by yuemalin
int list_is_empty(list_data_info_t *plist)
{
	if(plist->header == NULL)
	{
		return 1;
	}	
	return 0;
}




pthread_t tid_rx = 0;
pthread_t tid_tx = 0;



int link_host_init(void)
{
		int ret = -1;
		unsigned char idx;

		ttyS0 = -1;
		ttyS1 = -1;
#ifndef PC_TEST
		ttyS1 = open_port("/dev/ttyS1");
		if(ttyS1 < 0)
		{
				printf("%s %s %d : failed to open %s\n", __FILE__,__func__,__LINE__,"/dev/ttyS1");
				return -1;
		}
		set_port(ttyS1);
#else
		ret = server_creat_unsock("/tmp/socket0",&ttyS1);
		if(ret < 0)
		{
				printf("%s %s %d : Failed to create virtual ttyS1\n", __FILE__,__func__,__LINE__);
				return -1;
		}
#endif

		/* initiate global variable */
		for(idx = 0; idx < MAX_EXTBOARD; idx++)
		{
				list_tx[idx].header = NULL;
				list_tx[idx].tail	   = NULL;
				list_rx.header = NULL;
				list_rx.tail	   = NULL;
		}

		mark_e = 0;
		mark_w = 0;


		ret = pthread_create(&tid_tx,NULL,thread_tx,NULL);
		if(ret != 0)
		{
				printf("failed to  pthread_create() : thread_rx\n");
				return -3;
		}

		ret = pthread_create(&tid_rx,NULL,thread_rx,NULL);
		if(ret != 0)
		{
				printf("failed to  pthread_create() : thread_rx\n");
				return -2;
		}


		return 0;
}


int link_ext_init(void)
{
		int ret = -1;
		unsigned char idx;
		pthread_attr_t attr;
		struct sched_param param;
		ttyS0 = -1;
		ttyS1 = -1;
#ifndef PC_TEST
		ttyS1 = open_port("/dev/ttyS1");
		if(ttyS1 < 0)
		{
				printf("%s %d %d : failed to open %s\n",__FILE__,__func__,__LINE__, "/dev/ttyS1");
				return -1;
		}
		set_port(ttyS1);
#else
		ret = client_creat_unsock("/tmp/socket0",&ttyS1);
		if(ret < 0)
		{
				printf("%s %s %d : Failed to create virtual ttyS1\n",__FILE__,__func__,__LINE__);
				return -1;
		}
#endif
		/* initiate global variable */
		for(idx = 0; idx < MAX_EXTBOARD; idx++)
		{
				list_tx[idx].header = NULL;
				list_tx[idx].tail	   = NULL;
				list_rx.header = NULL;
				list_rx.tail	   = NULL;
		}

		mark_e = 0;
		mark_w = 0;



		ret = pthread_create(&tid_tx,NULL,thread_tx,NULL);
		if(ret != 0)
		{
				printf("failed to  pthread_create() : thread_rx\n");
				return -3;
		}

		ret = pthread_create(&tid_rx,NULL,thread_rx,NULL);
		if(ret != 0)
		{
				printf("failed to  pthread_create() : thread_rx\n");
				return -2;
		}

		return 0;
}


int link_exit(void)
{
		if(ttyS0 > 0)
				close_port(ttyS0);
		if(ttyS1 > 0)
				close_port(ttyS1);

		exit(0);
		return 0;
}

int link_send( int dst,  unsigned char *buf, unsigned int len )
{
		//printf("%s %s() %d \n",__FILE__,__func__,__LINE__);
		int ret ;
		int i = 0;
		pro_data_info_t *pdata_info = NULL;

		if(buf == NULL || len == 0)
		{
				return 0;
		}
		pdata_info = ( pro_data_info_t *)malloc(sizeof(pro_data_info_t));
		if(pdata_info == NULL)
		{
				return -1;
		}

		memset((void*)pdata_info,0x00,sizeof(pro_data_info_t));
		pdata_info->dev_id = dst;
		pdata_info->length = len;
		pdata_info->data = (unsigned char *)malloc(len);
		if(pdata_info->data == NULL)
		{
				free(pdata_info);
				return -2;
		}
		
		//���ڵ����
		memset((void *)(pdata_info->data),0x00,len);
		memcpy(pdata_info->data,buf,len);

		//�������ӵ�����������
		pthread_mutex_lock(&mutex_tx_list);
		append_to_list_tail(&list_tx[dst], pdata_info);
		if(tx_alive == 0)
		{
				tx_alive = 1;
		}
		pthread_mutex_unlock(&mutex_tx_list);

		return 0;
}

/* Even if when there is data in rx data linked list, this function is be called.
 * But, to prevent accidental operation, a chech action is added */
int link_recv( int dev_id,  pro_data_info_t **ppdata )
{
		pro_data_info_t *pdata_info = NULL;
		if(list_rx.header == NULL)
				return -1;
		pthread_mutex_lock(&mutex_rx_list);
		for(pdata_info = list_rx.header; pdata_info; pdata_info = pdata_info->next)
		{
				if(pdata_info->dev_id == dev_id)
				{/* have found the data item that we wanted */
						/* all data items sended from extended board whos device id is dev_id have been read out */
						pthread_mutex_lock(&mutex_listen);
						--dev_cnt_r[dev_id];
						if(dev_cnt_r[dev_id] == 0)
						{
								mark_r &= ~(1 << dev_id);
						}
						pthread_mutex_unlock(&mutex_listen);

						if(pdata_info->pre)
						{/* This data item is  not first item of rx data link*/

								if(pdata_info->next)  
								{/* This data item is not the first item and not the tail of rx data link*/
										pdata_info->pre->next = pdata_info->next;
										pdata_info->next->pre = pdata_info->pre;
								}
								else
								{/* This data item is not the first item but is the tail of rx data link*/
										pdata_info->pre->next = NULL;
								}
						}
						else
						{/* This data item is first item of rx data link*/
								if(pdata_info->next)
								{/* is first item of rx data link but is not the only one */
										list_rx.header = pdata_info->next;
										pdata_info->next->pre = NULL;
								}
								else
								{/* is first item of rx data link and is the only one */
										list_rx.header = NULL;
										list_rx.tail       = NULL;
								}
						}
						*ppdata = pdata_info;
						pthread_mutex_unlock(&mutex_rx_list);
						return 0;
				}
		}

		return -1;
}

/*
 * device id bit map.
 * e.g. If one extended board send message to our main board, we record it by set the responded bit.
 *       If the device id is 3, we set the 3rd bit of mark_r(unsigned short).
 */

int link_listen ( unsigned short *w_devset,  unsigned short *r_devset,  unsigned short *e_devset)
{

		//printf("%s %s() %d \n",__FILE__,__func__,__LINE__);
		int ret = 0 , i = 0 ;
		unsigned char repeat = 0;
		pro_data_info_t  *pdata_info;
		unsigned short sh_w = 0, sh_r = 0, sh_e = 0;

		pthread_mutex_lock(&mutex_listen);
		while(1)
		{
				/* to check whether it's failed to ** tx data ** to some device */
				if(w_devset)
						sh_w = *w_devset;
				if(sh_w != 0)
				{
						sh_w = sh_w & mark_w;	
						if(sh_w)		
						{			
								for(i=0;i<16;i++)			
								{						
										if((sh_w >> i) & 0x01)					
												ret++;			
								}			
								*w_devset = sh_w;
						}
				}
				/* to check whether there is **data sended** by from some device */
				if(r_devset)
				{
						sh_r = *r_devset;
						if(sh_r != 0)
						{
								sh_r &= mark_r;	
								if(sh_r)		
								{	
										for(i=0;i<16;i++)			
										{						
												if((sh_r >> i) & 0x01)
												{
														ret++;			
												}
										}			
										*r_devset = sh_r;
								}
						}
				}
				/* to check whether there are some device that **disconnected** to this device */
				if(e_devset)
						sh_e = *e_devset;
				if(sh_e != 0)
				{
						sh_e = sh_e & mark_e;	
						if(sh_e)		
						{			
								for(i=0;i<16;i++)			
								{						
										if((sh_e >> i) & 0x01)					
												ret++;			
								}			
								*e_devset = sh_e;
						}
				}
				if(ret != 0 || repeat != 0)
				{
						pthread_mutex_unlock(&mutex_listen);
						return ret;
				}

				repeat++;
				in_listen = 1;
				pthread_mutex_unlock(&mutex_listen);
				while(1)
				{
						pthread_mutex_lock(&mutex_listen);
						if(in_listen)
						{
								pthread_mutex_unlock(&mutex_listen);
								usleep(50000);  // 50ms
						}
						else
								break;
				}
		}

}


//added by yuemalin for  main board led contrl
int hw_led_on(unsigned char led_index, unsigned char color)
{
		unsigned char proTxBuffer[PRO_MAX_BUFFER_SIZE] = {0};
		if( led_index > MAX_MAIN_BOARD_LED_INDEX ||  color >  MAX_MAIN_BOARD_LED_COLOR )
		{
				printf("main borad led set param is misstake led_index=%d,color=%d [%s:%d]\n",led_index,color,__func__,__LINE__);
				return -1;
		}
		memset(proTxBuffer,0,PRO_MAX_BUFFER_SIZE);
		proTxBuffer[0] = MAIN_LED_CONTRL_CMD;
		proTxBuffer[1] = led_index;
		proTxBuffer[2] = color;
		proTxBuffer[3] = led_index+color;  //CRC=led_index+led_color
		pthread_mutex_lock(&mutex_tx_buf);
		write_port(ttyS1,proTxBuffer,MAIN_LED_CONTRL_LEN);
		pthread_mutex_unlock(&mutex_tx_buf);
		return 0;
}

int hw_led_off(unsigned char led_index)
{
		unsigned char proTxBuffer[PRO_MAX_BUFFER_SIZE] = {0};
		if( led_index > MAX_MAIN_BOARD_LED_INDEX )
		{
				printf("main borad led set param is misstake led_index=%d, [%s:%d]\n",led_index,__func__,__LINE__);
				return -1;
		}
		memset(proTxBuffer,0,PRO_MAX_BUFFER_SIZE);
		proTxBuffer[0] = MAIN_LED_CONTRL_CMD;
		proTxBuffer[1] = led_index;
		proTxBuffer[2] = 0;
		proTxBuffer[3] = led_index+0;  //CRC=led_index+led_color
		pthread_mutex_lock(&mutex_tx_buf);
		write_port(ttyS1,proTxBuffer,MAIN_LED_CONTRL_LEN);
		pthread_mutex_unlock(&mutex_tx_buf);
		return 0;
}



//added by yuemalin for clear tx_list 
void link_clear_all_list(void)
{	
	int i  = 0;
	//��շ�������
	for( i = 0 ; i < MAX_EXTBOARD ; i++)
	{
		pthread_mutex_lock(&mutex_tx_list);
		clear_tx_list(i);
		pthread_mutex_unlock(&mutex_tx_list);
	}
}





#if 0
int main(int argc, char *agv[])
{
		int ret = -1;
		void *tret;
		int sup,sub;
		pthread_t tid_mo;
		pthread_t tid_rx;
		pro_data_info_t data_info;
		pro_data_info_t *pdata; 
		unsigned char str0[5] = {0x0D,0x0A,0x0D,4,0x0D};
		unsigned char str1[5] = {6,7,8,9,0};
		//unsigned char str2[5] = {0x3F,0x3E,0x0D,0x0A,0x3C};
		unsigned char str2[256] = {0};
		unsigned char str3[5] = {0,9,8,7,6};
		int i,j;
		int cnt = 0;

		link_init();
		pthread_create(&tid_mo,NULL,thread_monitor,NULL);
		pthread_create(&tid_rx,NULL,thread_rx,NULL);

		for(i=0;i<256;i++)
				str2[i] = i;
#if 0
#ifdef MAINBOARD

		sleep(5);
		//for(i = 0; i < 5; i++)
		{
				link_send(0x01, str0, 5);
				sleep(3);
				link_send(0x01, str1, 5);
				sleep(3);
				link_send(0x01, str2, 5);
				sleep(3);
				link_send(0x01, str3, 5);
				sleep(3);
		}

		/*
		   sleep(5);
		   unsigned char char_array[5][2048];
		   for(i = 0; i < 5; i++)
		   {
		   for(j=0;j<2048;j++)
		   {
		   char_array[i][j] = j%(i+5);
		   }	
		   }
		//for(i = 0; i < 10; i++)
		{
		link_send(0x01, char_array[0], 2048);
		sleep(1);
		link_send(0x01, char_array[1], 2048);
		sleep(1);
		link_send(0x01, char_array[2], 2048);
		sleep(1);
		link_send(0x01, char_array[3], 2048);
		sleep(1);
		link_send(0x01, char_array[4], 2048);
		sleep(1);
		}
		 */


		//sleep(5);
#else
		unsigned short wset = 0, eset = 0, rset = 0;
		unsigned char buf[2048];
		int len = 2048;
		//sleep(2);
		//for(i = 0; i < 10; i++)
		while(1)
		{
				rset |= 0x101; // 0x08 is the number of mainboard
				ret = link_listen(NULL , &rset , NULL);
				if(ret <= 0)
				{
						printf(" There in no data sended from  mainboard \n");
				}
				else
				{
						//if(rset & (1<< 8))
						{	
								//ret = link_recv(0x08,buf,len);
								ret = link_recv(&pdata);
								for(j=0;j<pdata->length;j++)
								{
										printf("  %d", pdata->data[j]);
								}
								printf("\n");
								free(pdata->data);
								free(pdata);
								//memset(buf, 0, 2048);
						}
						if(cnt++ >= 3)
								break;
				}
				sleep(3);
		}
		//sleep(10);
#endif
#endif

#ifndef MAINBOARD
		sleep(3);
		link_send( str2, 256);
		sleep(3);
		//link_send( str2+128, 128);
		//link_send( str1, 5);
		sleep(3);
		//link_send( str2+128, 64);
		//link_send(str2, 5);
		sleep(3);
		//link_send( str2+192, 64);
		//link_send(str3, 5);
		sleep(3);


		/*
		   sleep(5);
		   unsigned char char_array[5][2048];
		   for(i = 0; i < 5; i++)
		   {
		   for(j=0;j<2048;j++)
		   {
		   char_array[i][j] = j%(i+5);
		   }	
		   }
		//for(i = 0; i < 10; i++)
		{
		link_send(0x01, char_array[0], 2048);
		sleep(1);
		link_send(0x01, char_array[1], 2048);
		sleep(1);
		link_send(0x01, char_array[2], 2048);
		sleep(1);
		link_send(0x01, char_array[3], 2048);
		sleep(1);
		link_send(0x01, char_array[4], 2048);
		sleep(1);
		}
		 */


		//sleep(5);
#else
		unsigned short wset = 0, eset = 0, rset = 0;
		unsigned char buf[2048];
		int len = 2048;
		//sleep(2);
		//for(i = 0; i < 10; i++)
		sleep(5);
		while(1)
		{
				rset = (1<< 1); // device id of first extended board
				//printf("%X\n",rset);
				ret = link_listen(NULL , &rset , NULL);
				if(ret <= 0)
				{
						printf(" There in no data sended from  mainboard \n");
				}
				else
				{
						//if(rset & (1<< 8))
						{	
								//ret = link_recv(0x08,buf,len);
								ret = link_recv(0x01,&pdata);
								for(j=0;j<pdata->length;j++)
								{
										printf("  %d", pdata->data[j]);
								}
								printf("\n");
								free(pdata->data);
								free(pdata);
								//memset(buf, 0, 2048);
						}
				}
				sleep(3);
		}
		//sleep(10);
#endif

		while(20);
		//link_exit();

		//pthread_join(tid_mo,NULL);
		//pthread_join(tid_rx,NULL);
		//close_uart(sup,sub);
		return 0;
}

#endif
