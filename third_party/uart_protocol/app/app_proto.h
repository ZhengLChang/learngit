
#ifndef _APP_PROTO_H_
#define _APP_PROTO_H_

	
extern unsigned char proto_verison;
extern unsigned char fw_version;
extern unsigned char bcf_version;

extern unsigned char clients_proto_verison[10];
extern unsigned char clients_fw_version[10];
extern unsigned char host_proto_version;
extern unsigned char host_fw_version;


#define APP_PRO_IDX_CMD_LENGTH 0
#define APP_PRO_IDX_DATA_START 1

#define MAX_NAME_LENGTH 128
#define MAX_PATH_LENGTH 512
#define MAX_ONELINE_LENGTH 512

#ifdef PC_TEST
#define APP_PROTO_WORK_PATH	"./"
#define APP_PROTO_BCF_PATH 	APP_PROTO_WORK_PATH"minigui/res/bcf.xml"
#define APP_PROTO_BLF_PATH  APP_PROTO_WORK_PATH"../data/tmp/dsscfg/blf.xml"
#define TOP_IMAGES_PATH 	APP_PROTO_WORK_PATH"minigui/res/pic"
#define BLF_IMAGES_PATH 	APP_PROTO_WORK_PATH"minigui/res/pic/blf"
#define BCF_IMAGES_PATH 	APP_PROTO_WORK_PATH"minigui/res/pic/bcf"
#define APP_PROTO_FW_PATH   APP_PROTO_WORK_PATH"../data/exp.tar.gz"
#else
#define APP_PROTO_WORK_PATH	"./"
#define APP_PROTO_BCF_PATH 	APP_PROTO_WORK_PATH"minigui/res/bcf.xml"
#define APP_PROTO_BLF_PATH  APP_PROTO_WORK_PATH"../data/tmp/dsscfg/blf.xml"
#define TOP_IMAGES_PATH 	APP_PROTO_WORK_PATH"minigui/res/pic"
#define BLF_IMAGES_PATH 	APP_PROTO_WORK_PATH"minigui/res/pic/blf"
#define BCF_IMAGES_PATH 	APP_PROTO_WORK_PATH"minigui/res/pic/bcf"
#define APP_PROTO_FW_PATH   APP_PROTO_WORK_PATH"../data/exp.tar.gz"
#endif

#define TRANS_START          0x0F
#define TRANS_CONTINUE       0xF0
#define TRANS_END            0xF1

//added by yuemalin   2017.07.09
#define MAIN_LCD_TYPE		0
#define SUB_LCD_TYPE		1



#define 	MSG_BUF_SIZE 10
struct message
{
	long msg_type;
	char msg_text[MSG_BUF_SIZE];
};

typedef enum
{
	e2m_login = 1, 		// 1
	e2m_req_bcf,		// 2
	e2m_req_blf,		// 3
	e2m_req_image,		// 4
	e2m_send_key,		// 5
	m2e_send_stat,		// 6
	m2e_send_blk,		// 7
	m2e_reset_ex,		// 8
	m2e_up_fw,		// 9
	m2e_send_image,		// 10
	m2e_send_lcd_backlight,  //11 added by yuemalin for change ext lcd backlight
	m2e_send_led_info,		 // 12 added by yuemalin for main to extern led status
	e2m_ask_status,			 // 13 added by yuemalin for extern ask status infomation from main board
	m2e_cmd,				 //14 main mechine send extern cmd to extern machine
	e2m_cmd,				 //15 main mechine send extern cmd to extern machine
}app_cmd_t;


typedef struct
{
	unsigned char dev_id;
	unsigned char proto_version;
	unsigned char fw_version;
	unsigned char info_reserved;
}app_proto_login_t;

typedef struct
{
	unsigned char bck_version;
	unsigned char reserved;
}app_proto_ack_login_t;

typedef struct
{
	unsigned char flag_cs;  //flag of start sending or continue sending or end sending
	unsigned char *content;
	unsigned int len_content;
}app_proto_fixed_file_t;


typedef struct
{
	unsigned char dev_id;
	unsigned char flag;
	unsigned char image_name[MAX_PATH_LENGTH];
}app_proto_req_image_t;

typedef struct
{
	unsigned char dev_id;
	unsigned char flag_cl;
	unsigned char flag_cs;  //flag of continue sending 
	unsigned char image_name[MAX_PATH_LENGTH];
	unsigned char len_name;
	unsigned char *content;
	unsigned int len_content;
}app_proto_ack_image_t;


typedef struct
{
	unsigned char dev_id;
	unsigned char key_id;
	unsigned char info;
}app_proto_e_send_key_t;

