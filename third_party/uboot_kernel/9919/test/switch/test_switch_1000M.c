#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>			/* See NOTES */
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <arpa/inet.h>
#include <linux/sockios.h>

#include "test_switch.h"


static char switch_dev[]="/dev/atcom_switch";




#define SIOCDEVPRIVATE	0x89F0	/* to 89FF */


#define ARRAY_SIZE(X) (sizeof(X)/sizeof(X[0]))

typedef __signed__ int __s32;
typedef unsigned int __u32;


typedef struct switch_netpriv{
	__s32 bridge_mode;
	__s32 vlan_enable;
	__s32 vlan_check_id_enable;
	__s32 vlan_differentiated;
	__s32 diffserv;
	__u32 diffserv_value;
	__u32 voice_vid;
	__u32 data_vid;
	__u32 voice_priority;
	__u32 data_priority;
	__u32 sig_port[6];
	__u32 rtp_port_min;
	__u32 rtp_port_max; 
	__u32 enable_tx;
}atcom_switch_vlan,*patcom_switch_vlan;

typedef enum pc_port_subcmd{
	ATCOM_SET_CPU_PORT_VLAN_INFO = 0,
	ATCOM_GET_CPU_PORT_VLAN_INFO,
	ATCOM_CPU_PORT_VLAN_INFO_END,
	ATCOM_CPU_PORT_VLAN_INFO_MASK=0X0F,
}pc_port_subcmd_t,*ppc_port_subcmd_t;

#define ENABLE	1
#define DISABLE 0
typedef unsigned int          BOOL;

#define SIZEOF_ARRAY(X)  (sizeof(X)/sizeof(X[0]))
int enbale_type[]={
	DISABLE,
	ENABLE,
};
char * enbale_type_name[]={
	"DISABLE",
	"ENABLE",
};

#define RTL8367C_FIDMAX                     0xF
#define RTL8367C_PRIMAX                     7

#define PORT_PC				UTP_PORT1
#define PORT_LAN			UTP_PORT3
#define PORT_CPU			EXT_PORT0




rtk_vlan_acceptFrameType_t ingress_frame_type[]={
	ACCEPT_FRAME_TYPE_ALL,
	ACCEPT_FRAME_TYPE_TAG_ONLY, 	
	ACCEPT_FRAME_TYPE_UNTAG_ONLY,
};
char * ingress_file_name[]={
	"ACCEPT_FRAME_TYPE_ALL",
	"ACCEPT_FRAME_TYPE_TAG_ONLY", 	
	"ACCEPT_FRAME_TYPE_UNTAG_ONLY",
};

unsigned int switch_cmd[]={
	IFX_SET_VLAN_INIT,
	IFX_SET_VLAN_AWARE ,    				
	IFX_SET_PORT_VLANID ,   				
	IFX_SET_PORT_COS,       				
	IFX_SET_PORT_INGRESS_FRAME_TYPE, 	
	IFX_SET_PORT_INGRESS_FILETER,        
	IFX_SET_VLAN_TABLE_ENTRY,            
	IFX_SET_PORT_ETHERNET_CONF , 
	IFX_RESET_VLAN,

	IFX_GET_VLAN_AWARE,     				
	IFX_GET_PORT_VLANID ,   				
	IFX_GET_PORT_COS,       				
	IFX_GET_PORT_INGRESS_FRAME_TYPE, 	
	IFX_GET_PORT_INGRESS_FILETER,        
	IFX_GET_VLAN_TABLE_ENTRY,            
	IFX_GET_PORT_ETHERNET_CONF,          
	IFX_GET_PORT_PHY_STATUS , 
	IFX_GET_PORT_PHY_STATUS+1,
	IFX_GET_PORT_PHY_STATUS+2,

};
unsigned char *switch_cmd_name[]={
	"IFX_SET_VLAN_INIT",
	"IFX_SET_VLAN_AWARE",     				
	"IFX_SET_PORT_VLANID",    				
	"IFX_SET_PORT_COS"  ,     				
	"IFX_SET_PORT_INGRESS_FRAME_TYPE", 	
	"IFX_SET_PORT_INGRESS_FILETER" ,     
	"IFX_SET_VLAN_TABLE_ENTRY",          
	"IFX_SET_PORT_ETHERNET_CONF", 
	"IFX_RESET_VLAN",
    
	"IFX_GET_VLAN_AWARE"  ,   				
	"IFX_GET_PORT_VLANID" ,   				
	"IFX_GET_PORT_COS"  ,     				
	"IFX_GET_PORT_INGRESS_FRAME_TYPE" ,	
	"IFX_GET_PORT_INGRESS_FILETER",      
	"IFX_GET_VLAN_TABLE_ENTRY " ,        
	"IFX_GET_PORT_ETHERNET_CONF" ,       
	"IFX_GET_PORT_PHY_STATUS" , 
	"IFX_CONFIG_ATCOM_VLAN",
	"EXIT_TEST" , 		    
};

unsigned int port_index[]={
	PORT_LAN, 
	PORT_PC, 
	PORT_CPU, 
};
unsigned char * port_name[]={
	"PORT_LAN", 
	"PORT_PC",
	"PORT_CPU",	
	"UnKnow port" , 
};



/*port member*/
#define  PORT_MEMBER_LAN    (1 << PORT_LAN)
#define  PORT_MEMBER_PC     (1 << PORT_PC)
#define  PORT_MEMBER_CPU    (1 << PORT_CPU)
#define  PORT_MEMBER_ALL 	(PORT_MEMBER_LAN|PORT_MEMBER_PC|PORT_MEMBER_CPU)
#define  PORT_UNMEMBER_ALL	(0)


unsigned int membermask_index[]={
	PORT_MEMBER_LAN,
	PORT_MEMBER_PC,
	PORT_MEMBER_CPU,
	PORT_MEMBER_ALL,
	PORT_UNMEMBER_ALL,

}; 

unsigned char * membermask_name[]={
	"PORT_MEMBER_LAN",
	"PORT_MEMBER_PC",
	"PORT_MEMBER_CPU",
	"PORT_MEMBER_ALL",
	"PORT_UNMEMBER_ALL",
};














#define SIZEOF_ARRAY(X) (sizeof(X)/sizeof(X[0]))






typedef  struct atcom_1000M_switch_info{
	int net_id;
	int switch_id;
	atcom_switch_vlan net_info;
}atcom_1000M_switch_info_t,*patcom_1000M_switch_info_t;


static void uninit_fd_secriptinos(atcom_1000M_switch_info_t * pinfo)
{
	if( NULL == pinfo ){
		printf("Error: param is NULL [%s:%d]!\n");
		return ;
	}
	
	if( -1 != pinfo->net_id ){
		close(pinfo->net_id);
		pinfo->net_id =-1;

	}

	if( -1 != pinfo->switch_id ){
		close(pinfo->switch_id);
		pinfo->switch_id = -1;
	}
	return ;
}



static int init_fd_secriptinos(atcom_1000M_switch_info_t * pinfo)
{
	int ret 	= 0;
	int fd_net	= 0;
	int fd_switch	= 0;

	if( NULL == pinfo ){
		printf("Error: param is NULL [%s:%d]!\n");
		return -1;
	}

	pinfo->net_id 		= -1;
	pinfo->switch_id	= -1;
	
	fd_net = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == fd_net)
	{
	    printf("socket fail ! \r\n");
	    goto error;
	}
	printf("socket open ok !\r\n");
	pinfo->net_id = fd_net;
	

	fd_switch = open(switch_dev, 0, 0);
	if(fd_switch < 0){
	    goto error1;
	}

	pinfo->switch_id	= fd_switch;
	printf("\nInfo: open %s ok !\n",switch_dev);
	
	
	return 0;

error1:
	close(pinfo->net_id);
	pinfo->net_id = -1;
error:
	return -1;

}






