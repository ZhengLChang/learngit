#ifndef _ATCOM_SWITCH_IOCTL_H
#define _ATCOM_SWITCH_IOCTL_H
#include "rtk_switch.h"
#include "rtk_error.h"
#include "vlan.h"
#include "port.h"
#include "acl.h"
typedef unsigned int          BOOL;


typedef struct vlan_aware{
	BOOL vlanawareflag;
}vlan_aware_t;

typedef struct port_vlan{
	rtk_port_t portid;
	rtk_vlan_t vlanid;
}port_vlan_t;

typedef struct port_cos{
	rtk_port_t portid;
	rtk_pri_t  cos;
}port_cos_t;

typedef struct port_ingressftype{
	rtk_port_t                 portid;
	rtk_vlan_acceptFrameType_t ingressftype;
}port_ingressftype_t;

typedef struct port_ingressfilter{
	rtk_port_t portid;
	BOOL       ingressfilter;
}port_ingressfilter_t;

typedef struct port_vlantableentry{
	rtk_vlan_t     vlanid;
	rtk_vlan_cfg_t vlan_cfg;
}port_vlan_entry_t;


typedef struct port_phystatus{
	rtk_port_t             portid;
	rtk_port_linkStatus_t  phystatus;
	rtk_port_speed_t 	  Speed;
	rtk_port_duplex_t     Duplex;
}port_phystatus_t;

typedef struct port_ethconfig{
	rtk_port_t             portid;
	rtk_port_phy_ability_t ability;
}port_ethconfig_t;

typedef enum filter_subcmd{
	IGRACL_INIT=0,
	IGRACL_FIELD_ADD,
	IGRACL_CFG_ADD,
	IGRACL_CFG_DEL,
	IGRACL_CFG_DELALL,
	IGRACL_CFG_GET,
	IGRACL_UNMATCHACTION_SET,
	IGRACL_UNMATCHACTION_GET,
	IGRACL_STATE_SET,
	IGRACL_STATE_GET,
	IGRACL_TEMPLATE_SET,
	IGRACL_TEMPLATE_GET,
	IGRACL_FIELD_SEL_SET,
	IGRACL_FIELD_SEL_GET,	
	IPRANGE_SET,
	IPRANGE_GET,
	VIDRANGE_SET,
	VIDRANGE_GET,
	PORTRANGE_SET,
	PORTRANGE_GET,
	IGRACLPOLARITY_SET,
	IGRACLPOLARITY_GET,
}filter_subcmd_t,*pfilter_subcmd;


typedef struct atcom_filter_filed_add{
	rtk_filter_cfg_t	Filter_cfg;
	rtk_filter_field_t	Filter_field;
}atcom_filter_filed_add_t,*patcom_filter_filed_add_t;

typedef struct atcom_filter_cfg_add{
	rtk_filter_id_t filter_id;
	rtk_filter_cfg_t Filter_cfg;
 	rtk_filter_action_t Filter_action;
	rtk_filter_number_t ruleNum;
}atcom_filter_cfg_add_t,*patcom_filter_cfg_add_t;

typedef struct atcom_filter_cfg_del{
	rtk_filter_id_t filter_id;
}atcom_filter_cfg_del_t,*patcom_filter_cfg_del_t;


typedef struct atcom_filter_cfg_get_info{
	rtk_filter_id_t 		filter_id;
	rtk_filter_cfg_raw_t 	Filter_cfg;
	rtk_filter_action_t 	Action;
}atcom_filter_cfg_get_info_t,*patcom_filter_cfg_get_info_t;

typedef struct atcom_filter_unmatchAction{
	rtk_port_t port;
	rtk_filter_unmatch_action_t action;
}atcom_filter_unmatchAction_t,*patcom_filter_unmatchAction_t;


typedef struct atcom_filter_state_info{
	rtk_port_t port;
	rtk_filter_state_t state;
}atcom_filter_state_info_t,*patcom_filter_state_info_t;


typedef struct atcom_filter_template_info{
	rtk_filter_template_t  aclTemplate;
}atcom_filter_template_info_t,*patcom_filter_template_info_t;

typedef struct field_sel_set{
	rtk_uint32 		index;
	rtk_field_sel_t format;
	rtk_uint32 		offset;
}atcom_filter_field_sel_info_t,*patcom_filter_field_sel_info_t;


typedef struct  iprange_info{
	rtk_uint32 				index;
	rtk_filter_iprange_t 	type;
	ipaddr_t 				upperIp;
	ipaddr_t 				lowerIp;
}atcom_filter_iprange_info_t,*patcom_filter_iprange_info_t;