typedef struct
{
	unsigned char key_id;
	unsigned char status;
}app_proto_send_status_t;

//added by yuemalin for extern change LCD backlight value
typedef struct
{
		unsigned char lcd_type;     //LCD类型: 0==> MAIN LCD 1 ==>SUB LCD
		unsigned char lcd_bl_value; //LCD backlight value :[0~9]
}lcd_backlight_t;

//added by yuemalin for extern change led status
typedef struct
{
		unsigned char led_num;            //LED编号
		unsigned char led_color;          //LED显示什么颜色0:灭 1:红 2:绿色 3:橙色
		unsigned char led_twinkle;        //LED的显示状态      0: 非闪烁 1:闪烁
		unsigned char led_interval;       //LED的闪烁间隔,   只有在led_twinkle == 1的时间才有效  0 : 600ms闪烁间隔 1: 120ms闪烁间隔
}led_status_info;

typedef enum{
	LCD_TEST,
	LED_TEST,
	NO_USED,
}CMD_EX;

typedef struct{
	unsigned char cmd;   //command
	unsigned char cmd_len;   //command len
	unsigned char pvalue[0];  //command value
}cmd_tlv,*pcmd_tlv;

#define EX_CMD_LEN(x) (x+sizeof(cmd_tlv)) 


typedef struct core_to_gui_callback
{
	int (*to_gui_start_gui)(void);
	int (*to_gui_status)(unsigned char key_id, unsigned char status) ;
	int (*to_gui_image)(unsigned char *name);
	int (*to_gui_reset)(void);
	int (*to_gui_login)(int dev_id,int fw_version);
	int (*to_gui_upgrade_start)(void );	
	int (*to_gui_upgrade)(void );	
	int (*to_gui_update_bcf)(void); 
	int (*to_gui_update_blf)(void); 
	int (*to_gui_key)(unsigned char dev_id,unsigned char key_id,unsigned char info);
	int (*change_login_status)(int status);
	int (*change_extern_lcd_backlight)(lcd_backlight_t lcd_bl);
	int (*change_extern_led_status)(led_status_info led_info);
	int (*notice_get_all_blf_image_done)(void);   //added by yuemalin for ext get all blf's image to notice app
	int (*notice_get_all_bcf_image_done)(void);   //added by yuemalin for ext get all bcf's image to notice app
	int (*to_gui_ask_status)(int id);             //added by yuemalin for ext ask key status
	int (*to_gui_dealwith_cmd)(pcmd_tlv pcmd);    //added by yuemalin for ext ask key status

}core_to_gui_callback_t;

extern int app_proto_init(core_to_gui_callback_t *callback);
extern int login_to_mainboard(unsigned char proto_version, unsigned char fw_version);
extern int wait_and_handle_events(void);
extern int app_send_status(unsigned char dev_id, unsigned char key_id,unsigned char status);
extern int app_send_image(unsigned char dev_id, unsigned char *path);
extern int app_send_blf(unsigned char dev_id);
extern int app_send_reset(unsigned char dev_id);
extern int app_send_upgrade(unsigned char dev_id);
extern int app_send_key(unsigned char key_id, unsigned char info);
extern int app_main_send_cmd(unsigned char dev_id,unsigned char cmd,unsigned char * pdata,int pdata_len );
extern int app_extern_send_cmd(unsigned char dev_id,unsigned char cmd,unsigned char * pdata,int pdata_len );

#if 0
extern int core_to_gui_status(unsigned char key_id, unsigned char status);
extern int core_to_gui_image(unsigned char *name);
extern int core_to_gui_reset(void);
extern int core_to_gui_upgrade(void );  
extern int core_to_gui_update_bcf(); 
extern int core_to_gui_update_blf(); 
extern int core_to_gui_key(unsigned char dev_id,unsigned char key_id,unsigned char info);
#endif
extern int gui_to_core_key(unsigned char key_id,unsigned char info);
//added by yuemalin for exchang extern lcd backlight
extern int core_to_gui_change_extern_lcd_backlight(lcd_backlight_t extern_lcd_bl);
extern int core_to_gui_change_extern_led_status(led_status_info led_info);

extern int app_change_main_board_led_status(unsigned char led_index, unsigned char led_color); //added by yuemalin 
//added by yuemalin for ext get all blf's image ,and notice app 
extern int notice_app_ext_get_all_blf_image(void);
//added by yuemalin for ext get all blf's image ,and notice app 
extern int notice_app_ext_get_all_bcf_image(void);

//added by yuemalin for ext ask for key status information from 
extern int notice_app_ext_ask_key_status(int id) ;


#endif  // end of "#ifndef _APP_PROTO_H_"

