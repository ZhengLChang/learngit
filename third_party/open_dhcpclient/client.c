/*
 * dhcpcd - DHCP client daemon -
 * Copyright (C) 1996 - 1997 Yoichi Hariguchi <yoichi@fore.com>
 * Copyright (C) January, 1998 Sergei Viznyuk <sv@phystech.com>
 * 
 * dhcpcd is an RFC2131 and RFC1541 compliant DHCP client daemon.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <setjmp.h>
#include <resolv.h>
#include <time.h>
#include "generaldefs.h"
#include "client.h"
#include "buildmsg.h"
#include "udpipgen.h"
#include "pathnames.h"



#ifdef CONFIG_LEDMAN
#include <linux/ledman.h>
#endif

extern	char		*ProgramName,**ProgramEnviron,*Cfilename;
extern	char		*IfName;
extern	int		IfName_len;
extern	char		*HostName;
extern	unsigned char	*ClassID;
extern	int		ClassID_len;
extern  unsigned char	*ClientID;
extern  int		ClientID_len;
extern	int		BeRFC1541;
extern	unsigned	LeaseTime;
extern	int		ReplResolvConf;
extern	int		SetDomainName;
extern	int		SetHostName;
extern	unsigned short	ip_id;
extern  void		*(*currState)();
extern  time_t          TimeOut;
extern  unsigned        nleaseTime;
extern  struct in_addr  inform_ipaddr;
extern	int		DoCheckSum;
extern	int		TestCase;
extern  int		Window;
extern	int		dhcpRequestMax;
extern	int		FiniteLeaseOnly;
#ifdef CONFIG_LEDMAN
extern	int		ledman_led;
#endif


#ifdef ARPCHECK
int arpCheck();
#endif
int arpRelease();
int arpInform();

int		dhcpSocket;
int             prev_ip_addr;
time_t		ReqSentTime;
dhcpOptions	DhcpOptions;
dhcpInterface   DhcpIface;
udpipMessage	UdpIpMsgSend,UdpIpMsgRecv;
jmp_buf		env;
unsigned char	ClientHwAddr[ETH_ALEN];

const struct ip *ipSend=(struct ip *)((struct udpiphdr *)UdpIpMsgSend.udpipmsg)->ip;
const struct udphdr *udpSend=(struct udphdr *)((char *)(((struct udpiphdr *)UdpIpMsgSend.udpipmsg)->ip)+sizeof(struct ip));
const struct ip *ipRecv=(struct ip *)((struct udpiphdr *)UdpIpMsgRecv.udpipmsg)->ip;
const struct udphdr *udpRecv=(struct udphdr *)((char *)(((struct udpiphdr *)UdpIpMsgRecv.udpipmsg)->ip)+sizeof(struct ip));
const dhcpMessage *DhcpMsgSend = (dhcpMessage *)&UdpIpMsgSend.udpipmsg[sizeof(udpiphdr)];
      dhcpMessage *DhcpMsgRecv = (dhcpMessage *)&UdpIpMsgRecv.udpipmsg[sizeof(udpiphdr)];

static int readIp(const char* ip)
{
   int n = 0;
   int res = 0;
   
   while (n < 4 && *ip)
   {
      if (isdigit(*ip)) 
      {
         res = (res << 8) | atoi(ip);
         n++;
         while (isdigit(*ip)) 
         {
            ip++;
         }
      } 
      else 
      {
         ip++;
      }	
   }
   return res;
}

static int udpSendto(short port, void *data, int len)
{
  int sockfd;
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;

  /* fill in server address */
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(readIp("127.0.0.1"));
  serv_addr.sin_port        = htons(port);

  /* open udp socket */
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    return -1; /* could not open socket */
  }

  /* bind any local address for us */ 
  memset(&cli_addr, 0, sizeof(cli_addr));
  cli_addr.sin_family      = AF_INET;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  cli_addr.sin_port        = htons(0);

  if (bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
    return -2; /* could not bind client socket */
  }
 
  /* send the data */
  if (sendto(sockfd, data, len, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != len) {
    return -3; /* could not sendto */
  }
  close(sockfd);
  return 0;
}


int sendDhcpMonitorMsg(int code, char *ipaddress, char *opt66, char *opt67)
{
    char buf[sizeof(msgHeader) + sizeof(dhcpMonitorMsg)] = {0};
    msgHeader *msg = (msgHeader*)buf;
    dhcpMonitorMsg *dhcpBody = (dhcpMonitorMsg *)(msg+1);
    
     msg->type = code;

	if (code == NET_MSG_DHCP_IP_CONFLICT)
	{
		msg->datalength = sizeof(dhcpMonitorMsg);
		if (ipaddress)
			strcpy(dhcpBody->ipaddress, ipaddress);
	}
	 		
	if (code == NET_MSG_DHCP_PARSE_OPT6667)
	{
		msg->datalength = sizeof(dhcpMonitorMsg);
		if (opt66)
			strcpy(dhcpBody->opt66, opt66);
		if (opt67)
			strcpy(dhcpBody->opt67, opt67);
	}

	return udpSendto(NET_MONITOR_PORT, (void*)msg, sizeof(msgHeader) + sizeof(dhcpMonitorMsg));
		
}

