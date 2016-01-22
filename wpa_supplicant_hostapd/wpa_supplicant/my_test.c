#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/uio.h>
//#include <linux/types.h>
#include <dirent.h>
#include <netdb.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <syslog.h>
#include <sys/mman.h>

#include <asm/types.h>

#include "at_wpa_cli.h"


at_wpa_list_t * test_search_ap(void)
{
	at_wpa_list_t *list;
	at_wpa_list_t *node;
	wifi_connect();
	list = wifi_search();
//	wifi_close_connection();

	node = list;
	while (node) {
		printf(
				">>>>>>>>>network_id:%d; bssid:%s; ssid:%s; sig_lev:%d; is_current:%d\n",
				node->network_id, node->bssid, node->ssid, node->signal_lev,
				node->is_current);

		node = node->next;
	}
	return list;
}

at_wpa_list_t *test_update_list(at_wpa_list_t *old_list)
{
	at_wpa_list_t *list;
	at_wpa_list_t *node;

	list = wifi_update_list(old_list);

	node = list;
	while (node) {
		printf(
				">>>>>>>>>network_id:%d; bssid:%s; ssid:%s; sig_lev:%d; is_current:%d\n",
				node->network_id, node->bssid, node->ssid, node->signal_lev,
				node->is_current);

		node = node->next;
	}
	return list;
}

int phone_smart_get_ip()
{
	int sock;
	struct ifreq req;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1) {
		printf("ERROR: create socket\n");
	}


	strncpy(req.ifr_name, "wlan0", IFNAMSIZ);

	if ( ioctl(sock, SIOCGIFADDR, &req) < 0 ) {
		printf("ERROR: ioctl \n");
		return -1;
	}
	printf("----get ip:%s\n", inet_ntoa(*(struct in_addr *) &((struct sockaddr_in *) &req.ifr_addr)->sin_addr));
	close(sock);
	return 0;
}

int main(void)
{
	int ret;
	int network_id;
	at_wpa_list_t *list;
	ret = wifi_up(0, "./wpa_supplicant.conf");
	if(ret < 0) {
		abort();
	}

	system("ifconfig wlan0 down");
	system("ifconfig wlan0 up");
	system("ifconfig wlan0 0.0.0.0");
	system("dhcpcd wlan0");
	phone_smart_get_ip();
	printf("\n~~~~~~~~~~~~~~~~~~first search\n");
	list = test_search_ap();
	while(list != NULL)
	{
		printf("%s\n", list->ssid);
		list = list->next;
	}
	network_id = wifi_add(NULL, "cisco", "atcom123456");
	if(network_id < 0)
	{
		printf("----ERROR: add wifi error\n");
	}
	else
	{
		int counter = 0;

		wifi_edit(network_id, "d8:24:bd:76:ce:ab", "cisco", "atcom123456");
		printf("---SUCCESS: add wifi success\n");
		wifi_select(network_id);
		while(counter < 20)
		{
			sleep(1);
			counter++;
			if(9 == wifi_get_auth_state())
			{
				at_wpa_list_t *cur_wifi = NULL;
				cur_wifi = wifi_update_list(NULL);
				if(cur_wifi && cur_wifi->is_current == 1
					&& cur_wifi->network_id == network_id)
				{
					wifi_list_free(cur_wifi);
					phone_smart_get_ip();
					system("ifconfig -a");
					printf("---connect to wifi success\n");
				}
				else
				{
					wifi_list_free(cur_wifi);
					wifi_remove(network_id);
					printf("---connect to wifi fail\n");
				}
				break;
			}
		}
		if(counter == 20)
		{
			printf("---connect to wifi fail\n");
		}
	}


#if 0
	network_id = wifi_add(NULL, "amoji", "147258369");
	if(network_id < 0) {
		printf("----ERROR: add wifi error\n");
	}

	wifi_edit(network_id, "9c:d6:43:d1:34:10", "amo", "147258369147");

	printf("\n~~~~~~~~~~~~~~~~~~add a netowrk\n");
	list = test_update_list(list);
	wifi_select(network_id);

	sleep(5);

	printf("\n~~~~~~~~~~~~~~~~~~select a netowrk\n");
	list = test_update_list(list);

	printf("=============RSSI:%d\n",wifi_get_rssi());

	wifi_disable(network_id);
	printf("\n~~~~~~~~~~~~~~~~~~disable a netowrk\n");
	list = test_update_list(list);

	wifi_remove(network_id);
	printf("\n~~~~~~~~~~~~~~~~~~remove a netowrk\n");
	list = test_update_list(list);


	free(list);          //**app must free list
#endif
	//printf("==========state:%d\n", wifi_get_auth_state());
	wifi_list_free(list);
	wifi_down();
	return 0;
}