typedef struct portrange_info{
	rtk_uint32 				index;
	rtk_filter_portrange_t 	type;
	rtk_uint32 				upperPort;
	rtk_uint32 				lowerPort;
}atcom_portrange_info_t,*patcom_portrange_info_t;

typedef struct vidrange_info{
	rtk_uint32 				index;
	rtk_filter_vidrange_t  	Type;
	rtk_uint32  			UpperVid;
	rtk_uint32 				LowerVid;
}atcom_vidrange_info_t,*patcom_vidrange_info_t;

typedef struct polarity{
	rtk_uint32 polarity;
}atcom_polarity_info_t,*patcom_polarity_info_t;

typedef struct atcom_switch_filter_data{
	filter_subcmd_t subcmd;
	union{
		atcom_filter_filed_add_t  		filter_filed_add_info;
		atcom_filter_cfg_add_t	  		filter_cfg_add_info;
		atcom_filter_cfg_del_t	  		filter_cfg_del_info;
		atcom_filter_cfg_get_info_t 	filter_cfg_get_info;
		atcom_filter_unmatchAction_t	filter_unmatchaction_info;
		atcom_filter_state_info_t		filter_state_info;
		atcom_filter_template_info_t	filter_template_info;
		atcom_filter_field_sel_info_t	field_sel_info;
		atcom_filter_iprange_info_t		iprange_set_info;
		atcom_vidrange_info_t			vidrange_info;
		atcom_portrange_info_t			portrange_info;
		atcom_polarity_info_t			polarity_info;
	}filter_info;
}atcom_switch_filter_data_t,*patcom_switch_filter_data_t;

//C0 ~DF
#define ATCOM_SWITCH_IOW(num, dtype)         _IOW('p', num, dtype)
#define ATCOM_SWITCH_IOR(num, dtype)         _IOR('p', num, dtype)

/*ioctl commands define*/
/*ioctl commands define*/
#define IFX_SET_VLAN_AWARE     				ATCOM_SWITCH_IOW(0XC0,vlan_aware_t)	
#define IFX_SET_PORT_VLANID    				ATCOM_SWITCH_IOW(0XC1,port_vlan_t)				
#define IFX_SET_PORT_COS       				ATCOM_SWITCH_IOW(0XC2,port_cos_t)				
#define IFX_SET_PORT_INGRESS_FRAME_TYPE 	ATCOM_SWITCH_IOW(0XC3,port_ingressftype_t)		
#define IFX_SET_PORT_INGRESS_FILETER        ATCOM_SWITCH_IOW(0XC4,port_ingressfilter_t)		
#define IFX_SET_VLAN_TABLE_ENTRY            ATCOM_SWITCH_IOW(0XC5,port_vlan_entry_t) 		
#define IFX_SET_PORT_ETHERNET_CONF          ATCOM_SWITCH_IOW(0XC6,port_ethconfig_t)			

#define IFX_GET_VLAN_AWARE     				ATCOM_SWITCH_IOR(0xD0,vlan_aware_t)				
#define IFX_GET_PORT_VLANID    				ATCOM_SWITCH_IOR(0xD1,port_vlan_t)				
#define IFX_GET_PORT_COS       				ATCOM_SWITCH_IOR(0xD2,port_cos_t)				
#define IFX_GET_PORT_INGRESS_FRAME_TYPE 	ATCOM_SWITCH_IOR(0xD3,port_ingressftype_t)		
#define IFX_GET_PORT_INGRESS_FILETER        ATCOM_SWITCH_IOR(0xD4,port_ingressfilter_t)		
#define IFX_GET_VLAN_TABLE_ENTRY            ATCOM_SWITCH_IOR(0xD5,port_vlan_entry_t)		
#define IFX_GET_PORT_ETHERNET_CONF          ATCOM_SWITCH_IOR(0xD6,port_ethconfig_t)			
#define IFX_GET_PORT_PHY_STATUS             ATCOM_SWITCH_IOR(0xD7,port_phystatus_t)	


#define RRK_DERICT_ACL_CTL             		ATCOM_SWITCH_IOW(0xE0,unsigned long)
#define IFX_SET_VLAN_INIT     				ATCOM_SWITCH_IOW(0XF0,int)	
#define IFX_RESET_VLAN    				    ATCOM_SWITCH_IOW(0XF1,int)	
#endif