/*****************************************************************************/
int parseDhcpMsgRecv() /* this routine parses dhcp message received */
{
#ifdef DEBUG
  int i,j;
#endif
  register u_char *p = DhcpMsgRecv->options+4;
  unsigned char *end = DhcpMsgRecv->options+sizeof(DhcpMsgRecv->options);
  while ( p < end )
    switch ( *p )
      {
        case endOption: goto swend;
       	case padOption: p++; break;
       	default:
          /* Handle a bug in the redhat 6.2 bootp server
           * which causes it to incorrectly pad the end option */
	  if ( ((p - DhcpMsgRecv->options)&1) && (p[1] == endOption) )
	    goto swend;
	  if ( p[1] )
	    {
	      if ( DhcpOptions.len[*p] == p[1] )
	        memcpy(DhcpOptions.val[*p],p+2,p[1]);
	      else
	        {
		  DhcpOptions.len[*p] = p[1];
	          if ( DhcpOptions.val[*p] )
	            free(DhcpOptions.val[*p]);
	      	  else
		    DhcpOptions.num++;
	      	  DhcpOptions.val[*p] = malloc(p[1]+1);
		  memset(DhcpOptions.val[*p],0,p[1]+1);
	  	  memcpy(DhcpOptions.val[*p],p+2,p[1]);
	        }
	    }
	  p+=p[1]+2;
      }
swend:
#ifdef DEBUG
  fprintf(stderr,"parseDhcpMsgRecv: %d options received:\n",DhcpOptions.num);
  for (i=1;i<255;i++)
    if ( DhcpOptions.val[i] )
      switch ( i )
        {
	  case 1: /* subnet mask */
	  case 3: /* routers on subnet */
	  case 4: /* time servers */
	  case 5: /* name servers */
	  case 6: /* dns servers */
	  case 28:/* broadcast addr */
	  case 33:/* staticRoute */
	  case 41:/* NIS servers */
	  case 42:/* NTP servers */
	  case 50:/* dhcpRequestdIPaddr */
	  case 54:/* dhcpServerIdentifier */
	    for (j=0;j<DhcpOptions.len[i];j+=4)
	      fprintf(stderr,"i=%-2d  len=%-2d  option = %u.%u.%u.%u\n",
		i,DhcpOptions.len[i],
		((unsigned char *)DhcpOptions.val[i])[0+j],
		((unsigned char *)DhcpOptions.val[i])[1+j],
		((unsigned char *)DhcpOptions.val[i])[2+j],
		((unsigned char *)DhcpOptions.val[i])[3+j]);
	    break;
	  case 2: /* time offset */
	  case 51:/* dhcpAddrLeaseTime */
	  case 57:/* dhcpMaxMsgSize */
	  case 58:/* dhcpT1value */
	  case 59:/* dhcpT2value */
	    fprintf(stderr,"i=%-2d  len=%-2d  option = %d\n",
		i,DhcpOptions.len[i],
		    ntohl(*(int *)DhcpOptions.val[i]));
	    break;
	  case 23:/* defaultIPTTL */
	  case 29:/* performMaskdiscovery */
	  case 31:/* performRouterdiscovery */
	  case 53:/* dhcpMessageType */
	    fprintf(stderr,"i=%-2d  len=%-2d  option = %u\n",
		i,DhcpOptions.len[i],*(unsigned char *)DhcpOptions.val[i]);
	    break;
	  default:
	    fprintf(stderr,"i=%-2d  len=%-2d  option = \"%s\"\n",
		i,DhcpOptions.len[i],(char *)DhcpOptions.val[i]);
	}
fprintf(stderr,"\
DhcpMsgRecv->yiaddr  = %u.%u.%u.%u\n\
DhcpMsgRecv->siaddr  = %u.%u.%u.%u\n\
DhcpMsgRecv->giaddr  = %u.%u.%u.%u\n\
DhcpMsgRecv->sname   = \"%s\"\n\
ServerHardwareAddr   = %02X.%02X.%02X.%02X.%02X.%02X\n",
((unsigned char *)&DhcpMsgRecv->yiaddr)[0],
((unsigned char *)&DhcpMsgRecv->yiaddr)[1],
((unsigned char *)&DhcpMsgRecv->yiaddr)[2],
((unsigned char *)&DhcpMsgRecv->yiaddr)[3],
((unsigned char *)&DhcpMsgRecv->siaddr)[0],
((unsigned char *)&DhcpMsgRecv->siaddr)[1],
((unsigned char *)&DhcpMsgRecv->siaddr)[2],
((unsigned char *)&DhcpMsgRecv->siaddr)[3],
((unsigned char *)&DhcpMsgRecv->giaddr)[0],
((unsigned char *)&DhcpMsgRecv->giaddr)[1],
((unsigned char *)&DhcpMsgRecv->giaddr)[2],
((unsigned char *)&DhcpMsgRecv->giaddr)[3],
DhcpMsgRecv->sname,
UdpIpMsgRecv.ethhdr.ether_shost[0],
UdpIpMsgRecv.ethhdr.ether_shost[1],
UdpIpMsgRecv.ethhdr.ether_shost[2],
UdpIpMsgRecv.ethhdr.ether_shost[3],
UdpIpMsgRecv.ethhdr.ether_shost[4],
UdpIpMsgRecv.ethhdr.ether_shost[5]);
#endif
	/*wyj*/
	if (DhcpOptions.len[66])
	{
		sendDhcpMonitorMsg(NET_MSG_DHCP_PARSE_OPT6667, NULL, (char *)DhcpOptions.val[66], (char *)DhcpOptions.val[67]);
	}
  if ( ! DhcpMsgRecv->yiaddr ) DhcpMsgRecv->yiaddr=DhcpMsgSend->ciaddr;
  if ( ! DhcpOptions.val[dhcpServerIdentifier] ) /* did not get dhcpServerIdentifier */
    {	/* make it the same as IP address of the sender */
      DhcpOptions.val[dhcpServerIdentifier] = malloc(4);
      memcpy(DhcpOptions.val[dhcpServerIdentifier],&ipRecv->ip_src.s_addr,4);
      DhcpOptions.len[dhcpServerIdentifier] = 4;
      DhcpOptions.num++;
	DebugSyslog(LOG_DEBUG,
	"dhcpServerIdentifier option is missing in DHCP server response. Assuming %u.%u.%u.%u\n",
	((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[0],
	((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[1],
	((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[2],
	((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[3]);
    }
  if ( ! DhcpOptions.val[dns] ) /* did not get DNS */
    {	/* make it the same as dhcpServerIdentifier */
      DhcpOptions.val[dns] = malloc(4);
      memcpy(DhcpOptions.val[dns],DhcpOptions.val[dhcpServerIdentifier],4);
      DhcpOptions.len[dns] = 4;
      DhcpOptions.num++;
	DebugSyslog(LOG_DEBUG,
	"dns option is missing in DHCP server response. Assuming %u.%u.%u.%u\n",
	((unsigned char *)DhcpOptions.val[dns])[0],
	((unsigned char *)DhcpOptions.val[dns])[1],
	((unsigned char *)DhcpOptions.val[dns])[2],
	((unsigned char *)DhcpOptions.val[dns])[3]);
    }
  if ( ! DhcpOptions.val[subnetMask] ) /* did not get subnetMask */
    {
      DhcpOptions.val[subnetMask] = malloc(4);
      ((unsigned char *)DhcpOptions.val[subnetMask])[0] = 255;
      if ( ((unsigned char *)&DhcpMsgRecv->yiaddr)[0] < 128 )
        ((unsigned char *)DhcpOptions.val[subnetMask])[1] = 0; /* class A */
      else
	{
          ((unsigned char *)DhcpOptions.val[subnetMask])[1] = 255;
	  if ( ((unsigned char *)&DhcpMsgRecv->yiaddr)[0] < 192 )
	    ((unsigned char *)DhcpOptions.val[subnetMask])[2] = 0;/* class B */
	  else
	    ((unsigned char *)DhcpOptions.val[subnetMask])[2] = 255;/* class C */
	}
      ((unsigned char *)DhcpOptions.val[subnetMask])[3] = 0;
      DhcpOptions.len[subnetMask] = 4;
      DhcpOptions.num++;
	DebugSyslog(LOG_DEBUG,
	"subnetMask option is missing in DHCP server response. Assuming %u.%u.%u.%u\n",
	((unsigned char *)DhcpOptions.val[subnetMask])[0],
	((unsigned char *)DhcpOptions.val[subnetMask])[1],
	((unsigned char *)DhcpOptions.val[subnetMask])[2],
	((unsigned char *)DhcpOptions.val[subnetMask])[3]);
    }
  if ( ! DhcpOptions.val[broadcastAddr] ) /* did not get broadcastAddr */
    {
      int br = DhcpMsgRecv->yiaddr | ~*((int *)DhcpOptions.val[subnetMask]);
      DhcpOptions.val[broadcastAddr] = malloc(4);
      memcpy(DhcpOptions.val[broadcastAddr],&br,4);
      DhcpOptions.len[broadcastAddr] = 4;
      DhcpOptions.num++;
	DebugSyslog(LOG_DEBUG,
	"broadcastAddr option is missing in DHCP server response. Assuming %u.%u.%u.%u\n",
	((unsigned char *)DhcpOptions.val[broadcastAddr])[0],
	((unsigned char *)DhcpOptions.val[broadcastAddr])[1],
	((unsigned char *)DhcpOptions.val[broadcastAddr])[2],
	((unsigned char *)DhcpOptions.val[broadcastAddr])[3]);
    }
#if 0
  if ( ! DhcpOptions.val[routersOnSubnet] )
    {
      DhcpOptions.val[routersOnSubnet] = malloc(4);
      if ( DhcpMsgRecv->giaddr )
      	memcpy(DhcpOptions.val[routersOnSubnet],&DhcpMsgRecv->giaddr,4);
      else
	memcpy(DhcpOptions.val[routersOnSubnet],DhcpOptions.val[dhcpServerIdentifier],4);
      DhcpOptions.len[routersOnSubnet] = 4;
      DhcpOptions.num++;
	DebugSyslog(LOG_DEBUG,
	"routersOnSubnet option is missing in DHCP server response. Assuming %u.%u.%u.%u\n",
	((unsigned char *)DhcpOptions.val[routersOnSubnet])[0],
	((unsigned char *)DhcpOptions.val[routersOnSubnet])[1],
	((unsigned char *)DhcpOptions.val[routersOnSubnet])[2],
	((unsigned char *)DhcpOptions.val[routersOnSubnet])[3]);
    }
#endif
  if ( DhcpOptions.val[dhcpIPaddrLeaseTime] )
    {
      if ( *(unsigned int *)DhcpOptions.val[dhcpIPaddrLeaseTime] == 0 )
	{
          memcpy(DhcpOptions.val[dhcpIPaddrLeaseTime],&nleaseTime,4);
	    DebugSyslog(LOG_DEBUG,
	    "dhcpIPaddrLeaseTime=0 in DHCP server response. Assuming %u sec\n",
	    LeaseTime);
	}
    }
  else /* did not get dhcpIPaddrLeaseTime */
    {
      DhcpOptions.val[dhcpIPaddrLeaseTime] = malloc(4);
      memcpy(DhcpOptions.val[dhcpIPaddrLeaseTime],&nleaseTime,4);
      DhcpOptions.len[dhcpIPaddrLeaseTime] = 4;
      DhcpOptions.num++;
	DebugSyslog(LOG_DEBUG,"dhcpIPaddrLeaseTime option is missing in DHCP server response. Assuming %u sec\n",LeaseTime);
    }
  if ( ! DhcpOptions.val[dhcpT1value] ) /* did not get T1 */
    {
      int t1 = htonl((unsigned )(0.5*ntohl(*(unsigned int *)DhcpOptions.val[dhcpIPaddrLeaseTime])));
      DhcpOptions.val[dhcpT1value] = malloc(4);
      memcpy(DhcpOptions.val[dhcpT1value],&t1,4);
      DhcpOptions.len[dhcpT1value] = 4;
      DhcpOptions.num++;
    }
  if ( ! DhcpOptions.val[dhcpT2value] ) /* did not get T2 */
    {
      int t2 =  htonl((unsigned )(0.875*ntohl(*(unsigned int *)DhcpOptions.val[dhcpIPaddrLeaseTime])));
      DhcpOptions.val[dhcpT2value] = malloc(4);
      memcpy(DhcpOptions.val[dhcpT2value],&t2,4);
      DhcpOptions.len[dhcpT2value] = 4;
      DhcpOptions.num++;
    }
  if ( DhcpOptions.val[dhcpMessageType] )
    return *(unsigned char *)DhcpOptions.val[dhcpMessageType];
  return 0;
}
/*****************************************************************************/
void classIDsetup()
{
  struct utsname sname;
  if ( uname(&sname) ) syslog(LOG_ERR,"classIDsetup: uname: %m\n");
  DhcpIface.class_len=snprintf(DhcpIface.class_id,CLASS_ID_MAX_LEN,
  "%s %s %s",sname.sysname,sname.release,sname.machine);
}
/*****************************************************************************/
void clientIDsetup()
{
  unsigned char *c = DhcpIface.client_id;
  *c++ = dhcpClientIdentifier;
  if ( ClientID )
    {
      *c++ = ClientID_len + 1;	/* 1 for the field below */
      *c++ = 0;			/* type: string */
      memcpy(c,ClientID,ClientID_len);
      DhcpIface.client_len = ClientID_len + 3;
      return;
    }
  *c++ = ETH_ALEN + 1;	        /* length: 6 (MAC Addr) + 1 (# field) */
  *c++ = ARPHRD_ETHER;		/* type: Ethernet address */
  memcpy(c,ClientHwAddr,ETH_ALEN);
  DhcpIface.client_len = ETH_ALEN + 3;
}
/*****************************************************************************/
void releaseDhcpOptions()
{
  register int i;
  for (i=1;i<256;i++)
    if ( DhcpOptions.val[i] ) free(DhcpOptions.val[i]);
  memset(&DhcpOptions,0,sizeof(dhcpOptions));
}
/*****************************************************************************/
/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.  */
static int
timeval_subtract (result, x, y)
     struct timeval *result, *x, *y;
{
  /* Perform the carry for the later subtraction by updating Y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     `tv_usec' is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}
static int
timespec_subtract (result, x, y)
     struct timespec *result, *x, *y;
{
  /* Perform the carry for the later subtraction by updating Y. */
  if (x->tv_nsec < y->tv_nsec) {
    long int nsec = (y->tv_nsec - x->tv_nsec) / 1000000000 + 1;
    y->tv_nsec -= 1000000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_nsec - y->tv_nsec > 1000000000) {
    long int nsec = (x->tv_nsec - y->tv_nsec) / 1000000000;
    y->tv_nsec += 1000000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     `tv_usec' is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_nsec = x->tv_nsec - y->tv_nsec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}
/*****************************************************************************/
int dhcpSendAndRecv(xid,msg,buildUdpIpMsg)
unsigned xid,msg;
void (*buildUdpIpMsg)(unsigned);
{
#ifdef OLD_LINUX_VERSION
  struct sockaddr addr;
#endif
  struct timespec begin, current, diff;
  int i,len;
  int j=DHCP_INITIAL_RTO/2;
  int timeout = 0;
  int retryCount=dhcpRequestMax;
  do
    {
      do
    	{
	  j+=j;
	  if (j > DHCP_MAX_RTO) j = DHCP_MAX_RTO;
	  buildUdpIpMsg(xid);
	  if ((buildUdpIpMsg == buildDhcpReboot) ||
			  (buildUdpIpMsg == buildDhcpReboot)) {
		  retryCount--;
		  DebugSyslog(LOG_WARNING, "retryCount = %d", 
				  dhcpRequestMax - retryCount);
	  }
#ifdef OLD_LINUX_VERSION
      	  memset(&addr,0,sizeof(struct sockaddr));
      	  memcpy(addr.sa_data,IfName,IfName_len);
      	  if ( sendto(dhcpSocket,&UdpIpMsgSend,
		      sizeof(struct packed_ether_header)+
		      sizeof(udpiphdr)+sizeof(dhcpMessage),0,
		      &addr,sizeof(struct sockaddr)) == -1 )
#else
      	  if ( send(dhcpSocket,&UdpIpMsgSend,
		      sizeof(struct packed_ether_header)+
		      sizeof(udpiphdr)+sizeof(dhcpMessage),0) == -1 )
#endif
	    {
	      /* cable is probably not connected */
	      if (errno == ENOBUFS) {
		struct ifreq ifr;
		int err = 0;

		memset(&ifr,0,sizeof(struct ifreq));
		memcpy(ifr.ifr_name,IfName,IfName_len);
		if (ioctl(dhcpSocket,SIOCGIFFLAGS,&ifr) == -1) {
		  syslog(LOG_ERR, "dhcpENOBUFS: ioctl SIOCGIFFLAGS: %m\n");
		  err++;
		}
	        ifr.ifr_flags &= ~IFF_UP;
		if (!err && ioctl(dhcpSocket,SIOCSIFFLAGS,&ifr) == -1) {
		  syslog(LOG_ERR,"dhcpENOBUFS: ioctl SIOCSIFFLAGS1: %m\n");
		  err++;
		}
	        ifr.ifr_flags |= IFF_UP;
		if (!err && ioctl(dhcpSocket,SIOCSIFFLAGS,&ifr) == -1) {
		  syslog(LOG_ERR,"dhcpENOBUFS: ioctl SIOCSIFFLAGS2: %m\n");
		  err++;
		}
		if (err)
		  return -1;
		syslog(LOG_WARNING, "dhcpENOBUFS: Network cable disconnected ?");
	      } else {
	        syslog(LOG_ERR,"sendto: %m\n");
	        return -1;
	      }
	    }
	  clock_gettime(CLOCK_MONOTONIC, &begin);
      	  i=random();
	  if (((buildUdpIpMsg == buildDhcpReboot) || 
				  (buildUdpIpMsg == buildDhcpRequest)) && 
			  (retryCount <= 0))
		  break;
    	}
      while ( peekfd(dhcpSocket,j+i%200000) );
	  if ((buildUdpIpMsg == buildDhcpReboot) ||
			  (buildUdpIpMsg == buildDhcpRequest)) {
		  if (retryCount<=0) {
			  DebugSyslog(LOG_DEBUG, "Retry count timed out "
					  "- falling back to DHCP Discover");
			  return -1;
		  }
	  }
      do
	{
	  struct ip ipRecv_local;
	  memset(&UdpIpMsgRecv,0,sizeof(udpipMessage));
#ifdef OLD_LINUX_VERSION
      	  i=sizeof(struct sockaddr);
      	  len=recvfrom(dhcpSocket,&UdpIpMsgRecv,sizeof(udpipMessage),0,
		     (struct sockaddr *)&addr,&i);
#else
      	  len=recv(dhcpSocket,&UdpIpMsgRecv,sizeof(udpipMessage),0);
#endif
	  if ( len == -1 )
    	    {
	      if (errno == ENETDOWN) {
		struct ifreq ifr;
		int err = 0;

		memset(&ifr,0,sizeof(struct ifreq));
		memcpy(ifr.ifr_name,IfName,IfName_len);
		if (ioctl(dhcpSocket,SIOCGIFFLAGS,&ifr) == -1) {
		  syslog(LOG_ERR, "dhcpENETDOWN: ioctl SIOCGIFFLAGS: %m\n");
		  err++;
		}
	        ifr.ifr_flags |= IFF_UP;
		if (!err && ioctl(dhcpSocket,SIOCSIFFLAGS,&ifr) == -1) {
		  syslog(LOG_ERR,"dhcpENETDOWN: ioctl SIOCSIFFLAGS2: %m\n");
		  err++;
		}
		if (err)
		  return -1;
		syslog(LOG_WARNING, "dhcpENETDOWN: Network cable disconnected ?");
		continue;
	      }
      	      syslog(LOG_ERR,"recvfrom: %m\n");
      	      return -1;
    	    }
	  clock_gettime(CLOCK_MONOTONIC, &current);
	  timespec_subtract(&diff, &current, &begin);
	  timeout = j - diff.tv_sec*1000000 - (diff.tv_nsec/1000) + random()%200000;

	  if ( UdpIpMsgRecv.ethhdr.ether_type != htons(ETHERTYPE_IP) )
	    continue;
	  /* Use local copy because ipRecv is not aligned.  */
	  memcpy(&ipRecv_local, ((struct udpiphdr *)UdpIpMsgRecv.udpipmsg)->ip,
		 sizeof(struct ip));
	  if ( ipRecv_local.ip_p != IPPROTO_UDP ) continue;
	  len-=sizeof(struct packed_ether_header);
	  i=(int )ntohs(ipRecv_local.ip_len);
	  if ( len < i )
	    {
	      DebugSyslog(LOG_DEBUG,
		"corrupted IP packet of size=%d and ip_len=%d discarded\n",
		len,i);
	      continue;
	    }
	  len=i-(ipRecv_local.ip_hl<<2);
	  i=(int )ntohs(udpRecv->uh_ulen);
	  if ( len < i )
	    {
	      DebugSyslog(LOG_DEBUG,
		"corrupted UDP msg of size=%d and uh_ulen=%d discarded\n",
		len,i);
	      continue;
	    }
	  if ( DoCheckSum )
	    {
	      len=udpipchk((udpiphdr *)UdpIpMsgRecv.udpipmsg);
	      if ( len )
		{
		  if ( DebugFlag )
		    switch ( len )
		      {
			case -1: DebugSyslog(LOG_DEBUG,
			  "corrupted IP packet with ip_len=%d discarded\n",
			  (int )ntohs(ipRecv_local.ip_len));
			  break;
			case -2: DebugSyslog(LOG_DEBUG,
			  "corrupted UDP msg with uh_ulen=%d discarded\n",
			  (int )ntohs(udpRecv->uh_ulen));
			break;
		      }
		  continue;
		}
	    }
	  DhcpMsgRecv = (dhcpMessage *)&UdpIpMsgRecv.udpipmsg[(ipRecv_local.ip_hl<<2)+sizeof(struct udphdr)];
	  if ( DhcpMsgRecv->htype != ARPHRD_ETHER ) continue;
	  if ( DhcpMsgRecv->xid != xid ) continue;
	  if ( DhcpMsgRecv->op != DHCP_BOOTREPLY ) continue;
	  if ( parseDhcpMsgRecv() == msg ) return 0;
	  if ( *(unsigned char *)DhcpOptions.val[dhcpMessageType] == DHCP_NAK )
	    {
	      if ( DhcpOptions.val[dhcpMsg] )
		syslog(LOG_ERR,
		"DHCP_NAK server response received: %s\n",
		(char *)DhcpOptions.val[dhcpMsg]);
	      else
		syslog(LOG_ERR,
		"DHCP_NAK server response received\n");
		return 1;
	    }
    	}
      while ( timeout > 0 && peekfd(dhcpSocket, timeout) == 0 );
    }
  while ( 1 );
  return 1;
}
/*****************************************************************************/
int dhcpConfig()
{
  int i;
  FILE *f;
  char	cache_file[48];
  struct ifreq		ifr;
  struct rtentry	rtent;
#ifdef OLD_LINUX_VERSION
  struct sockaddr_pkt	sap;
#endif
  struct sockaddr_in	*p = (struct sockaddr_in *)&(ifr.ifr_addr);
  struct hostent *hp=NULL;
  char *dname=NULL;
  int dname_len=0;
  int failed;
  char cmd[128];

  if ( TestCase ) return 0;
  memset(&ifr,0,sizeof(struct ifreq));
  memcpy(ifr.ifr_name,IfName,IfName_len);
  p->sin_family = AF_INET;
  p->sin_addr.s_addr = DhcpIface.ciaddr;
  if ( ioctl(dhcpSocket,SIOCSIFADDR,&ifr) == -1 )  /* setting IP address */
    {
      syslog(LOG_ERR,"dhcpConfig: ioctl SIOCSIFADDR: %m\n");
      return -1;
    }
  memcpy(&p->sin_addr.s_addr,DhcpOptions.val[subnetMask],4);
  if ( ioctl(dhcpSocket,SIOCSIFNETMASK,&ifr) == -1 )  /* setting netmask */
    {
      syslog(LOG_ERR,"dhcpConfig: ioctl SIOCSIFNETMASK: %m\n");
      return -1;
    }
  memcpy(&p->sin_addr.s_addr,DhcpOptions.val[broadcastAddr],4);
  if ( ioctl(dhcpSocket,SIOCSIFBRDADDR,&ifr) == -1 ) /* setting broadcast address */
    syslog(LOG_ERR,"dhcpConfig: ioctl SIOCSIFBRDADDR: %m\n");

  /* setting local route - not needed on later kernels  */
#ifdef OLD_LINUX_VERSION
  memset(&rtent,0,sizeof(struct rtentry));
  p			=	(struct sockaddr_in *)&rtent.rt_dst;
  p->sin_family		=	AF_INET;
  memcpy(&p->sin_addr.s_addr,DhcpOptions.val[subnetMask],4);
  p->sin_addr.s_addr	&=	DhcpIface.ciaddr;
  p			=	(struct sockaddr_in *)&rtent.rt_gateway;
  p->sin_family		=	AF_INET;
  p->sin_addr.s_addr	=	0;
  p			=	(struct sockaddr_in *)&rtent.rt_genmask;
  p->sin_family		=	AF_INET;
  memcpy(&p->sin_addr.s_addr, DhcpOptions.val[subnetMask], 4);
  rtent.rt_dev	=	IfName;
  rtent.rt_metric     =	1;
  rtent.rt_flags      =	RTF_UP;
  if ( ioctl(dhcpSocket,SIOCADDRT,&rtent) )
    syslog(LOG_ERR,"dhcpConfig: ioctl SIOCADDRT: %m\n");
#endif

  for (i=0;i<DhcpOptions.len[staticRoute];i+=8)
    {  /* setting static routes */
      memset(&rtent,0,sizeof(struct rtentry));
      p                   =   (struct sockaddr_in *)&rtent.rt_dst;
      p->sin_family	  =	  AF_INET;
      memcpy(&p->sin_addr.s_addr,
      ((char *)DhcpOptions.val[staticRoute])+i,4);
      p		          =	  (struct sockaddr_in *)&rtent.rt_gateway;
      p->sin_family	  =	  AF_INET;
      memcpy(&p->sin_addr.s_addr,
      ((char *)DhcpOptions.val[staticRoute])+i+4,4);
      p		          =	  (struct sockaddr_in *)&rtent.rt_genmask;
      p->sin_family	  =       AF_INET;
      p->sin_addr.s_addr  =	  0xffffffff;
      rtent.rt_dev	      =	  IfName;
      rtent.rt_metric     =	  1;
      rtent.rt_flags      =	  RTF_UP|RTF_HOST;
      if ( ioctl(dhcpSocket,SIOCADDRT,&rtent) )
	syslog(LOG_ERR,"dhcpConfig: ioctl SIOCADDRT: %m\n");
    }
  if ( DhcpOptions.len[routersOnSubnet] > 3 )
  for (i=0;i<DhcpOptions.len[routersOnSubnet];i+=4)
    {  /* setting default routes */
      memset(&rtent,0,sizeof(struct rtentry));
      p			=	(struct sockaddr_in *)&rtent.rt_dst;
      p->sin_family		=	AF_INET;
      p->sin_addr.s_addr	=	0;
      p			=	(struct sockaddr_in *)&rtent.rt_gateway;
      p->sin_family		=	AF_INET;
      memcpy(&p->sin_addr.s_addr,
      ((char *)DhcpOptions.val[routersOnSubnet])+i,4);
      if ( p->sin_addr.s_addr==0 || p->sin_addr.s_addr==DhcpIface.ciaddr )
        continue;
      p			=	(struct sockaddr_in *)&rtent.rt_genmask;
      p->sin_family		=	AF_INET;
      p->sin_addr.s_addr	=	0;
      rtent.rt_dev		=	IfName;
      rtent.rt_metric	        =	1;
      rtent.rt_window		=	Window;
      rtent.rt_flags	        =	RTF_UP|RTF_GATEWAY|(Window ? RTF_WINDOW : 0);
      if ( ioctl(dhcpSocket,SIOCADDRT,&rtent) == 0 )
	break;
      else
	{
	  if ( errno == ENETUNREACH )    /* possibly gateway is over the bridge */
	    {                            /* try adding a route to gateway first */
	      memset(&rtent,0,sizeof(struct rtentry));
	      p                   =   (struct sockaddr_in *)&rtent.rt_dst;
	      p->sin_family	      =	  AF_INET;
	      memcpy(&p->sin_addr.s_addr,
	      ((char *)DhcpOptions.val[routersOnSubnet])+i,4);
	      p		      =	  (struct sockaddr_in *)&rtent.rt_gateway;
	      p->sin_family	      =	  AF_INET;
	      p->sin_addr.s_addr  =   0;
	      p		      =	  (struct sockaddr_in *)&rtent.rt_genmask;
	      p->sin_family	      =   AF_INET;
	      p->sin_addr.s_addr  =	  0xffffffff;
	      rtent.rt_dev	      =	  IfName;
	      rtent.rt_metric     =	  0;
	      rtent.rt_flags      =	  RTF_UP|RTF_HOST;
	      if ( ioctl(dhcpSocket,SIOCADDRT,&rtent) == 0 )
		{
		  memset(&rtent,0,sizeof(struct rtentry));
		  p    	             =	(struct sockaddr_in *)&rtent.rt_dst;
		  p->sin_family	     =	AF_INET;
		  p->sin_addr.s_addr =	0;
		  p		     =	(struct sockaddr_in *)&rtent.rt_gateway;
		  p->sin_family	     =	AF_INET;
		  memcpy(&p->sin_addr.s_addr,
		  ((char *)DhcpOptions.val[routersOnSubnet])+i,4);
		  p		     =	(struct sockaddr_in *)&rtent.rt_genmask;
		  p->sin_family	     =	AF_INET;
		  p->sin_addr.s_addr =	0;
		  rtent.rt_dev	     =	IfName;
		  rtent.rt_metric    =	1;
		  rtent.rt_window    =  Window;
	          rtent.rt_flags     =	RTF_UP|RTF_GATEWAY|(Window ? RTF_WINDOW : 0);
	          if ( ioctl(dhcpSocket,SIOCADDRT,&rtent) == 0 )
		    break;
		  else
		    syslog(LOG_ERR,"dhcpConfig: ioctl SIOCADDRT: %m\n");
		}
	      else
		syslog(LOG_ERR,"dhcpConfig: ioctl SIOCADDRT: %m\n");
	    }
	  else
	    syslog(LOG_ERR,"dhcpConfig: ioctl SIOCADDRT: %m\n");
	}
    }

  /* rebind dhcpSocket after changing ip address to avoid problems with 2.0 kernels */
#ifdef OLD_LINUX_VERSION
  memset(&sap,0,sizeof(sap));
  sap.spkt_family = AF_INET;
  sap.spkt_protocol = htons(ETH_P_ALL);
  memcpy(sap.spkt_device,IfName,IfName_len);
  if ( bind(dhcpSocket,(void*)&sap,sizeof(struct sockaddr)) == -1 )
    syslog(LOG_ERR,"dhcpConfig: bind: %m\n");
#endif  

  arpInform();
    DebugSyslog(LOG_DEBUG, "dhcpcd: your IP address = %u.%u.%u.%u\n",
    ((unsigned char *)&DhcpIface.ciaddr)[0],
    ((unsigned char *)&DhcpIface.ciaddr)[1],
    ((unsigned char *)&DhcpIface.ciaddr)[2],
    ((unsigned char *)&DhcpIface.ciaddr)[3]);
  if ( ReplResolvConf )
    {
      mode_t oldumask;
      failed = 0;

      rename(RESOLV_CONF,""RESOLV_CONF".sv");
      oldumask=umask(022);
      f=fopen(RESOLV_CONF,"w");
      umask(oldumask);
      if ( f )
	{
	  int i;
#if 0
	  if ( DhcpOptions.len[nisDomainName] )
	    fprintf(f,"domain %s\n",(char *)DhcpOptions.val[nisDomainName]);
	  else
	    if ( DhcpOptions.len[domainName] )
	      fprintf(f,"domain %s\n",(char *)DhcpOptions.val[domainName]);
#endif
	  for (i=0;i<DhcpOptions.len[dns];i+=4)
	    fprintf(f,"nameserver %u.%u.%u.%u\n",
	    ((unsigned char *)DhcpOptions.val[dns])[i],
	    ((unsigned char *)DhcpOptions.val[dns])[i+1],
	    ((unsigned char *)DhcpOptions.val[dns])[i+2],
	    ((unsigned char *)DhcpOptions.val[dns])[i+3]);
#if 0
	  if ( DhcpOptions.len[nisDomainName] )
	    fprintf(f,"search %s\n",(char *)DhcpOptions.val[nisDomainName]);
#endif
	  if ( DhcpOptions.len[domainName] )
#ifdef __UC_LIBC__
		/* until uC-libc has search support */
	    fprintf(f,"domain %s\n",(char *)DhcpOptions.val[domainName]);
#else
	    fprintf(f,"search %s\n",(char *)DhcpOptions.val[domainName]);
#endif
	  if (ferror(f)) {
	    failed = ENOSPC;
	  }
	  if (fclose(f) != 0) {
	    failed = errno;
	  }
	}
      else {
	failed = errno;
      }

      if (failed) {
	syslog(LOG_ALERT, "dhcpConfig: failed to update %s: %s\n", RESOLV_CONF, strerror(failed));

	/* Presumably we are out of space. Just put it back the way it was */
	rename(RESOLV_CONF ".sv",RESOLV_CONF);
      }

   /* moved the next section of code from before to after we've created
    * resolv.conf. See below for explanation. <poeml@suse.de>
    * res_init() is normally called from within the first function of the
    * resolver which is called. Here, we want resolv.conf to be
    * reread. Otherwise, we won't be able to find out about our hostname,
    * because the resolver won't notice the change in resolv.conf */
      (void)res_init();
    }
  if ( SetHostName )
    {
      if ( ! DhcpOptions.len[hostName] )
	{
	  hp=gethostbyaddr((char *)&DhcpIface.ciaddr,
	  sizeof(DhcpIface.ciaddr),AF_INET);
	  if ( hp )
	    {
	      dname=hp->h_name;
	      while ( *dname > 32 )
		if ( *dname == '.' )
		  break;
		else
		  dname++;
	      dname_len=dname-hp->h_name;
	      DhcpOptions.val[hostName]=(char *)malloc(dname_len+1);
	      DhcpOptions.len[hostName]=dname_len;
	      memcpy((char *)DhcpOptions.val[hostName],
	      hp->h_name,dname_len);
	      ((char *)DhcpOptions.val[hostName])[dname_len]=0;
	      DhcpOptions.num++;
	    }
	}
      if ( DhcpOptions.len[hostName] )
        {
          sethostname(DhcpOptions.val[hostName],DhcpOptions.len[hostName]);
	    DebugSyslog(LOG_DEBUG, "dhcpcd: your hostname = %s\n",
	    (char *)DhcpOptions.val[hostName]);
	}
    }
  if ( SetDomainName )
    {
#if 0
      if ( DhcpOptions.len[nisDomainName] )
        {
          setdomainname(DhcpOptions.val[nisDomainName],
		      DhcpOptions.len[nisDomainName]);
	    DebugSyslog(LOG_DEBUG, "dhcpcd: your domainname = %s\n",
		(char *)DhcpOptions.val[nisDomainName]);
        }
      else
        {
#endif
	  if ( ! DhcpOptions.len[domainName] )
	    {
	      if ( ! hp )
		hp=gethostbyaddr((char *)&DhcpIface.ciaddr,
		sizeof(DhcpIface.ciaddr),AF_INET);
	      if ( hp )
		{
		  dname=hp->h_name;
		  while ( *dname > 32 )
		    if ( *dname == '.' )
		      {
			dname++;
		        break;
		      }
		    else
		      dname++;
		  dname_len=strlen(dname);
		  if ( dname_len )
		    {
		      DhcpOptions.val[domainName]=(char *)malloc(dname_len+1);
		      DhcpOptions.len[domainName]=dname_len;
		      memcpy((char *)DhcpOptions.val[domainName],
		      dname,dname_len);
		      ((char *)DhcpOptions.val[domainName])[dname_len]=0;
		      DhcpOptions.num++;
		    }
		}
	    }
          if ( DhcpOptions.len[domainName] )
            {
              setdomainname(DhcpOptions.val[domainName],
			DhcpOptions.len[domainName]);
		DebugSyslog(LOG_DEBUG, "dhcpcd: your domainname = %s\n",
		(char *)DhcpOptions.val[domainName]);
	    }
#if 0
	}
#endif
    }
  memset(DhcpIface.version,0,sizeof(DhcpIface.version));
  strncpy(DhcpIface.version,VERSION,sizeof(DhcpIface.version));
  sprintf(cache_file,DHCP_CACHE_FILE,IfName);
  failed = 0;
  i=open(cache_file,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR+S_IWUSR);
  if (i >= 0) {
      if (write(i,(char *)&DhcpIface,sizeof(dhcpInterface)) != sizeof(dhcpInterface)) {
	failed = errno;
      }
      if (close(i) != 0) {
	failed = errno;
      }
      if (failed) {
	unlink(cache_file);
      }
  }
  else {
    failed = errno;
  }

  if (failed) {
      /* We don't want to leave an incomplete file, so remove it
       * and log an error
       */
    syslog(LOG_ALERT,"dhcpConfig: failed to write cache file %s: %s\n", cache_file, strerror(failed));
  }

  sprintf(cache_file,DHCP_HOSTINFO,IfName);

  failed = 0;
  f=fopen(cache_file,"w");
  if ( f )
    {
      int b,c;
      failed = 0;

      memcpy(&b,DhcpOptions.val[subnetMask],4);
      c = DhcpIface.ciaddr & b;
      fprintf(f,"\
IPADDR=%u.%u.%u.%u\n\
NETMASK=%u.%u.%u.%u\n\
NETWORK=%u.%u.%u.%u\n\
BROADCAST=%u.%u.%u.%u\n",
((unsigned char *)&DhcpIface.ciaddr)[0],
((unsigned char *)&DhcpIface.ciaddr)[1],
((unsigned char *)&DhcpIface.ciaddr)[2],
((unsigned char *)&DhcpIface.ciaddr)[3],
((unsigned char *)DhcpOptions.val[subnetMask])[0],
((unsigned char *)DhcpOptions.val[subnetMask])[1],
((unsigned char *)DhcpOptions.val[subnetMask])[2],
((unsigned char *)DhcpOptions.val[subnetMask])[3],
((unsigned char *)&c)[0],
((unsigned char *)&c)[1],
((unsigned char *)&c)[2],
((unsigned char *)&c)[3],
((unsigned char *)DhcpOptions.val[broadcastAddr])[0],
((unsigned char *)DhcpOptions.val[broadcastAddr])[1],
((unsigned char *)DhcpOptions.val[broadcastAddr])[2],
((unsigned char *)DhcpOptions.val[broadcastAddr])[3]);
      if ( DhcpOptions.len[routersOnSubnet] > 3 )
	{
	  fprintf(f,"\
GATEWAY=%u.%u.%u.%u",
((unsigned char *)DhcpOptions.val[routersOnSubnet])[0],
((unsigned char *)DhcpOptions.val[routersOnSubnet])[1],
((unsigned char *)DhcpOptions.val[routersOnSubnet])[2],
((unsigned char *)DhcpOptions.val[routersOnSubnet])[3]);
	  for (i=4;i<DhcpOptions.len[routersOnSubnet];i+=4)
  	    fprintf(f,",%u.%u.%u.%u",
	    ((unsigned char *)DhcpOptions.val[routersOnSubnet])[i],
	    ((unsigned char *)DhcpOptions.val[routersOnSubnet])[1+i],
	    ((unsigned char *)DhcpOptions.val[routersOnSubnet])[2+i],
	    ((unsigned char *)DhcpOptions.val[routersOnSubnet])[3+i]);
	}
if ( DhcpOptions.len[staticRoute] )
  {
    fprintf(f,"\nROUTE=%u.%u.%u.%u,%u.%u.%u.%u",
    ((unsigned char *)DhcpOptions.val[staticRoute])[0],
    ((unsigned char *)DhcpOptions.val[staticRoute])[1],
    ((unsigned char *)DhcpOptions.val[staticRoute])[2],
    ((unsigned char *)DhcpOptions.val[staticRoute])[3],
    ((unsigned char *)DhcpOptions.val[staticRoute])[4],
    ((unsigned char *)DhcpOptions.val[staticRoute])[5],
    ((unsigned char *)DhcpOptions.val[staticRoute])[6],
    ((unsigned char *)DhcpOptions.val[staticRoute])[7]);
    for (i=8;i<DhcpOptions.len[staticRoute];i+=8)
    fprintf(f,",%u.%u.%u.%u,%u.%u.%u.%u",
    ((unsigned char *)DhcpOptions.val[staticRoute])[i],
    ((unsigned char *)DhcpOptions.val[staticRoute])[1+i],
    ((unsigned char *)DhcpOptions.val[staticRoute])[2+i],
    ((unsigned char *)DhcpOptions.val[staticRoute])[3+i],
    ((unsigned char *)DhcpOptions.val[staticRoute])[4+i],
    ((unsigned char *)DhcpOptions.val[staticRoute])[5+i],
    ((unsigned char *)DhcpOptions.val[staticRoute])[6+i],
    ((unsigned char *)DhcpOptions.val[staticRoute])[7+i]);
  }
if ( DhcpOptions.len[hostName] )
  fprintf(f,"\nHOSTNAME=%s",(char *)DhcpOptions.val[hostName]);
if ( DhcpOptions.len[domainName] )
  fprintf(f,"\nDOMAIN=%s",(char *)DhcpOptions.val[domainName]);
if ( DhcpOptions.len[nisDomainName] )
  fprintf(f,"\nNISDOMAIN=%s",(char *)DhcpOptions.val[nisDomainName]);
if ( DhcpOptions.len[rootPath] )
  fprintf(f,"\nROOTPATH=%s",(char *)DhcpOptions.val[rootPath]);
fprintf(f,"\n\
DNS=%u.%u.%u.%u",
((unsigned char *)DhcpOptions.val[dns])[0],
((unsigned char *)DhcpOptions.val[dns])[1],
((unsigned char *)DhcpOptions.val[dns])[2],
((unsigned char *)DhcpOptions.val[dns])[3]);
for (i=4;i<DhcpOptions.len[dns];i+=4)
  fprintf(f,",%u.%u.%u.%u",
  ((unsigned char *)DhcpOptions.val[dns])[i],
  ((unsigned char *)DhcpOptions.val[dns])[1+i],
  ((unsigned char *)DhcpOptions.val[dns])[2+i],
  ((unsigned char *)DhcpOptions.val[dns])[3+i]);
fprintf(f,"\n\
DHCPSID=%u.%u.%u.%u\n\
DHCPGIADDR=%u.%u.%u.%u\n\
DHCPSIADDR=%u.%u.%u.%u\n\
DHCPCHADDR=%02X:%02X:%02X:%02X:%02X:%02X\n\
DHCPSHADDR=%02X:%02X:%02X:%02X:%02X:%02X\n\
DHCPSNAME=%s\n\
LEASETIME=%u\n\
RENEWALTIME=%u\n\
REBINDTIME=%u\n",
((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[0],
((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[1],
((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[2],
((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[3],
((unsigned char *)&DhcpMsgRecv->giaddr)[0],
((unsigned char *)&DhcpMsgRecv->giaddr)[1],
((unsigned char *)&DhcpMsgRecv->giaddr)[2],
((unsigned char *)&DhcpMsgRecv->giaddr)[3],
((unsigned char *)&DhcpMsgRecv->siaddr)[0],
((unsigned char *)&DhcpMsgRecv->siaddr)[1],
((unsigned char *)&DhcpMsgRecv->siaddr)[2],
((unsigned char *)&DhcpMsgRecv->siaddr)[3],
ClientHwAddr[0],
ClientHwAddr[1],
ClientHwAddr[2],
ClientHwAddr[3],
ClientHwAddr[4],
ClientHwAddr[5],
DhcpIface.shaddr[0],
DhcpIface.shaddr[1],
DhcpIface.shaddr[2],
DhcpIface.shaddr[3],
DhcpIface.shaddr[4],
DhcpIface.shaddr[5],
DhcpMsgRecv->sname,
ntohl(*(unsigned int *)DhcpOptions.val[dhcpIPaddrLeaseTime]),
ntohl(*(unsigned int *)DhcpOptions.val[dhcpT1value]),
ntohl(*(unsigned int *)DhcpOptions.val[dhcpT2value]));

      if (ferror(f)) {
	failed = ENOSPC;
      }
      if (fclose(f) != 0) {
	failed = errno;
      }
      if (failed) {
	/* We don't want to leave an incomplete file, so remove it
	 * and log an error
	 */
	 unlink(cache_file);
      }
    }
    else {
      failed = errno;
    }

  if (failed) {
    syslog(LOG_ALERT,"dhcpConfig: failed to write info file %s: %s\n", cache_file, strerror(failed));
  }

#ifdef CONFIG_LEDMAN
  if ( ledman_led >= 0 )
    ledman_cmd(LEDMAN_CMD_ALT_OFF, ledman_led);
#endif
  if ( Cfilename )
#ifndef __uClinux__
    if ( fork() == 0 )
#else
    if ( vfork() == 0 )
#endif
      {
	char *argc[2];
	argc[0]=Cfilename;
	argc[1]=NULL;
	if ( execve(Cfilename,argc,ProgramEnviron) )
	  syslog(LOG_ERR,"error executing \"%s\": %m\n",
	  Cfilename);
#ifndef __uClinux__
	exit(0);
#else
	_exit(0);
#endif
      }
  if ( DhcpIface.ciaddr != prev_ip_addr ) /* IP address has changed */
    {
#ifdef EMBED
      struct stat s;
#endif
      char *argc[4],filename[64],ipaddrstr[16];
      sprintf(ipaddrstr,"%u.%u.%u.%u",
      ((unsigned char *)&DhcpIface.ciaddr)[0],
      ((unsigned char *)&DhcpIface.ciaddr)[1],
      ((unsigned char *)&DhcpIface.ciaddr)[2],
      ((unsigned char *)&DhcpIface.ciaddr)[3]);
      sprintf(filename,EXEC_ON_IP_CHANGE,IfName);
#ifdef EMBED
      if ( stat(filename, &s) != 0 )
	strcpy(filename,OLD_EXEC_ON_IP_CHANGE);
#endif
      argc[0]=PROGRAM_NAME;
      argc[1]=ipaddrstr;
      if ( DebugFlag )
	argc[2]="-d";
      else
	argc[2]=NULL;
      argc[3]=NULL;
#ifndef __uClinux__
      if ( fork() == 0 )
#else
      if ( vfork() == 0 )
#endif
	{
	  if ( execve(filename,argc,ProgramEnviron) && errno != ENOENT )
	    syslog(LOG_ERR,"error executing \"%s %s\": %m\n",
	    filename,ipaddrstr);
#ifndef __uClinux__
	  exit(0);
#else
	  _exit(0);
#endif
	}
      prev_ip_addr=DhcpIface.ciaddr;
    }
  if ( *(unsigned int *)DhcpOptions.val[dhcpIPaddrLeaseTime] == 0xffffffff )
    {
      if (FiniteLeaseOnly) {
        DhcpOptions.val[dhcpIPaddrLeaseTime]--;
      } else {
        syslog(LOG_INFO,"infinite IP address lease time. Exiting\n");
		sendDhcpMonitorMsg(NET_MSG_DHCP_CONFIG_OK, NULL, NULL, NULL);
        exit(0);
      }
    }
  return 0;
}
/*****************************************************************************/
int readDhcpCache()
{
  int i,o;
  char cache_file[48];
  sprintf(cache_file,DHCP_CACHE_FILE,IfName);
  i=open(cache_file,O_RDONLY);
  if ( i == -1 ) return -1;
  o=read(i,(char *)&DhcpIface,sizeof(dhcpInterface));
  close(i);
  if ( o != sizeof(dhcpInterface) ) return -1;
  if ( strncmp(DhcpIface.version,VERSION,sizeof(DhcpIface.version)) ) return -1;
  return 0;
}
/*****************************************************************************/
void deleteDhcpCache()
{
  char cache_file[48];
  sprintf(cache_file,DHCP_CACHE_FILE,IfName);
  unlink(cache_file);
}
/*****************************************************************************/
void openSocket()
{
  int o = 1;
  struct ifreq	ifr;
#ifdef OLD_LINUX_VERSION
  struct sockaddr_pkt sap;
#else
  struct sockaddr_ll sap;
#endif
  memset(&ifr,0,sizeof(struct ifreq));
  memcpy(ifr.ifr_name,IfName,IfName_len);
#ifdef OLD_LINUX_VERSION
  dhcpSocket = socket(AF_INET,SOCK_PACKET,htons(ETH_P_ALL));
#else
  dhcpSocket = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
#endif
  if ( dhcpSocket == -1 )
    {
      syslog(LOG_ERR,"dhcpStart: socket: %m\n");
	  deletePidFile();
      exit(1);
    }

  if ( ioctl(dhcpSocket,SIOCGIFHWADDR,&ifr) )
    {
      syslog(LOG_ERR,"dhcpStart: ioctl SIOCGIFHWADDR: %m\n");
	  deletePidFile();
      exit(1);
    }
  memcpy(ClientHwAddr,ifr.ifr_hwaddr.sa_data,ETH_ALEN);

  if ( ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER )
    {
      syslog(LOG_ERR,"dhcpStart: interface %s is not Ethernet\n",ifr.ifr_name);
	  deletePidFile();
      exit(1);
    }

  if ( setsockopt(dhcpSocket,SOL_SOCKET,SO_BROADCAST,&o,sizeof(o)) == -1 )
    {
      syslog(LOG_ERR,"dhcpStart: setsockopt: %m\n");
	  deletePidFile();
      exit(1);
    }

  if ( ioctl(dhcpSocket,SIOCGIFFLAGS,&ifr) )
    {
      syslog(LOG_ERR, "dhcpStart: ioctl SIOCGIFFLAGS: %m\n");
	  deletePidFile();
      exit(1);
    }

  ifr.ifr_flags |= IFF_UP | IFF_BROADCAST | IFF_NOTRAILERS | IFF_RUNNING;
  if ( ioctl(dhcpSocket,SIOCSIFFLAGS,&ifr) )
    {
      syslog(LOG_ERR,"dhcpStart: ioctl SIOCSIFFLAGS: %m\n");
	  deletePidFile();
      exit(1);
    }

  memset(&sap,0,sizeof(sap));
#ifdef OLD_LINUX_VERSION
  sap.spkt_family = AF_INET;
  sap.spkt_protocol = htons(ETH_P_ALL);
  memcpy(sap.spkt_device,IfName,IfName_len);
#else
  if ( ioctl(dhcpSocket,SIOCGIFINDEX,&ifr) )
    {
      syslog(LOG_ERR, "dhcpStart: ioctl SIOCGIFFLAGS: %m\n");
      exit(1);
    }
  sap.sll_family = AF_PACKET;
  sap.sll_protocol = 0;
  sap.sll_ifindex = ifr.ifr_ifindex;
#endif

  if ( bind(dhcpSocket, (void*)&sap, sizeof(sap)) == -1 )
    syslog(LOG_ERR,"dhcpStart: bind: %m\n");

  /* flush any non-bound packets we may have received */
  /* yes this does happen - DavidM  :-) */

  while ( peekfd(dhcpSocket, 0) == 0 )
    (void) recv(dhcpSocket, &UdpIpMsgRecv, sizeof(udpipMessage), 0);
}
/*****************************************************************************/


unsigned long long  monotonic_us(void)
{
     struct timespec tv;
     clock_gettime(CLOCK_MONOTONIC, &tv);
     return tv.tv_sec * 1000000ULL + tv.tv_nsec / 1000;
}
void *dhcpStart()
{
  openSocket();
  //ip_id=time(NULL)&0xffff;
  //srandom(ip_id);
  srand(monotonic_us());
#ifdef CONFIG_LEDMAN
  if ( ledman_led >= 0 )
    {
      ledman_cmd(LEDMAN_CMD_ALT_ON, ledman_led);
      ledman_cmd(LEDMAN_CMD_FLASH | LEDMAN_CMD_ALTBIT, ledman_led);
    }
#endif
  return &dhcpInit;
}
/*****************************************************************************/
void *dhcpReboot()
{
  dhcpStart();
  memset(&DhcpOptions,0,sizeof(DhcpOptions));
  memset(&DhcpIface,0,sizeof(dhcpInterface));
  if ( readDhcpCache() )
    {
      struct ifreq	ifr;
      struct sockaddr_in *p = (struct sockaddr_in *)&(ifr.ifr_addr);
      memset(&DhcpIface,0,sizeof(dhcpInterface));
      memset(&ifr,0,sizeof(struct ifreq));
      memcpy(ifr.ifr_name,IfName,IfName_len);
      p->sin_family = AF_INET;
      if ( ioctl(dhcpSocket,SIOCGIFADDR,&ifr) == 0 )
        DhcpIface.ciaddr=p->sin_addr.s_addr;
      if ( ClassID )
	{
    	  memcpy(DhcpIface.class_id,ClassID,ClassID_len);
	  DhcpIface.class_len=ClassID_len;
	}
      else
	classIDsetup();
      clientIDsetup();
      return &dhcpInit;
    }
  if ( sigsetjmp(env,0xffff) )
    {
	DebugSyslog(LOG_DEBUG,"timed out waiting for DHCP_ACK response\n");
      alarm(TimeOut);
      return &dhcpInit;
    }
  return dhcpRequest(random(),&buildDhcpReboot);
}
/*****************************************************************************/
void *dhcpInit()
{
  releaseDhcpOptions();

#ifdef DEBUG
fprintf(stderr,"ClassID  = \"%s\"\n\
ClientID = \"%u.%u.%u.%02X.%02X.%02X.%02X.%02X.%02X\"\n",
DhcpIface.class_id,
DhcpIface.client_id[0],DhcpIface.client_id[1],DhcpIface.client_id[2],
DhcpIface.client_id[3],DhcpIface.client_id[4],DhcpIface.client_id[5],
DhcpIface.client_id[6],DhcpIface.client_id[7],DhcpIface.client_id[8]);
#endif

  DebugSyslog(LOG_DEBUG,"broadcasting DHCP_DISCOVER\n");
  if ( dhcpSendAndRecv(random(),DHCP_OFFER,&buildDhcpDiscover) )
    {
#if 0
      dhcpStop();
      return 0;
#endif
	  return &dhcpInit;
    }
  DebugSyslog(LOG_DEBUG,"broadcasting second DHCP_DISCOVER\n");
  dhcpSendAndRecv(DhcpMsgRecv->xid,DHCP_OFFER,&buildDhcpDiscover);
  DhcpIface.ciaddr = DhcpMsgRecv->yiaddr;
  memcpy(&DhcpIface.siaddr,DhcpOptions.val[dhcpServerIdentifier],4);
  memcpy(DhcpIface.shaddr,UdpIpMsgRecv.ethhdr.ether_shost,ETH_ALEN);
  DhcpIface.xid = DhcpMsgRecv->xid;
/* DHCP_OFFER received */
  DebugSyslog(LOG_DEBUG,"DHCP_OFFER received from %s (%u.%u.%u.%u)\n",
    DhcpMsgRecv->sname,
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[0],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[1],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[2],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[3]);

  return dhcpRequest(DhcpIface.xid,&buildDhcpRequest);
}
/*****************************************************************************/
void *dhcpRequest(xid,buildDhcpMsg)
unsigned xid;
void (*buildDhcpMsg)(unsigned);
{
char ipaddress[16] = {0};
  prev_ip_addr = 0;
/* send the message and read and parse replies into DhcpOptions */
  DebugSyslog(LOG_DEBUG,"broadcasting DHCP_REQUEST (dhcpRequest) for %u.%u.%u.%u\n",
	   ((unsigned char *)&DhcpIface.ciaddr)[0],
	   ((unsigned char *)&DhcpIface.ciaddr)[1],
	   ((unsigned char *)&DhcpIface.ciaddr)[2],
	   ((unsigned char *)&DhcpIface.ciaddr)[3]);
  if ( dhcpSendAndRecv(xid,DHCP_ACK,buildDhcpMsg) ) return &dhcpInit;
  ReqSentTime=time(NULL);
  DebugSyslog(LOG_DEBUG,
    "DHCP_ACK received from %s (%u.%u.%u.%u)\n",DhcpMsgRecv->sname,
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[0],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[1],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[2],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[3]);
#ifdef ARPCHECK
/* check if the offered IP address already in use */
  if ( arpCheck() )
    {
      DebugSyslog(LOG_DEBUG,
	"requested %u.%u.%u.%u address is in use\n",
	((unsigned char *)&DhcpIface.ciaddr)[0],
	((unsigned char *)&DhcpIface.ciaddr)[1],
	((unsigned char *)&DhcpIface.ciaddr)[2],
	((unsigned char *)&DhcpIface.ciaddr)[3]);
	 sprintf(ipaddress,"%u.%u.%u.%u",
	 ((unsigned char *)&DhcpIface.ciaddr)[0],
	 ((unsigned char *)&DhcpIface.ciaddr)[1],
	 ((unsigned char *)&DhcpIface.ciaddr)[2],
	 ((unsigned char *)&DhcpIface.ciaddr)[3]);
      dhcpDecline();
      DhcpIface.ciaddr = 0;
      sendDhcpMonitorMsg(NET_MSG_DHCP_IP_CONFLICT, ipaddress, NULL, NULL);
      return &dhcpInit;
    }
  DebugSyslog(LOG_DEBUG,
    "verified %u.%u.%u.%u address is not in use\n",
    ((unsigned char *)&DhcpIface.ciaddr)[0],
    ((unsigned char *)&DhcpIface.ciaddr)[1],
    ((unsigned char *)&DhcpIface.ciaddr)[2],
    ((unsigned char *)&DhcpIface.ciaddr)[3]);
#endif
  if ( dhcpConfig() )
    {
      dhcpStop();
      sendDhcpMonitorMsg(NET_MSG_DHCP_CONFIG_NOK, NULL, NULL, NULL);
      return 0;
    }
   	else
   		sendDhcpMonitorMsg(NET_MSG_DHCP_CONFIG_OK, NULL, NULL, NULL);

  /* Successfull ACK: Use the fields obtained for future requests */
  memcpy(&DhcpIface.siaddr,DhcpOptions.val[dhcpServerIdentifier],4);
  memcpy(DhcpIface.shaddr,UdpIpMsgRecv.ethhdr.ether_shost,ETH_ALEN);

  return &dhcpBound;
}
/*****************************************************************************/
void *dhcpBound()
{
  int i;
  close(dhcpSocket);
  dhcpSocket = -1;
  if ( sigsetjmp(env,0xffff) )
    {
      openSocket();
      return &dhcpRenew;
    }
  i=ReqSentTime+ntohl(*(unsigned int *)DhcpOptions.val[dhcpT1value])-time(NULL);
  if ( i > 0 )
    alarm(i);
  else
    {
      openSocket();
      return &dhcpRenew;
    }

  sleep(ntohl(*(u_int *)DhcpOptions.val[dhcpT1value]));
  openSocket();
  return &dhcpRenew;
}
/*****************************************************************************/
void *dhcpRenew()
{
  int i;
  if ( sigsetjmp(env,0xffff) ) return &dhcpRebind;
  i = ReqSentTime+ntohl(*(unsigned int *)DhcpOptions.val[dhcpT2value])-time(NULL);
  if ( i > 0 )
    alarm(i);
  else
    return &dhcpRebind;

  DebugSyslog(LOG_DEBUG,"sending DHCP_REQUEST for %u.%u.%u.%u to %u.%u.%u.%u\n",
	   ((unsigned char *)&DhcpIface.ciaddr)[0],
	   ((unsigned char *)&DhcpIface.ciaddr)[1],
	   ((unsigned char *)&DhcpIface.ciaddr)[2],
	   ((unsigned char *)&DhcpIface.ciaddr)[3],
	   ((unsigned char *)&DhcpIface.siaddr)[0],
	   ((unsigned char *)&DhcpIface.siaddr)[1],
	   ((unsigned char *)&DhcpIface.siaddr)[2],
	   ((unsigned char *)&DhcpIface.siaddr)[3]);
  if ( dhcpSendAndRecv(random(),DHCP_ACK,&buildDhcpRenew) ) return &dhcpRebind;
  ReqSentTime=time(NULL);
  DebugSyslog(LOG_DEBUG,
    "DHCP_ACK received from %s (%u.%u.%u.%u)\n",DhcpMsgRecv->sname,
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[0],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[1],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[2],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[3]);
  return &dhcpBound;
}
/*****************************************************************************/
void *dhcpRebind()
{
  int i;
  if ( sigsetjmp(env,0xffff) ) return &dhcpStop;
  i = ReqSentTime+ntohl(*(unsigned int *)DhcpOptions.val[dhcpIPaddrLeaseTime])-time(NULL);
  if ( i > 0 )
    alarm(i);
  else
    return &dhcpStop;

  prev_ip_addr = 0;
  DebugSyslog(LOG_DEBUG,"broadcasting DHCP_REQUEST (dhcpRebind) for %u.%u.%u.%u\n",
	   ((unsigned char *)&DhcpIface.ciaddr)[0],
	   ((unsigned char *)&DhcpIface.ciaddr)[1],
	   ((unsigned char *)&DhcpIface.ciaddr)[2],
	   ((unsigned char *)&DhcpIface.ciaddr)[3]);
  if ( dhcpSendAndRecv(random(),DHCP_ACK,&buildDhcpRebind) ) return &dhcpStop;
  ReqSentTime=time(NULL);
  DebugSyslog(LOG_DEBUG,
    "DHCP_ACK received from %s (%u.%u.%u.%u)\n",DhcpMsgRecv->sname,
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[0],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[1],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[2],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[3]);

  /* Successfull ACK: Use the fields obtained for future requests */
  memcpy(&DhcpIface.siaddr,DhcpOptions.val[dhcpServerIdentifier],4);
  memcpy(DhcpIface.shaddr,UdpIpMsgRecv.ethhdr.ether_shost,ETH_ALEN);

  return &dhcpBound;
}
/*****************************************************************************/
void *dhcpRelease()
{
#ifdef OLD_LINUX_VERSION
  struct sockaddr addr;
#endif
  deleteDhcpCache();
  if ( DhcpIface.ciaddr == 0 ) return &dhcpInit;

  if ( dhcpSocket < 0 )
    openSocket();
  buildDhcpRelease(random());

  DebugSyslog(LOG_DEBUG,"sending DHCP_RELEASE for %u.%u.%u.%u to %u.%u.%u.%u\n",
	   ((unsigned char *)&DhcpIface.ciaddr)[0],
	   ((unsigned char *)&DhcpIface.ciaddr)[1],
	   ((unsigned char *)&DhcpIface.ciaddr)[2],
	   ((unsigned char *)&DhcpIface.ciaddr)[3],
	   ((unsigned char *)&DhcpIface.siaddr)[0],
	   ((unsigned char *)&DhcpIface.siaddr)[1],
	   ((unsigned char *)&DhcpIface.siaddr)[2],
	   ((unsigned char *)&DhcpIface.siaddr)[3]);
#ifdef OLD_LINUX_VERSION
  memset(&addr,0,sizeof(struct sockaddr));
  memcpy(addr.sa_data,IfName,IfName_len);
  if ( sendto(dhcpSocket,&UdpIpMsgSend,sizeof(struct packed_ether_header)+
	      sizeof(udpiphdr)+sizeof(dhcpMessage),0,
	      &addr,sizeof(struct sockaddr)) == -1 )
    syslog(LOG_ERR,"dhcpRelease: sendto: %m\n");
#else
  if ( send(dhcpSocket,&UdpIpMsgSend,sizeof(struct packed_ether_header)+
	      sizeof(udpiphdr)+sizeof(dhcpMessage),0) == -1 )
    syslog(LOG_ERR,"dhcpRelease: send: %m\n");
#endif
  arpRelease(); /* clear ARP cache entries for client IP addr */
  return &dhcpInit;
}
/*****************************************************************************/
void *dhcpStop()
{
  struct ifreq ifr;
  struct sockaddr_in	*p = (struct sockaddr_in *)&(ifr.ifr_addr);

  if ( dhcpSocket < 0 )
    openSocket();
  releaseDhcpOptions();
  if ( TestCase ) return &dhcpStart;
#if 0
  memset(&ifr,0,sizeof(struct ifreq));
  memcpy(ifr.ifr_name,IfName,IfName_len);
  p->sin_family = AF_INET;
  p->sin_addr.s_addr = 0;
#ifndef OLD_LINUX_VERSION
  if ( ioctl(dhcpSocket,SIOCSIFADDR,&ifr) == -1 )
    syslog(LOG_ERR,"dhcpStop: ioctl SIOCSIFADDR: %m\n");
#endif
  if ( ioctl(dhcpSocket,SIOCSIFFLAGS,&ifr) )
    syslog(LOG_ERR,"dhcpStop: ioctl SIOCSIFFLAGS: %m\n");
#endif	
  close(dhcpSocket);
  if ( ReplResolvConf )
    rename(""RESOLV_CONF".sv",RESOLV_CONF);
  return &dhcpStart;
}
/*****************************************************************************/
#ifdef ARPCHECK
void *dhcpDecline()
{
  struct sockaddr addr;
  memset(&UdpIpMsgSend,0,sizeof(udpipMessage));
  memcpy(UdpIpMsgSend.ethhdr.ether_dhost,MAC_BCAST_ADDR,ETH_ALEN);
  memcpy(UdpIpMsgSend.ethhdr.ether_shost,ClientHwAddr,ETH_ALEN);
  UdpIpMsgSend.ethhdr.ether_type = htons(ETHERTYPE_IP);
  buildDhcpDecline(random());
  udpipgen((udpiphdr *)&UdpIpMsgSend.udpipmsg,0,INADDR_BROADCAST,
  htons(DHCP_CLIENT_PORT),htons(DHCP_SERVER_PORT),sizeof(dhcpMessage));
  DebugSyslog(LOG_DEBUG,"broadcasting DHCP_DECLINE\n");
#ifdef OLD_LINUX_VERSION
  memset(&addr,0,sizeof(struct sockaddr));
  memcpy(addr.sa_data,IfName,IfName_len);
  if ( sendto(dhcpSocket,&UdpIpMsgSend,sizeof(struct packed_ether_header)+
	      sizeof(udpiphdr)+sizeof(dhcpMessage),0,
	      &addr,sizeof(struct sockaddr)) == -1 )
    syslog(LOG_ERR,"dhcpDecline: sendto: %m\n");
#else
  if ( send(dhcpSocket,&UdpIpMsgSend,sizeof(struct packed_ether_header)+
	      sizeof(udpiphdr)+sizeof(dhcpMessage),0) == -1 )
    syslog(LOG_ERR,"dhcpDecline: sendto: %m\n");
#endif
  return &dhcpInit;
}
#endif
/*****************************************************************************/
void checkIfAlreadyRunning()
{
  FILE *fp;
  pid_t pid;
  char pidfile[48];
  sprintf(pidfile,PID_FILE_PATH,IfName);
  fp=fopen(pidfile,"r");
  if (!fp) return;

  fscanf(fp,"%u",&pid);
  fclose(fp);

  fprintf(stderr, "kill(pid=%d, sig=0) returned %d\n", pid, kill(pid, 0));

  if ( kill(pid,0) != 0)
    {
      unlink(pidfile);
	  return;
    }

  fprintf(stderr,"****  %s: already running at pid %d (%m)\n", ProgramName, pid);
  exit(1);
}

/*****************************************************************************/
void *dhcpInform()
{
  dhcpStart();
  memset(&DhcpOptions,0,sizeof(DhcpOptions));
  memset(&DhcpIface,0,sizeof(dhcpInterface));
  if ( ! inform_ipaddr.s_addr )
    {
      struct ifreq ifr;
      struct sockaddr_in *p = (struct sockaddr_in *)&(ifr.ifr_addr);
      memset(&ifr,0,sizeof(struct ifreq));
      memcpy(ifr.ifr_name,IfName,IfName_len);
      p->sin_family = AF_INET;
      if ( ioctl(dhcpSocket,SIOCGIFADDR,&ifr) == 0 )
        inform_ipaddr.s_addr=p->sin_addr.s_addr;
      if ( ! inform_ipaddr.s_addr )
	{
	  if ( readDhcpCache() )
	    {
	      syslog(LOG_ERR,"dhcpInform: no IP address given\n");
	      return 0;
	    }
          else
	    inform_ipaddr.s_addr=DhcpIface.ciaddr;
	}
    }
  DhcpIface.ciaddr=inform_ipaddr.s_addr;
  if ( ! DhcpIface.class_len )
    { 
      if ( ClassID )
        {
    	  memcpy(DhcpIface.class_id,ClassID,ClassID_len);
	  DhcpIface.class_len=ClassID_len;
        }
      else
        classIDsetup();
    }
  if ( ! DhcpIface.client_len ) clientIDsetup();
  if ( sigsetjmp(env,0xffff) )
    {
	DebugSyslog(LOG_DEBUG,"timed out waiting for DHCP_ACK response\n");
      return 0;
    }
  DebugSyslog(LOG_DEBUG,"broadcasting DHCP_INFORM for %u.%u.%u.%u\n",
	   ((unsigned char *)&DhcpIface.ciaddr)[0],
	   ((unsigned char *)&DhcpIface.ciaddr)[1],
	   ((unsigned char *)&DhcpIface.ciaddr)[2],
	   ((unsigned char *)&DhcpIface.ciaddr)[3]);
  if ( dhcpSendAndRecv(random(),DHCP_ACK,buildDhcpInform) ) return 0;
  DebugSyslog(LOG_DEBUG,
    "DHCP_ACK received from %s (%u.%u.%u.%u)\n",DhcpMsgRecv->sname,
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[0],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[1],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[2],
    ((unsigned char *)DhcpOptions.val[dhcpServerIdentifier])[3]);
#ifdef ARPCHECK
/* check if the offered IP address already in use */
  if ( arpCheck() )
    {
      DebugSyslog(LOG_DEBUG,
	"requested %u.%u.%u.%u address is in use\n",
	((unsigned char *)&DhcpIface.ciaddr)[0],
	((unsigned char *)&DhcpIface.ciaddr)[1],
	((unsigned char *)&DhcpIface.ciaddr)[2],
	((unsigned char *)&DhcpIface.ciaddr)[3]);
      dhcpDecline();
      return 0;
    }
  DebugSyslog(LOG_DEBUG,
    "verified %u.%u.%u.%u address is not in use\n",
    ((unsigned char *)&DhcpIface.ciaddr)[0],
    ((unsigned char *)&DhcpIface.ciaddr)[1],
    ((unsigned char *)&DhcpIface.ciaddr)[2],
    ((unsigned char *)&DhcpIface.ciaddr)[3]);
#endif
  if ( dhcpConfig() ) return 0;
  exit(0);
}