void printf_vlan_infos(char * pstr,atcom_switch_vlan info)
{
	int i 	= 0;	 
	printf("\n====%s\n",pstr);
	printf("bridge_mode =%d\n",			info.bridge_mode );
	printf("vlan_enable=%d\n",			info.vlan_enable );
	printf("vlan_check_id_enable=%d\n",	info.vlan_check_id_enable );
	printf("vlan_differentiated=%d\n",	info.vlan_differentiated );
	printf("diffserv=%d\n",				info.diffserv);
	printf("diffserv_value=%d\n",		info.diffserv_value );
	printf("voice_vid=%d\n",			info.voice_vid );
	printf("data_vid=%d\n",				info.data_vid );
	printf("voice_priority=%d\n",		info.voice_priority);
	printf("data_priority=%d\n",		info.data_priority );
	for( i = 0 ; i < ARRAY_SIZE(info.sig_port);i++){
		printf("sig_port[i]=%d ",info.sig_port[i] = i);
	}
	printf("\nrtp_port_min=%d\n",	info.rtp_port_min );
	printf("rtp_port_max=%d\n", 	info.rtp_port_max);  
	printf("enable_tx=%d\n",		info.enable_tx);	
	printf("\n==============================================================\n");
	return ;
}


int get_enable_disable(BOOL * enable)
{
	int ret  	= 0;
	int index 	= 0;
	int	i		= 0;
	while(1){
		printf("please choose enable type:\n");
		for( i = 0; i < SIZEOF_ARRAY(enbale_type);i++){
			printf(" %d)=>%s",i,enbale_type_name[i]);
		}
		printf("\n");
		scanf("%d",&index);
		getchar();
		if( 0 > index  || SIZEOF_ARRAY(enbale_type) <= index ){
			printf("Error: enable type failed,please choose again !\n");
			continue;
		}else{
			*enable = enbale_type[index];
			break;
		}
	}
	
	return 0;
}


static int atcom_config_eth_bridge_mode(int *pbridge_mode)
{
	int bridge_mode = 0;
	int ret 		= 0;
	printf("\n\t please choose bridge mode:\n");
	ret  = get_enable_disable(&bridge_mode);
	if( ret != 0){
		printf("Error: bridge_mode Confihure failed !\n");
		return -1;
	}
	*pbridge_mode = bridge_mode;
}


static int atcom_config_eth_vlan_mode(int *pvlan_enable)
{
	int vlan_enable = 0;
	int ret 		= 0;
	printf("\n\t do you want enable vlan ?\n");
	ret  = get_enable_disable(&vlan_enable);
	if( ret != 0){
		printf("Error: bridge_mode Confihure failed !\n");
		return -1;
	}
	*pvlan_enable = vlan_enable;
}

static int atcom_config_eth_vlan_check_mode(int *pvlan_check_id_enable)
{
	int vlan_check_id_enable = 0;
	int ret 		= 0;
	printf("\n\t do you want enable vlan_check_id ?\n");
	ret  = get_enable_disable(&vlan_check_id_enable);
	if( ret != 0){
		printf("Error: bridge_mode Confihure failed !\n");
		return -1;
	}
	*pvlan_check_id_enable = vlan_check_id_enable;
}



static int atcom_config_eth_vlan_differentiated(int *pvlan_differentiated)
{
	int vlan_differentiated = 0;
	int ret 		= 0;
	printf("\n\t please input vlan_differentiated value:\n");
	scanf("%d",&vlan_differentiated);
	getchar();
	*pvlan_differentiated = vlan_differentiated;
	return 0;
}

static int atcom_config_eth_diffserv(int *pdiffserv)
{
	int diffserv = 0;
	int ret 		= 0;
	printf("\n\t please input diffserv value:\n");
	scanf("%d",&diffserv);
	getchar();
	*pdiffserv = diffserv;
	return 0;
}

static int atcom_config_eth_diffserv_value(unsigned int *pdiffserv_value)
{
	unsigned int diffserv_value = 0;
	int ret 		= 0;
	printf("\n\t please input diffserv_value value:\n");
	scanf("%uud",&diffserv_value);
	getchar();
	*pdiffserv_value = diffserv_value;
	return 0;
}



static int atcom_config_eth_voice_vid(unsigned int *pvoice_vid)
{
	unsigned int voice_vid = 0;
	int ret 		= 0;
	printf("\n\t please input voice_vid value:\n");
	scanf("%uud",&voice_vid);
	getchar();
	*pvoice_vid = voice_vid;
	return 0;
}

static int atcom_config_eth_data_vid(unsigned int *pdata_vid)
{
	unsigned int data_vid = 0;
	int ret 		= 0;
	printf("\n\t please input data_vid value:\n");
	scanf("%uud",&data_vid);
	getchar();
	*pdata_vid = data_vid;
	return 0;
}

static int atcom_config_eth_voice_priority(unsigned int *pvoice_priority)
{
	unsigned int voice_priority = 0;
	int ret 		= 0;
	printf("\n\t please input voice_priority value:\n");
	scanf("%uud",&voice_priority);
	getchar();
	*pvoice_priority = voice_priority;
	return 0;
}

static int atcom_config_eth_data_priority(unsigned int *pdata_priority)
{
	unsigned int data_priority = 0;
	int ret 		= 0;
	printf("\n\t please input data_priority value:\n");
	scanf("%uud",&data_priority);
	getchar();
	*pdata_priority = data_priority;
	return 0;
}



static int atcom_config_eth_sig_port(unsigned int *psig_port,int len)
{
	unsigned int 	value	 = 0;
	int 			ret 	 = 0;
	int 			i 	     = 0;
	printf("\n\t please input sig_port[0~%d]:\n",len);
	for( i = 0; i <  len ;i++ ){
		printf("please input sig_port[%d]'s value: \n",i);
		scanf("%uud",&value);
		getchar();
		psig_port[i] = value;
	}
	
	return 0;
}

static int atcom_config_eth_rtp_port_min(unsigned int *prtp_port_min)
{
	unsigned int rtp_port_min = 0;
	int ret 		= 0;
	printf("\n\t please input rtp_port_min value:\n");
	scanf("%uud",&rtp_port_min);
	getchar();
	*prtp_port_min = rtp_port_min;
	return 0;
}


static int atcom_config_eth_rtp_port_max(unsigned int *prtp_port_max)
{
	unsigned int rtp_port_max = 0;
	int ret 		= 0;
	printf("\n\t please input rtp_port_min value:\n");
	scanf("%uud",&rtp_port_max);
	getchar();
	*prtp_port_max = rtp_port_max;
	return 0;
}

static int atcom_config_eth_enable_tx(unsigned int *penable_tx)
{
	unsigned int enable_tx = 0;
	int ret 		= 0;
	printf("\n\t do you want enable vlan ?\n");
	ret  = get_enable_disable(&enable_tx);
	if( ret != 0){
		printf("Error: bridge_mode Confihure failed !\n");
		return -1;
	}
	*penable_tx = enable_tx;
	return 0;
}


static int atcom_config_eth_vlan_infos(atcom_switch_vlan * pinfo)
{
	printf("please configure vlan info:\n");
#if  0
	atcom_config_eth_bridge_mode(&pinfo->bridge_mode);
	atcom_config_eth_vlan_mode(&pinfo->vlan_enable);
	atcom_config_eth_vlan_check_mode(&pinfo->vlan_check_id_enable);
	atcom_config_eth_vlan_differentiated(&pinfo->vlan_differentiated);
	atcom_config_eth_diffserv(&pinfo->diffserv);
	atcom_config_eth_diffserv_value(&pinfo->diffserv_value);
	atcom_config_eth_voice_vid(&pinfo->voice_vid);
	atcom_config_eth_data_vid(&pinfo->data_vid);
	atcom_config_eth_voice_priority(&pinfo->voice_priority);
	atcom_config_eth_data_priority(&pinfo->data_priority);
	atcom_config_eth_sig_port(pinfo->sig_port,6);
	atcom_config_eth_rtp_port_min(&pinfo->rtp_port_min);
	atcom_config_eth_rtp_port_max(&pinfo->rtp_port_max);
	atcom_config_eth_enable_tx(&pinfo->enable_tx);
#else
	atcom_config_eth_bridge_mode(&pinfo->bridge_mode);
	atcom_config_eth_vlan_mode(&pinfo->vlan_enable);
	atcom_config_eth_voice_vid(&pinfo->voice_vid);
	atcom_config_eth_data_vid(&pinfo->data_vid);
	atcom_config_eth_voice_priority(&pinfo->voice_priority);
	atcom_config_eth_data_priority(&pinfo->data_priority);
	atcom_config_eth_rtp_port_min(&pinfo->rtp_port_min);
	atcom_config_eth_rtp_port_max(&pinfo->rtp_port_max);
#endif
	return 0;
}

