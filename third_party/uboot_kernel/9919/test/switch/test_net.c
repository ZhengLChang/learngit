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

#if 0
 struct ifreq ifr;
 
  ifr.ifr_ifindex = 2; //获得第2个网络接口的名称 
  err = ioctl(s, SIOCGIFNAME, &ifr);
 
  if (err)
    {
      perror("index error");
    }
 else
   {

     printf("the %dst interface is:%s\n", ifr.ifr_ifindex, ifr.ifr_name);

   }
//传入网络接口名称，获得标志
 memcpy(ifr.ifr_name, "eth0", 5);
 err = ioctl(s, SIOCGIFFLAGS, &ifr);
 if (!err)
   {
     printf("SIOCGIFFLAGS:%d\n", ifr.ifr_flags);
   }
//获得MTU和MAC
   err = ioctl(s, SIOCGIFMTU, &ifr);
#endif

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


void printf_vlan_info(char * pstr,atcom_switch_vlan info)
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
		if( 0 > index  || SIZEOF_ARRAY(enbale_type) < index ){
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


static int atcom_config_eth_vlan_info(atcom_switch_vlan * pinfo)
{
	printf("please configure vlan info:\n");
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
	return 0;
}

int atcom_set_local_vlan_configure(int fd)
{
	int 	ret	= 0;
	int 	i 	= 0;
		unsigned int 	cmd = SIOCDEVPRIVATE+ATCOM_SET_CPU_PORT_VLAN_INFO;
	struct ifreq  ifr;
	atcom_switch_vlan info;
	printf("=================%s:%d================\n",__func__,__LINE__);
	memset(&info,0x00,sizeof(info));

	atcom_config_eth_vlan_info(&info);

	memcpy(ifr.ifr_name, "eth0", 5);
	ifr.ifr_ifru.ifru_data = &info;

	ret  = ioctl(fd,cmd,&ifr);
	if( ret < 0 ){
		printf("Error: set local vlan configure failed !\n");
	}
	printf_vlan_info("set:",info);

	return ret;
}


int atcom_get_local_vlan_configure(int fd)
{
	
	int 	ret	= 0;
	int 	i 	= 0;
	atcom_switch_vlan info;
	struct ifreq  ifr;
	unsigned int 	cmd = SIOCDEVPRIVATE+ATCOM_GET_CPU_PORT_VLAN_INFO;
	printf("=================%s:%d================\n",__func__,__LINE__);

	memset(&info,0x00,sizeof(info));
	memcpy(ifr.ifr_name, "eth0", 5);
	ifr.ifr_ifru.ifru_data = &info;

	ret  = ioctl(fd,cmd,&ifr);
	if( ret < 0 ){
		printf("Error: Get local vlan configure failed !\n");
		return ret;
	}
	printf_vlan_info("Get:",info);
	return ret;
}


int main(void)
{
	int 		cfd;
	char 		vlan_choose;
	int 		ret 	= 0;

	struct sockaddr_in s_add,c_add;

	printf("Hello,welcome to client !\r\n");

	cfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == cfd)
	{
	    printf("socket fail ! \r\n");
	    return -1;
	}
	printf("socket ok !\r\n");
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
				goto out;
				break;
			case '1':
				ret = atcom_set_local_vlan_configure(cfd);
				if( ret < 0 ){
					goto out;
				}
				break;
			case '2':
				ret = atcom_get_local_vlan_configure(cfd);
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
	close(cfd);
	return ret;
}





