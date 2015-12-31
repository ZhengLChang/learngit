#ifndef _LINK_PROTO_H_
#define _LINK_PROTO_H_

#include <pthread.h>
#include <sys/time.h>

#define PRO_MAX_BUFFER_SIZE 		255		
#define PRO_MAX_DATADOMAIN_SIZE 		240	// 245 <= 255 - 3 - 2 - 3 = 247

#define PRO_VERSION 0
#define PRO_PKT_START_CHAR 0xAA   //在tty层使用0XAA开头，使用OX00结束
#define PRO_IDX_PKT_START 		0	
#define PRO_IDX_PKT_ID_CMD_NUM  1
#define PRO_IDX_PKT_DATALEN 	2
#define PRO_IDX_PKT_DATA_START 5
#define PRO_IDX_PKT_HEADER_LEN 5
#define PR0_IDX_PKT_CRC_START  3   //added by yuemalin


#define MAX_TRY_SEND_NUM       20

#define MAX_REPEAT_TIMES 20
#define MAX_WAIT_SECONDS 3

#define MAX_PACKET_NUM 	0x04
#define MIN_PACKET_NUM  0x01
#define TTY_DATA_HEADER	0xAA
#define TTY_ANSWER_DATA 0xEE
#define TTY_ANSWER_DATA_LEN  5

#define PRO_OD_FLAG 		255
#define MAIN_LED_CONTRL_CMD 0x55
#define MAIN_LED_CONTRL_LEN	0X04

#define CMD_BUFFER_LEN		1024

//added by yuemalin  for debug
#define DEBUG_LEVEL		5
#define PRINTF(level,fmt,argc...)  		\
do{                     				\
        if(level >= DEBUG_LEVEL )    	\
        {               				\
                struct timeval tv_out ;	\
                memset(&tv_out,0x00,sizeof(struct timeval));	\
                gettimeofday(&tv_out, NULL);	\
                printf(fmt"[sec:%ld,usec:%ld] [%s:%d]!\n",##argc,tv_out.tv_sec,tv_out.tv_usec,__func__,__LINE__);    \
        }									\
}while(0)

typedef enum
	{	
		pro_cmd_ack,  		// 0
		pro_cmd_nak,    	// 1
		pro_cmd_can,    	// 2
		pro_cmd_sof,    	// 3   
		pro_cmd_tx,      	// 4
		pro_cmd_eof,    	// 5  
		pro_cmd_timeout, 	//6  added by yuemalin
		pro_cmd_invalid,  	//7  added by yuemalin
	}protocol_cmd_t;

typedef struct pro_data_info
{
	unsigned int length;
	unsigned char dev_id;
	unsigned char *data;
	//unsigned char num;
	struct pro_data_info *pre;
	struct pro_data_info *next;
}pro_data_info_t;


typedef struct 
{
	unsigned char header;
	unsigned char dev_id:3;
	unsigned char cmd:3;
	unsigned char num:2;
	unsigned char length;
	unsigned char crc8[2];
	unsigned char data[PRO_MAX_DATADOMAIN_SIZE];
}pro_packet_info_t;
	
typedef struct
{
	pro_data_info_t *header;
	pro_data_info_t *tail;
}list_data_info_t;

#define MAX_EXTBOARD 10
extern pro_packet_info_t pkt_info_table[2] ;
extern unsigned char proRxBuffer[PRO_MAX_BUFFER_SIZE] ;
extern unsigned char proTxBuffer[PRO_MAX_BUFFER_SIZE];
extern int ttyS0;   //port which xmit data through
extern int ttyS1; // another tty  port except for tx_tty
extern list_data_info_t list_tx[MAX_EXTBOARD];	// tx linked list of tx data info
extern list_data_info_t list_rx; // rx linked list of rx data info
extern unsigned short  mark_e,  mark_w, mark_r;
extern unsigned char *ptmp;
extern unsigned char dev_cnt_r[MAX_EXTBOARD];
extern unsigned short cnt_r;

extern int link_init(void);
extern int link_exit(void);
extern int link_send( int dst,  unsigned char *buf, unsigned int len );
extern int link_recv( int dev_id,  pro_data_info_t **ppdata );
extern int link_listen ( unsigned short *w_devset,  unsigned short *r_devset,  unsigned short *e_devset );
#if 0   //modified by yuemalin for main board led 
extern int hw_led_on(unsigned char led_index, int , int);
extern int hw_led_off(unsigned char led_index);
#else
extern int hw_led_on( unsigned char led_index, unsigned char led_color);
extern int hw_led_off(unsigned char led_index);
#endif
#endif