int atcom_set_local_vlan_configure_for_net(atcom_1000M_switch_info_t * pinfo)
{
	int 	ret	= 0;
	int 	i 	= 0;
	struct ifreq  ifr;
	unsigned int	cmd = SIOCDEVPRIVATE+ATCOM_SET_CPU_PORT_VLAN_INFO;
	atcom_switch_vlan info;
	int fd = 0;

	if( NULL == pinfo){
		printf("param is NULL [%s:%d]!\n",__func__,__LINE__);
		return -1;
	}

	printf("=================%s:%d================\n",__func__,__LINE__);

	fd = pinfo->net_id;	
	memset(&info,0x00,sizeof(info));
	
	atcom_config_eth_vlan_infos(&info);

	memcpy(&(pinfo->net_info),&info,sizeof(info));

	memcpy(ifr.ifr_name, "eth0", 5);
	ifr.ifr_ifru.ifru_data = &info;

	ret  = ioctl(fd,cmd,&ifr);
	if( ret < 0 ){
		printf("Error: set local vlan configure failed !\n");
	}
	printf_vlan_infos("set:",info);

	return ret;
}

static int atcom_get_local_vlan_configure_for_net(atcom_1000M_switch_info_t * pinfo)
{
	
	int 	ret	= 0;
	int 	i 	= 0;
	atcom_switch_vlan info;
	struct ifreq  ifr;
	unsigned int 	cmd = SIOCDEVPRIVATE+ATCOM_GET_CPU_PORT_VLAN_INFO;
	int fd = 0;

	if( NULL == pinfo){
		printf("param is NULL [%s:%d]!\n",__func__,__LINE__);
		return -1;
	}

	fd = pinfo->net_id;
	
	printf("=================%s:%d================\n",__func__,__LINE__);

	memset(&info,0x00,sizeof(info));
	memcpy(ifr.ifr_name, "eth0", 5);
	ifr.ifr_ifru.ifru_data = &info;

	ret  = ioctl(fd,cmd,&ifr);
	if( ret < 0 ){
		printf("Error: Get local vlan configure failed !\n");
		return ret;
	}
	printf_vlan_infos("Get:",info);
	memcpy(&pinfo->net_info,&info,sizeof(info));
	return ret;
}




static int  atcom_configure_net_vlan_info(atcom_1000M_switch_info_t * pinfo)
{
	int 	ret  =0;
	char 	vlan_choose;
	int 	cfd	= 0;
	if( NULL == pinfo ){
		printf("Error: param is NULL [%s:%d]!\n");
		return -1;
	}

	cfd = pinfo->net_id;

	while(1){
		printf("\nplease choose which function you wanna do ? ");
		printf("\n\t 1) ====> set net switch vlan information");
		printf("\n\t 2) ====> get net switch vlan information");
		printf("\n\t Q|q) ====> exit\n");
		vlan_choose  = 0;
		scanf("%c",&vlan_choose);
		getchar();
		switch(vlan_choose)
		{
			case 'q':
			case 'Q':
				goto exit;
				break;
			case '1':
				ret = atcom_set_local_vlan_configure_for_net(pinfo);
				if( ret < 0 ){
					goto out;
				}
				break;
			case '2':
				ret = atcom_get_local_vlan_configure_for_net(pinfo);
				if( ret < 0 ){
					goto out;
				}
				break;	
			default:
				printf("Error: Choose invalue,please choose again !\n");
				break;
		}
	}

out:
	return -1;
exit:
	return 0;
}



static int atcom_switch_init_vlan(atcom_1000M_switch_info_t *pinfo)
{
	int ret 	= 0;
	int switch_fd		= 0;
	
	switch_fd = pinfo->switch_id;

	if (switch_fd < 0)
		return -1;
	vlan_aware_t vlaneflag;
	ret = ioctl(switch_fd, IFX_SET_VLAN_INIT, &vlaneflag);
	return ret;
}




int atcom_switch_set_vlan_awareness(BOOL benabled,atcom_1000M_switch_info_t *pinfo)
{
	int retval = -1;
	int switch_fd = pinfo->switch_id;
	
	if (switch_fd < 0)
		return -1;

	vlan_aware_t vlaneflag;
	vlaneflag.vlanawareflag = benabled ? 1 : 0;

	retval = ioctl(switch_fd, IFX_SET_VLAN_AWARE, &vlaneflag);

	return retval;
}


static int atcom_switch_usr_enable_valan_aware(atcom_1000M_switch_info_t *pinfo)
{
	int  ret  = 0;
	char enable = 0;
	BOOL vlan_enable;
	printf(" \n do you want enable switch vlan function:");
	ret = get_enable_disable(&vlan_enable);
	if( ret != 0 ){
		printf("Error: vlan enable failed !\n");
		return -1;
	}
	ret = atcom_switch_set_vlan_awareness(vlan_enable,pinfo); 	
	return ret;
}



int atcom_switch_get_port(rtk_port_t * port)
{	
	int i 		= 0;
	int index	= 0;
	while(1){
		printf("\n\nplease choose which port index:");
		for( i = 0; i < SIZEOF_ARRAY(port_index); i++){
			printf("\n\t [%d] ==> %s ",i,port_name[i]);
		}
		printf("\n");
		scanf("%d",&index);
		getchar();
		printf("index=%d\n",index);
	   if( index < 0 || index >  SIZEOF_ARRAY(port_index)){
		    printf("get port index failed !\n");
			continue;
		}else{
			*port = port_index[index];
			 break;
		}
	}
	return 0;
}

static int atcom_switch_get_vlan_id( int * const vlanid)
{
#define RTL8367C_EVIDMAX                    0x1FFF
	int vlan_id;
	while(1){
		printf("please input vlan id(0~%d) :",RTL8367C_EVIDMAX);
		scanf("%d",&vlan_id);
		getchar();	
		if( 0 > vlan_id || vlan_id > RTL8367C_EVIDMAX){
			printf("Error: vlan id is too bigger or too littler !\n");	
			continue;
		}else{
			*vlanid = vlan_id;
			break;
		}
	}
	return 0;
}
const char * atcom_switch_get_port_name(int port)
{
	int i 		= 0;
	int index 	= -1;
	for( i = 0 ; i < SIZEOF_ARRAY(port_index);i++ )
	{
		if( port == port_index[i] ){
			index = i;
			break;
		}
	}	

	return port_name[i];
}



void atcom_switch_printf_port_vlanId(port_vlan_t * const pinfo)
{
	int i = 0;
	if( 0 > pinfo->portid ){
		printf("Error: port id is wrong port_id=%d!\n",pinfo->portid,__func__,__LINE__);
	}

	for(  i = 0; i < SIZEOF_ARRAY(port_index);i++ ){
		if( pinfo->portid == port_index[i]){
			printf("port[%d]=%s, Vlan ID=%d \n",pinfo->portid,atcom_switch_get_port_name(pinfo->portid),pinfo->vlanid);
			return ;
		}
	}

	printf("Waring: port Id is too bigger or too lower [port=%d,vlan_ID=0x%x] (%s:%d)\n",pinfo->portid,pinfo->vlanid,__func__,__LINE__);

	return ;

}


static int atcom_switch_usr_set_port_vid(atcom_1000M_switch_info_t *pinfo)
{
	int 		 	ret 	= 0;
	port_vlan_t  	port_vlan_info;	
	rtk_port_t 	 	portid;
	int 	 	 	vlanid;
	int				i = 0;
	int 			switch_fd = pinfo->switch_id;
#if  1
		
	ret = atcom_switch_get_port(&portid);
	if( ret < 0 ){
		printf("Error:Set port for set port vlan failed !\n");
		return -1;
	}
	ret = atcom_switch_get_vlan_id(&vlanid);
	if( ret < 0){
		printf("Error:Set port for set port vlan failed !\n");
		return -1;
	}
	
	
	port_vlan_info.portid = portid;
	port_vlan_info.vlanid = vlanid;
	ret = ioctl(switch_fd, IFX_SET_PORT_VLANID, &port_vlan_info);
	if( ret < 0 ){
		printf("Error: run cmd IFX_SET_PORT_VLANID failed [%s;%d]! \n",__func__,__LINE__);
	}else{
		printf("port=%d\n",port_vlan_info.portid);
		atcom_switch_printf_port_vlanId(&port_vlan_info);
	}
#else
	
	for( i = 0; i < SIZEOF_ARRAY(port_index); i++){
			port_vlan_info.portid = port_index[i];
			port_vlan_info.vlanid = pinfo->net_info.data_vid;

			atcom_switch_printf_port_vlanId(&port_vlan_info);

			printf("port=%d\n",port_vlan_info.portid);

			ret = ioctl(switch_fd, IFX_SET_PORT_VLANID, &port_vlan_info);
			if( ret < 0 ){
				printf("Error: run cmd IFX_SET_PORT_VLANID failed [%s;%d]! \n",__func__,__LINE__);
			}else{
				printf("port=%d\n",port_vlan_info.portid);
				atcom_switch_printf_port_vlanId(&port_vlan_info);
			}
	}
#endif
	return ret;
}



static void atcom_switch_printf_port_cos_info(const port_cos_t * const port_cos_info)
{
	printf("Port COS info:\n");
	printf("\n\t port[%d] = %s \n",port_cos_info->portid,atcom_switch_get_port_name(port_cos_info->portid));
	printf("\n\t port cos is = 0x%x \n",port_cos_info->cos);
	return ;
}


static int atcom_switch_cos_inval(port_cos_t * const pinfo)
{
	int ret  = 0;
	if( pinfo->portid > PORT_CPU
		 || 0 > pinfo->portid
		 || pinfo->cos > 3 
		 || 0 > pinfo->cos )
	{
		return 1;
	}
	return 0;
}



static int atcom_switch_get_cos_for_port(int *const pvalue,int const port)
{
	int value = 0;
	while(1){
		printf("please input a cos value(0~3) for port[%d]=%s\n",port,atcom_switch_get_port_name(port));
		scanf("%d",&value);
		getchar();
		if( 0 > value || value > 3 ){
			printf(" cos value is too bigger or too small !\n");	
			continue;
		}else
		{
			*pvalue = value;
			break;
		}
	}
	return 0;
}
static int atcom_switch_init_prot_cos(port_cos_t * const port_cos_info)
{
	int ret 	= 0;
	int value	= 0;
	rtk_port_t portid	= 0;
	printf("please input cos for port: \n");

	ret = atcom_switch_get_port(&portid);
	if( ret < 0 ){
		printf("Error: get switch port index failed !\n");
		return -1;
	}
	 
	ret = atcom_switch_get_cos_for_port(&value,portid);
	if( ret < 0 ){
		printf("Error:get port cos value failed !\n");
		return -1;
	}

	port_cos_info->portid = portid;
	port_cos_info->cos  	 = value;

	if( atcom_switch_cos_inval(port_cos_info)){
		printf("param is inval, please input again !");
		return -1;
	}

	atcom_switch_printf_port_cos_info(port_cos_info);

	return 0;
}




static int atcom_switch_usr_set_port_cos(atcom_1000M_switch_info_t *pinfo)
{
	int ret  = 0;
	port_cos_t port_cos_info;
	int switch_fd	= pinfo->switch_id;
		
	memset(&port_cos_info,0x00,sizeof(port_cos_info));

	ret = atcom_switch_init_prot_cos(&port_cos_info);
	if( ret < 0 ){
		printf("Error:Get port vlan failed ! \n");
		return ret;
	}
	ret = ioctl(switch_fd, IFX_SET_PORT_COS, &port_cos_info);
	if( ret < 0 ){
		printf("Error: run cmd IFX_SET_VLAN_AWARE failed [%s;%d]! \n",__func__,__LINE__);
	}
	return ret ;
}

int atcom_switch_get_frame_type(rtk_vlan_acceptFrameType_t * const pfile)
{	
	int i 		= 0;
	int index	= 0;
	while(1){
		printf("\n\n please choose ingress frame type:");
		for( i = 0; i < SIZEOF_ARRAY(ingress_frame_type); i++){
			printf("\n\t [%d] ==> %s ",i,ingress_file_name[i]);
		}
		printf("\n");
		scanf("%d",&index);
		getchar();
	   if(index < 0 || index >  SIZEOF_ARRAY(ingress_frame_type)){
		    printf("get port index failed !\n");
			continue;
		}else{
			*pfile = ingress_frame_type[index];
			 break;
		}
	}
	return 0;
}

void atcom_switch_printf_ingress_file_type(port_ingressftype_t * const pinfo)
{
	printf(" port ingress file type:\n");
	printf("\t port[%d]=%s \n",pinfo->portid,atcom_switch_get_port_name(pinfo->portid));
	printf("\t ingress file type=%s \n",ingress_file_name[pinfo->ingressftype]);
	return;
}


static int atcom_switch_get_port_ingress_info_file(port_ingressftype_t * const pinfo)
{
	int ret  =  0;
	int i 	 =  0;
	rtk_port_t portid	= 0;
	unsigned char choose = 0;
	rtk_vlan_acceptFrameType_t file_type;

	
	printf("please intput port ingress info :\n");
	ret = atcom_switch_get_port(&portid);
	if( ret < 0 ){
		printf("Error: get port information failed !\n");
		return -1;
	}
	pinfo->portid = portid;
	ret = atcom_switch_get_frame_type(&file_type);
	if( ret < 0 ){
		printf("Error: get ingress file type failed !\n");
		return -1;
	}
	pinfo->ingressftype = 	file_type;		
	atcom_switch_printf_ingress_file_type(pinfo);
	return 0;
	
}


static int atcom_switch_usr_set_port_ingress(atcom_1000M_switch_info_t *pinfo)
{
	int ret 	= 0;
	port_ingressftype_t port_ingress_frame_info;
	int 	switch_fd	= pinfo->switch_id;

	memset(&port_ingress_frame_info,0x00,sizeof(port_ingress_frame_info));
	
	ret = atcom_switch_get_port_ingress_info_file(&port_ingress_frame_info);
	if( ret < 0 ){
		return ret;
	}

	ret = ioctl(switch_fd, IFX_SET_PORT_INGRESS_FRAME_TYPE, &port_ingress_frame_info);
	if( ret < 0 ){
		printf("Error: run cmd IFX_SET_PORT_INGRESS_FRAME_TYPE failed [%s;%d]! \n",__func__,__LINE__);
	}


	return ret;
}



static void atcom_switch_printf_port_ingress_fileter(port_ingressfilter_t* const pinfo)
{
	int ret  	= 0;
	int i 		= 0;

	printf("port[%d] ingress fileter info  is :\n",pinfo->portid);
	if( pinfo->portid < SIZEOF_ARRAY(port_index)
		&& pinfo->ingressfilter < SIZEOF_ARRAY(enbale_type)){
		for( i = 0; i < SIZEOF_ARRAY(port_index);i++){
			if( pinfo->portid == port_index[i] ){
				printf("prot[%d](%s) set ingress to \"%s\"\n",pinfo->portid,atcom_switch_get_port_name(pinfo->portid),enbale_type_name[pinfo->ingressfilter]);
				return ;
			}
		}
		printf("Error: Port=[%d], ingressfilter=%d is unaccess !\n",pinfo->portid,pinfo->ingressfilter);
	}else{
		printf("Error: Port=[%d], ingressfilter=%d is unaccess !\n",pinfo->portid,pinfo->ingressfilter);
	}
	return ;
}



static int  atcom_switch_set_port_ingress_fileter(atcom_1000M_switch_info_t *pinfo)
{
	int ret = 0;
	port_ingressfilter_t port_filter;
	rtk_port_t port	= 0;
	BOOL 	enbale ;
	int 	switch_fd = pinfo->switch_id;

	memset(&port_filter,0x00,sizeof(port_filter));

	ret = atcom_switch_get_port(&port);
	if( ret < 0 ){
		printf("Error: get port information failed !\n");
		return -1;
	}
	
	ret  = get_enable_disable(&enbale);
	if( ret < 0){
		printf("Error: choose disable or enable for port ingress fileter failed !\n");
		return ret;
	}
	
	port_filter.portid 			= port;
	port_filter.ingressfilter 	= enbale; 

	atcom_switch_printf_port_ingress_fileter(&port_filter);
	ret = ioctl(switch_fd, IFX_SET_PORT_INGRESS_FILETER, &port_filter);
	if( ret < 0 ){
		printf("Error: run cmd IFX_SET_PORT_INGRESS_FILETER failed [%s;%d]! \n",__func__,__LINE__);
	}
	
	return ret;
}

static int atcom_switch_get_port_vlanid(rtk_vlan_t * pvlanid)
{
	int   ret 	= 0;
	rtk_vlan_t vlanid 	= 0;
	while(1){
		printf("please input a vlanid(1~%d): \t",RTL8367C_EVIDMAX);
		scanf("%uud",&vlanid);
		getchar();
		if( 0 > vlanid || RTL8367C_EVIDMAX < vlanid ){
			printf("Error: vlan is is not in scope !\n");
			continue;
		}else{
			* pvlanid = vlanid;
			break;
		}
	}
	return 0;
}

static int atcom_switch_get_port_membermask(rtk_vlan_t vid,rtk_portmask_t * pmembermask)
{
	int   ret 	= 0;
	int   index  = 0;
	int   i 	 = 0;

	while(1){
		memset(pmembermask->bits,0x00,sizeof(pmembermask->bits));
		printf("\n please choose vlanid[%d]'s membermask:\n",vid);
		for( i = 0; i < SIZEOF_ARRAY(membermask_index); i++){
			printf("\n\t [%d] ==> %s ",i,membermask_name[i]);
		}
		printf("\n");
		scanf("%uud",&index);
		getchar();
		if( index < 0 || index >  SIZEOF_ARRAY(membermask_index)){
			 printf("choose membermark failed  !\n");
			 continue;
		 }else{
			  pmembermask->bits[0] = membermask_index[index];
			  break;
		 }
	}
	return 0;
}

static int atcom_switch_get_port_untagmask(rtk_vlan_t vid,rtk_portmask_t * puntagmask)
{
	int   ret	= 0;
	int   index  = 0;
	int   i 	 = 0;

	while(1){
		memset(puntagmask->bits,0x00,sizeof(puntagmask->bits));
		printf("\n please choose vlanid[%d]'s untagmask:\n",vid);
		for( i = 0; i < SIZEOF_ARRAY(membermask_index); i++){
			printf("\n\t [%d] ==> %s ",i,membermask_name[i]);
		}
		printf("\n");
		scanf("%uud",&index);
		getchar();
		if( index < 0 || index >  SIZEOF_ARRAY(membermask_index)){
			 printf("choose untagmask failed  !\n");
			 continue;
		 }else{
			  puntagmask->bits[0] = membermask_index[index];
			  break;
		 }
	}
	return 0;
}


static int atcom_switch_print_prot_vlan(port_vlan_entry_t * const pinfo)
{
	int ret = 0;
	printf("port vlan entry info: \n");
	printf("\n\t vlanid    : %d",pinfo->vlanid);
	printf("\n\t membermask: 0x%x",pinfo->vlan_cfg.mbr.bits[0]);
	printf("\n\t untagmask : 0x%x",pinfo->vlan_cfg.untag.bits[0]);
	printf("\n\t ivl_en : %s",enbale_type_name[!!pinfo->vlan_cfg.ivl_en]);
	printf("\n\t fid_msti : 0x%x",pinfo->vlan_cfg.fid_msti);
	printf("\n\t envlanpol : %s",enbale_type_name[!!pinfo->vlan_cfg.envlanpol]);
	printf("\n\t meteridx : 0x%x",pinfo->vlan_cfg.meteridx);
	printf("\n\t vbpen : %s",enbale_type_name[!!pinfo->vlan_cfg.vbpen]);
	printf("\n\t vbpri : 0x%x",pinfo->vlan_cfg.vbpri);
	printf("\n\n");
}

static int atcom_switch_get_prot_vlan(port_vlan_entry_t * pinfo)
{
	int ret 	= 0;
	int index 	= 0;	
	printf("please input  port vkan entry: \n");
	atcom_switch_get_port_vlanid(&pinfo->vlanid);	
	atcom_switch_get_port_membermask(pinfo->vlanid,&pinfo->vlan_cfg.mbr);	
	atcom_switch_get_port_untagmask(pinfo->vlanid,&pinfo->vlan_cfg.untag);	
#if  0
	atcom_get_port_ivl_en(pinfo->vlanid,&pinfo->ivl_en);	
	atcom_get_port_fid_msti(pinfo->vlanid,&pinfo->fid_msti);
	atcom_get_port_envlanpol(pinfo->vlanid,&pinfo->envlanpol);	
	atcom_get_port_meteridx(pinfo->vlanid,&pinfo->meteridx);	
	atcom_get_port_vbpen(pinfo->vlanid,&pinfo->vbpen);	
	atcom_get_port_vbpri(pinfo->vlanid,&pinfo->vbpri);	
#endif
	atcom_switch_print_prot_vlan(pinfo);
	return 0;	
}



static int atcom_switch_set_vlan_table(atcom_1000M_switch_info_t *pinfo)
{
	int ret  = 0;
	port_vlan_entry_t port_vlan_info;
	rtk_vlan_cfg_t vlan1;

	int 	switch_fd	= pinfo->switch_id;
	
	memset(&port_vlan_info,0x00,sizeof(port_vlan_info));		
	memset(&vlan1, 0x00, sizeof(rtk_vlan_cfg_t));	
	RTK_PORTMASK_PORT_SET(vlan1.mbr, PORT_CPU);
	RTK_PORTMASK_PORT_SET(vlan1.mbr, PORT_LAN);
	RTK_PORTMASK_PORT_SET(vlan1.mbr, PORT_PC);
	RTK_PORTMASK_PORT_SET(vlan1.untag, PORT_PC);    //remove vlan tags
	vlan1.ivl_en	= 1;
	memcpy(&port_vlan_info.vlan_cfg,&vlan1,sizeof(vlan1));
	port_vlan_info.vlanid = pinfo->net_info.voice_vid;
	
	ret = ioctl(switch_fd, IFX_SET_VLAN_TABLE_ENTRY, &port_vlan_info);
	if( ret < 0 ){
		printf("Error: run cmd IFX_SET_VLAN_TABLE_ENTRY failed [%s;%d]! \n",__func__,__LINE__);
	}else{
		atcom_switch_print_prot_vlan(&port_vlan_info);
	}
	return ret ;
}

static int atcom_switch_get_port_ability(rtk_port_phy_ability_t * pability)
{
	int 	ret 			= 0;
	int 	i				= 0;
	char 	index 			= 0;

	rtk_uint32    	AutoNegotiation = 0;  
	rtk_uint32    	Half_10			= 0; 		 
	rtk_uint32    	Full_10			= 0; 		 
	rtk_uint32    	Half_100		= 0;		 
	rtk_uint32    	Full_100		= 0;		 
	rtk_uint32    	Full_1000		= 0;		 
	rtk_uint32    	FC				= 0;				 
	rtk_uint32    	AsyFC			= 0;
	
	printf("please input all ablility for ethernet:\n");

	//set Autonegotiation
	while(1){
		printf("please choose autonegotition ability:\n");
		for( i = 0 ; i < SIZEOF_ARRAY(enbale_type);i++){
			printf("\n\t %d)===>%s",i,enbale_type_name[i]);
		}	
		printf("\n");
		scanf("%d",&index);
		getchar();
		if(  index > SIZEOF_ARRAY(enbale_type) && 0 > index ){
			printf("Error: autoNegotition get failed !\n");
			continue;
		}else{
			printf("AutoNegotiation is %s\n",enbale_type_name[index]);
			AutoNegotiation = enbale_type[index];
			break;
		}
	}
	
	//half_10M
	while(1){
		printf("please choose Half_10 ability:\n");
		for( i = 0 ; i < SIZEOF_ARRAY(enbale_type);i++){
			printf("\n\t %d)===>%s",i,enbale_type_name[i]);
		}	
		printf("\n");

		scanf("%d",&index);
		getchar();
		if(  index > SIZEOF_ARRAY(enbale_type) && 0 > index ){
			printf("Error: Half_10 get failed !\n");
			continue;
		}else{
			printf("Half_10 is %s\n",enbale_type_name[index]);
			Half_10 = enbale_type[index];
			break;
		}
	}
	//Full_10M
	while(1){
		printf("please choose Full_10 ability:\n");
		for( i = 0 ; i < SIZEOF_ARRAY(enbale_type);i++){
			printf("\n\t %d)===>%s",i,enbale_type_name[i]);
		}	
		printf("\n");

		scanf("%d",&index);
		getchar();
		if(  index > SIZEOF_ARRAY(enbale_type) && 0 > index ){
			printf("Error: Full_10 get failed !\n");
			continue;
		}else{
			printf("Full_10 is %s\n",enbale_type_name[index]);
			Full_10 = enbale_type[index];
			break;
		}
	}
	//Half_100M
	while(1){
		printf("please choose Half_100 ability:\n");
		for( i = 0 ; i < SIZEOF_ARRAY(enbale_type);i++){
			printf("\n\t %d)===>%s",i,enbale_type_name[i]);
		}	
		printf("\n");

		scanf("%d",&index);
		getchar();
		if(  index > SIZEOF_ARRAY(enbale_type) && 0 > index ){
			printf("Error: Half_100 get failed !\n");
			continue;
		}else{
			printf("Half_100 is %s\n",enbale_type_name[index]);
			Half_100 = enbale_type[index];
			break;
		}
	}
	//Full_100M
	while(1){
		printf("please choose Full_100 ability:\n");
		for( i = 0 ; i < SIZEOF_ARRAY(enbale_type);i++){
			printf("\n\t %d)===>%s",i,enbale_type_name[i]);
		}	
		printf("\n");

		scanf("%d",&index);
		getchar();
		if(  index > SIZEOF_ARRAY(enbale_type) && 0 > index ){
			printf("Error: Full_100 get failed !\n");
			continue;
		}else{
			printf("Full_100 is %s\n",enbale_type_name[index]);
			Full_100 = enbale_type[index];
			break;
		}
	}
	//Full_1000M
	while(1){
		printf("please choose Full_1000 ability:\n");
		for( i = 0 ; i < SIZEOF_ARRAY(enbale_type);i++){
			printf("\n\t %d)===>%s",i,enbale_type_name[i]);
		}	
		printf("\n");

		scanf("%d",&index);
		getchar();
		if(  index > SIZEOF_ARRAY(enbale_type) && 0 > index ){
			printf("Error: Full_1000 get failed !\n");
			continue;
		}else{
			printf("Full_1000 is %s\n",enbale_type_name[index]);
			Full_1000 = enbale_type[index];
			break;
		}
	}

	//FC
	while(1){
		printf("please choose  flow control FC ability:\n");
		for( i = 0 ; i < SIZEOF_ARRAY(enbale_type);i++){
			printf("\n\t %d)===>%s",i,enbale_type_name[i]);
		}	
		printf("\n");

		scanf("%d",&index);
		getchar();
		if(  index > SIZEOF_ARRAY(enbale_type) && 0 > index ){
			printf("Error:  flow control FC get failed !\n");
			continue;
		}else{
			printf("FC is %s\n",enbale_type_name[index]);
			FC = (rtk_uint32)enbale_type[index];
			break;
		}
	}

	//AsyFC
	while(1){
		printf("please choose asymmetric flow control AsyFC ability:\n");
		for( i = 0 ; i < SIZEOF_ARRAY(enbale_type);i++){
			printf("\n\t %d)===>%s",i,enbale_type_name[i]);
		}	
		printf("\n");

		scanf("%d",&index);
		getchar();
		if(  index > SIZEOF_ARRAY(enbale_type) && 0 > index ){
			printf("Error: asymmetric flow control AsyFC get failed !\n");
			continue;
		}else{
			printf("asymmetric flow control AsyFC is %s\n",enbale_type_name[index]);
			AsyFC = enbale_type[index];
			break;
		}
	}

	pability->AutoNegotiation  	= AutoNegotiation; 
	pability->Half_10		  	= Half_10;         
	pability->Full_10			= Full_10;         
	pability->Half_100			= Half_100;        
	pability->Full_100			= Full_100;        
	pability->Full_1000			= Full_1000;       
	pability->FC				= FC;              
	pability->AsyFC				= AsyFC;   
        
	return 0;	
}




static void atcom_switch_printf_port_ethernet_config(port_ethconfig_t * const pinfo)
{
	printf("Ethernet configure: \n");
	if( port_index[SIZEOF_ARRAY(port_index)-1] >= pinfo->portid  && 0 <=  pinfo->portid){
		printf("pinfo->port[%d]:%s",pinfo->portid,atcom_switch_get_port_name(pinfo->portid));
	}else{
		printf("Error:port = %d has misstake ! \n",pinfo->portid);
	}
    printf("\n\tAutoNegotiation is %s",pinfo->ability.AutoNegotiation == ENABLE?"Enable":"Disable"); 
    printf("\n\tHalf_10 is %s",pinfo->ability.Half_10==ENABLE?"Enable":"Disable");         
    printf("\n\tFull_10 is %s",pinfo->ability.Full_10==ENABLE?"Enable":"Disable");         
    printf("\n\tHalf_100 is %s",pinfo->ability.Half_100==ENABLE?"Enable":"Disable");        
    printf("\n\tFull_100 is %s",pinfo->ability.Full_100==ENABLE?"Enable":"Disable");        
    printf("\n\tFull_1000 is %s",pinfo->ability.Full_1000==ENABLE?"Enable":"Disable");       
    printf("\n\tFC is %s",pinfo->ability.FC==ENABLE?"Enable":"Disable");              
    printf("\n\tAsyFC is %s\n",pinfo->ability.AsyFC==ENABLE?"Enable":"Disable");           	
	return ;
}




static int atcom_switch_set_port_ethernet_config(atcom_1000M_switch_info_t *pinfo)
{
	int 		ret = 0;
	rtk_port_t port	= 0;
	port_ethconfig_t		port_phy_ability_info;
	rtk_port_phy_ability_t  port_ability;
	int 		switch_fd = pinfo->switch_id;

	memset(&port_phy_ability_info,0x00,sizeof(port_phy_ability_info));
	
	ret = atcom_switch_get_port(&port);
	if( ret < 0 ){
		printf("Error: get port failed !\n");
		return ret;
	}
	
	ret = atcom_switch_get_port_ability(&port_ability);
	if( ret < 0 ){
		printf("Error: get port ability failed !\n");
		return ret;
	}

	port_phy_ability_info.portid = port;
	memcpy(&port_phy_ability_info.ability,&port_ability,sizeof(port_ability));

	atcom_switch_printf_port_ethernet_config(&port_phy_ability_info);

	ret = ioctl(switch_fd, IFX_SET_PORT_ETHERNET_CONF, &port_phy_ability_info);
	if( ret < 0 ){
		printf("Error: run cmd IFX_SET_PORT_ETHERNET_CONF failed [%s;%d]! \n",__func__,__LINE__);
	}
	
	
	return ret;
}


static int atcom_reset_switch_vlan(atcom_1000M_switch_info_t *pinfo)
{
	int 		ret = 0;
	int 		reset = 0;
	int 		switch_fd = pinfo->switch_id;

	ret = ioctl(switch_fd, IFX_RESET_VLAN, &reset);
	if( ret != 0 ){
		printf("Error: run cmd IFX_RESET_VLAN failed  ret = %d [%s:%d]! \n",ret,__func__,__LINE__);
	}
	return ret;
}

static int atcom_switch_get_vlan_awareness(vlan_aware_t * pvlaneflag,atcom_1000M_switch_info_t *pinfo)
{
	int retval = -1;
	int switch_fd = pinfo->switch_id;
	
	if (switch_fd < 0)
		return -1;
	retval = ioctl(switch_fd, IFX_GET_VLAN_AWARE, pvlaneflag);
	return retval;
}




static  int atcom_switch_usr_get_valan_aware(atcom_1000M_switch_info_t *pinfo)
{
	int  ret  = 0;
	vlan_aware_t vlaneflag;
	memset(&vlaneflag,0x00,sizeof(vlaneflag));
	ret = atcom_switch_get_vlan_awareness(&vlaneflag,pinfo);
	if( ret ){
		return ret;
	}
	else
	{
		if( ENABLE == vlaneflag.vlanawareflag ){
			printf("switch has been [enable]  vlan \n");
		}else{
			printf("switch has been [disenable] vlan \n");
		}
	}
	return ret;
}


static int atcom_switch_usr_get_port_vid(atcom_1000M_switch_info_t *pinfo)
{
	int 		 	ret	= 0;
	port_vlan_t  	port_vlan_info;	
	rtk_port_t 		portid;
	int 		 	vlanid;
	int 			switch_fd = pinfo->switch_id;
		
	ret = atcom_switch_get_port(&portid);
	if( ret < 0 ){
		printf("Error:Set port for set port vlan failed !\n");
		return -1;
	}	
	port_vlan_info.portid = portid;
	port_vlan_info.vlanid = vlanid;

	ret = ioctl(switch_fd, IFX_GET_PORT_VLANID, &port_vlan_info);
	if( ret < 0 ){
		printf("Error: run cmd IFX_GET_PORT_VLANID failed [%s;%d]! \n",__func__,__LINE__);
	}else{
		atcom_switch_printf_port_vlanId(&port_vlan_info);
	}
	return ret;
}



static int atcom_switch_usr_get_port_cos(atcom_1000M_switch_info_t *pinfo)
{
	int ret  = 0;
	rtk_port_t port	= 0;
	port_cos_t port_cos_info;

	int 	switch_fd =pinfo->switch_id;

	memset(&port_cos_info,0x00,sizeof(port_cos_info));
	ret = atcom_switch_get_port(&port);
	if( ret < 0 ){
		printf("Error: Get port index failed !\n");
		return -1;
	}

	port_cos_info.portid = port;

	ret = ioctl(switch_fd, IFX_GET_PORT_COS, &port_cos_info);
	if( ret < 0 ){
		printf("Error: run cmd IFX_GET_PORT_COS failed [%s;%d]! \n",__func__,__LINE__);
	}else{
		atcom_switch_printf_port_cos_info(&port_cos_info);
	}
	return ret ;
}


static int atcom_switch_usr_get_port_ingress(atcom_1000M_switch_info_t *pinfo)
{
	int ret 	= 0;
	rtk_port_t port	= 0;
	port_ingressftype_t port_ingress_frame_info;

	int 	switch_fd = pinfo->switch_id;

	memset(&port_ingress_frame_info,0x00,sizeof(port_ingress_frame_info));
	
	ret = atcom_switch_get_port(&port);
	if( ret < 0 ){
		printf("Error: get port information failed !\n");
		return -1;
	}

	port_ingress_frame_info.portid = port;
	ret = ioctl(switch_fd, IFX_GET_PORT_INGRESS_FRAME_TYPE, &port_ingress_frame_info);
	if( ret < 0 ){
		printf("Error: run cmd IFX_GET_PORT_INGRESS_FRAME_TYPE failed [%s;%d]! \n",__func__,__LINE__);
	}

	atcom_switch_printf_ingress_file_type(&port_ingress_frame_info);

	return ret;
}




static int  atcom_switch_get_port_ingress_fileter(atcom_1000M_switch_info_t *pinfo)
{
	int ret = 0;
	port_ingressfilter_t port_filter;
	rtk_port_t port ;
	BOOL 	enbale ;
	int		switch_fd = pinfo->switch_id;

	memset(&port_filter,0x00,sizeof(port_filter));

	ret = atcom_switch_get_port(&port);
	if( ret < 0 ){
		printf("Error: get port information failed !\n");
		return -1;
	}

	ret = ioctl(switch_fd, IFX_GET_PORT_INGRESS_FILETER, &port_filter);
	if( ret < 0 ){
		printf("Error: run cmd IFX_GET_PORT_INGRESS_FILETER failed [%s;%d]! \n",__func__,__LINE__);
	}
	
	atcom_switch_printf_port_ingress_fileter(&port_filter);

	return ret ;
}


static int atcom_switch_get_vlan_table(atcom_1000M_switch_info_t *pinfo)
{
	int ret  = 0;
	rtk_vlan_t vlan_ID = 0;

	port_vlan_entry_t port_vlan_info;
	int 	switch_fd = pinfo->switch_id;


	memset(&port_vlan_info,0x00,sizeof(port_vlan_info));
	ret = atcom_switch_get_vlan_id(&vlan_ID);
	if( ret < 0){
		printf("Error: Set vlan id failed !\n");
		return -1;
	}
	port_vlan_info.vlanid = vlan_ID;

	ret = ioctl(switch_fd, IFX_GET_VLAN_TABLE_ENTRY, &port_vlan_info);
	if( ret < 0 ){
		printf("Error: run cmd IFX_SET_VLAN_AWARE failed [%s;%d]! \n",__func__,__LINE__);
	}
	else{
		atcom_switch_print_prot_vlan(&port_vlan_info);
	}
	return ret;
}

static int atcom_switch_get_port_ethernet_config(atcom_1000M_switch_info_t *pinfo)
{
	int 				ret = 0;
	rtk_port_t  		port	= 0;
	port_ethconfig_t	port_phy_ability_info;
	int 				switch_fd	= pinfo->switch_id;


	ret = atcom_switch_get_port(&port);
	if( ret < 0 ){
		printf("Error: get port failed !\n");
		return ret;
	}
	port_phy_ability_info.portid = port;
	port_phy_ability_info.ability.AutoNegotiation = ENABLE;

	ret = ioctl(switch_fd, IFX_GET_PORT_ETHERNET_CONF, &port_phy_ability_info);
	if( ret < 0 ){
		printf("Error: run cmd IFX_GET_PORT_ETHERNET_CONF failed [%s;%d]! \n",__func__,__LINE__);
	}else{
		atcom_switch_printf_port_ethernet_config(&port_phy_ability_info);
	}

	return ret;
}


int atcom_switch_get_port_phy_status(port_phystatus_t *plinkstatus,atcom_1000M_switch_info_t *pinfo)
{
	int 	retval 		= -1;
	int 	switch_fd	= pinfo->switch_id;

	if (switch_fd < 0){
		printf("Error:param is NULL [%s:%d]!\n",__func__,__LINE__);
		return -1;
	}

	retval = ioctl(switch_fd, IFX_GET_PORT_PHY_STATUS, plinkstatus);
	
	return retval;
}


void atcom_switch_printf_phy_status_info(port_phystatus_t * pinfo)
{
	char phy_info[1024]={0};

	if( NULL  == pinfo ){
		printf("Error: param is NULL [%s:%d] \n",__func__,__LINE__);
		return;
	}
	printf("\nport=%d,Duplex=%d,phystatus=%d,speed=%d\n",pinfo->portid,
				pinfo->Duplex,pinfo->phystatus,pinfo->Speed);
	sprintf(phy_info+strlen(phy_info),"port:%s ",atcom_switch_get_port_name(pinfo->portid));	

	switch(pinfo->Duplex){
		case PORT_HALF_DUPLEX:
			sprintf(phy_info+strlen(phy_info),"work mode:%s ","Half duplex"); 
			break;
		case PORT_FULL_DUPLEX:
			sprintf(phy_info+strlen(phy_info),"work mode:%s ","Full duplex"); 
			break;
		default:
			sprintf(phy_info+strlen(phy_info),"work mode:%s ","Unknow"); 
			break;

	}
	switch(pinfo->phystatus){	
		case PORT_LINKDOWN :
			sprintf(phy_info+strlen(phy_info),"link status:%s ","down"); 
			break;
		case PORT_LINKUP:
			sprintf(phy_info+strlen(phy_info),"link status:%s ","up"); 
			break;
		default:
			sprintf(phy_info+strlen(phy_info),"link status:%s ","Unknow"); 
			break;
	}

	switch(pinfo->Speed)
	{
		case PORT_SPEED_10M :
			sprintf(phy_info+strlen(phy_info),"speed:%s ","10M"); 
			break;
		case PORT_SPEED_100M:
			sprintf(phy_info+strlen(phy_info),"speed:%s ","100M"); 
			break;
		case PORT_SPEED_500M:
			sprintf(phy_info+strlen(phy_info),"speed:%s ","500M"); 
			break;
		case PORT_SPEED_1000M:
			sprintf(phy_info+strlen(phy_info),"speed:%s ","1000M"); 
			break;
		case PORT_SPEED_2500M:
			sprintf(phy_info+strlen(phy_info),"speed:%s ","2500M"); 
			break;
		default:
			sprintf(phy_info+strlen(phy_info),"speed:%s ","Unknow"); 
			break;
	}
	
	printf("%s\n",phy_info);
	return ;
}




int atocm_switch_get_phy_linkstatus(atcom_1000M_switch_info_t *pinfo)
{
	int ret 	   	= 0;
	rtk_port_t port	= 0;
	port_phystatus_t  port_phy_status ;
	memset(&port_phy_status,0x00,sizeof(port_phy_status));
	ret = atcom_switch_get_port(&port);
	if( ret < 0 ){
		printf("Error: Get port Id failed !\n");
		return -1;
	}

	port_phy_status.portid =port;

	ret = atcom_switch_get_port_phy_status(&port_phy_status,pinfo);
	if( ret < 0 ){
		printf("Error: get port phy status info failed");
		return ret;
	}else{
		atcom_switch_printf_phy_status_info(&port_phy_status);
	}
	return ret;	
}




static int atcom_set_switch_info(atcom_1000M_switch_info_t *pinfo)
{
	int ret = 0;
	int i = 0;
	port_vlan_t port_vlan;
	port_vlan_entry_t port_vlan_info;
	rtk_vlan_cfg_t vlan1;


	ret = atcom_switch_init_vlan(pinfo);
	if( ret < 0 ){
		printf("Error init switch vlan faild !\n",__func__,__LINE__);		
		return ret;
	}	

	for( i = 0; i <SIZEOF_ARRAY(port_index);i++){
		port_vlan.portid = port_index[i];
		port_vlan.vlanid = pinfo->net_info.voice_vid;
		ret = ioctl(pinfo->switch_id, IFX_SET_PORT_VLANID, &port_vlan);
		if( ret < 0 ){
			printf("Error: run cmd IFX_SET_PORT_VLANID failed [%s;%d]! \n",__func__,__LINE__);
		}else{
			printf("port=%d\n",port_vlan.portid);
			atcom_switch_printf_port_vlanId(&port_vlan);
		}		
	}	

	memset(&vlan1,0x00,sizeof(vlan1));
	memset(&port_vlan_info,0x00,sizeof(port_vlan_info));
	for( i = 0; i <SIZEOF_ARRAY(port_index);i++){
		RTK_PORTMASK_PORT_SET(vlan1.mbr, port_index[i]);
		RTK_PORTMASK_PORT_SET(vlan1.untag, port_index[i]);
	}

	vlan1.ivl_en = 1;
	memcpy(&port_vlan_info.vlan_cfg,&vlan1,sizeof(vlan1));
	port_vlan_info.vlanid = pinfo->net_info.voice_vid;
	ret = ioctl(pinfo->switch_id, IFX_SET_VLAN_TABLE_ENTRY, &port_vlan_info);
	if( ret < 0 ){
		printf("Error: run cmd IFX_SET_VLAN_TABLE_ENTRY failed [%s;%d]! \n",__func__,__LINE__);
	}else{
		atcom_switch_print_prot_vlan(&port_vlan_info);
	}
	return ret ;
}


static int atcom_test_switch_function(int cmd_index,atcom_1000M_switch_info_t *pinfo)
{
	int ret 	= 0;
	int cmd_do  = 0;
	
	if( NULL  == pinfo ){
		printf("Error: Param is NULL [%s:%d]!\n",__func__,__LINE__);
		return -1;
	}

	cmd_do = (int)(switch_cmd[cmd_index]);

	switch(cmd_do){
		case IFX_SET_VLAN_INIT:
			ret = atcom_switch_init_vlan(pinfo);
			break;
		case IFX_SET_VLAN_AWARE:
			ret = atcom_switch_usr_enable_valan_aware(pinfo); 	
			break;				
		case IFX_SET_PORT_VLANID:
			ret = atcom_switch_usr_set_port_vid(pinfo);	
			break;								
		case IFX_SET_PORT_COS:
			ret  = atcom_switch_usr_set_port_cos(pinfo);
			break;									
		case IFX_SET_PORT_INGRESS_FRAME_TYPE:
			ret  = atcom_switch_usr_set_port_ingress(pinfo);
			break;					
		case IFX_SET_PORT_INGRESS_FILETER:
			ret  = atcom_switch_set_port_ingress_fileter(pinfo);
			break;					   
		case IFX_SET_VLAN_TABLE_ENTRY:	
			ret  = atcom_switch_set_vlan_table(pinfo);
			break;					   
		case IFX_SET_PORT_ETHERNET_CONF: 
			ret = atcom_switch_set_port_ethernet_config(pinfo);
			break;	
		case IFX_RESET_VLAN:
			ret = atcom_reset_switch_vlan(pinfo);
			break;		   
#if 1
		case IFX_GET_VLAN_AWARE: 
			ret = atcom_switch_usr_get_valan_aware(pinfo);
			break;									
		case IFX_GET_PORT_VLANID:
			ret = atcom_switch_usr_get_port_vid(pinfo);
			break;								
		case IFX_GET_PORT_COS:	
			ret = atcom_switch_usr_get_port_cos(pinfo);
			break;									
		case IFX_GET_PORT_INGRESS_FRAME_TYPE:	
			ret = atcom_switch_usr_get_port_ingress(pinfo);
			break;				
		case IFX_GET_PORT_INGRESS_FILETER:	
			ret = atcom_switch_get_port_ingress_fileter(pinfo); 
			break;				
		case IFX_GET_VLAN_TABLE_ENTRY:	
			ret  = atcom_switch_get_vlan_table(pinfo);	   
			break;				
		case IFX_GET_PORT_ETHERNET_CONF: 
			ret = atcom_switch_get_port_ethernet_config(pinfo);   
			break;		
		case IFX_GET_PORT_PHY_STATUS:
			ret =atocm_switch_get_phy_linkstatus(pinfo);
			break;
		case IFX_GET_PORT_PHY_STATUS+1:
			ret = atcom_set_switch_info(pinfo);
			break;
#endif						   
		default:
			ret = -1;
			break;
	}
	return ret;
}


static void atcom_printf_failed_cmd(int cmd_index,int ret)
{
	if(cmd_index < SIZEOF_ARRAY(switch_cmd) && cmd_index >= 0 ){
		printf("Error: run cmd[%s] failed ret=%d !\n",switch_cmd_name[cmd_index],ret);
	}
	return ;
}



int atcom_switch_test_choose_cmd(void)
{
	int  i = 0;
	int  index =0 ;
	printf("\n please choose which command you want to run:");

	for( i = 0; i <SIZEOF_ARRAY(switch_cmd);i++){
		if( 8 == i ){
			printf("\n");
		}
		printf("\n\t[%d] ==> %s",i,switch_cmd_name[i]);
	}
	printf("\n");
	scanf("%d",&index);
	getchar();
	return index;	
}




static int atcom_configure_switch_vlan_info(atcom_1000M_switch_info_t *pinfo)
{
	int ret  = 0;
	int cmd_index;
	int max_cmd_index;
	printf("\n============9919 switch configure========\n");

	max_cmd_index = SIZEOF_ARRAY(switch_cmd)-1;

	while(1){
		 cmd_index = atcom_switch_test_choose_cmd(); 	
		if( cmd_index >= max_cmd_index || cmd_index < 0 ){
			if( cmd_index == max_cmd_index ){
				goto test_switch_failed;
			}else{
				printf("Switch is not support this cmd !\n");
			}
		}else{
			printf("\nyou choose [%d] ==> %s\n",cmd_index,switch_cmd_name[cmd_index]);
		}
		ret = atcom_test_switch_function(cmd_index,pinfo);
		if(ret <0 ){
			atcom_printf_failed_cmd(cmd_index,ret);
			goto test_switch_failed;
		}
	}
			
	return 0;
test_switch_failed:
	return ret;
}






int main(int argc,char ** argv)
{
	int ret	= 0;
	unsigned char choose=0;

	atcom_1000M_switch_info_t switch_info;
	memset(&switch_info,0x00,sizeof(switch_info));
	
	ret = init_fd_secriptinos(&switch_info);	
	if( ret < 0 ){
		printf("init vlan test fds failed [%s:%d]!\n",__func__,__LINE__);
		return -1;
	}
	while(1){
		ret = atcom_configure_net_vlan_info(&switch_info);
		if( ret < 0 ){
			printf("atcom_configure_net_vlan_info failed [%s:%d]!\n",__func__,__LINE__);
			goto error;
		}

		
		ret = atcom_configure_switch_vlan_info(&switch_info);
		if( ret < 0 ){
			printf("atcom_configure_switch_vlan_info failed [%s:%d]!\n",__func__,__LINE__);
			goto error;
		}
		printf("\ndo you want try set all again ? yes[Y|y] no[N|n] \n");	
		scanf("%c",&choose);
		getchar();
		if(choose=='y'||choose=='Y'){
			continue;
		}else{
			atcom_set_switch_info(&switch_info);
			break;
		}
	}
	return 0;

error:
	uninit_fd_secriptinos(&switch_info);
	return -1;
}




