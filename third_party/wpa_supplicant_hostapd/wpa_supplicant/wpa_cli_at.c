/*
 * WPA Supplicant - command line interface for wpa_supplicant daemon
 * Copyright (c) 2004-2011, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "includes.h"
#include "at_wpa_cli.h"
#include <pthread.h>

#ifdef CONFIG_CTRL_IFACE_UNIX
#include <dirent.h>
#endif /* CONFIG_CTRL_IFACE_UNIX */

#include "common/wpa_ctrl.h"
#include "utils/common.h"
#include "utils/eloop.h"
#include "utils/edit.h"
#include "common/version.h"
#ifdef ANDROID
#include <cutils/properties.h>
#endif /* ANDROID */


static struct wpa_ctrl *ctrl_conn = NULL;
static struct wpa_ctrl *mon_conn = NULL;
//static int wpa_cli_quit = 0;
static int wpa_cli_attached = 0;
//static int wpa_cli_connected = 0;
//static int wpa_cli_last_id = 0;
#ifndef CONFIG_CTRL_IFACE_DIR
#define CONFIG_CTRL_IFACE_DIR "/var/run/wpa_supplicant"
#endif /* CONFIG_CTRL_IFACE_DIR */
static const char *ctrl_iface_dir = CONFIG_CTRL_IFACE_DIR;
static char *ctrl_ifname = NULL;
//static const char *pid_file = NULL;
//static const char *action_file = NULL;
//static int ping_interval = 5;
static int interactive = 0;
static pthread_mutex_t at_wpa_mutex = PTHREAD_MUTEX_INITIALIZER;


extern int at_wpa_passphrase(char *ssid, char *password, unsigned char *buf, int buf_size);

static char * wpa_cli_get_default_ifname(void)
{
	char *ifname = NULL;

#ifdef CONFIG_CTRL_IFACE_UNIX
	struct dirent *dent;
	DIR *dir = opendir(ctrl_iface_dir);
	if (!dir) {
#ifdef ANDROID
		char ifprop[PROPERTY_VALUE_MAX];
		if (property_get("wifi.interface", ifprop, NULL) != 0) {
			ifname = os_strdup(ifprop);
			printf("Using interface '%s'\n", ifname);
			return ifname;
		}
#endif /* ANDROID */
		return NULL;
	}
	while ((dent = readdir(dir))) {
#ifdef _DIRENT_HAVE_D_TYPE
		/*
		 * Skip the file if it is not a socket. Also accept
		 * DT_UNKNOWN (0) in case the C library or underlying
		 * file system does not support d_type.
		 */
		if (dent->d_type != DT_SOCK && dent->d_type != DT_UNKNOWN)
			continue;
#endif /* _DIRENT_HAVE_D_TYPE */
		if (os_strcmp(dent->d_name, ".") == 0 ||
		    os_strcmp(dent->d_name, "..") == 0)
			continue;
		printf("Selected interface '%s'\n", dent->d_name);
		ifname = os_strdup(dent->d_name);
		break;
	}
	closedir(dir);
#endif /* CONFIG_CTRL_IFACE_UNIX */

#ifdef CONFIG_CTRL_IFACE_NAMED_PIPE
	char buf[2048], *pos;
	size_t len;
	struct wpa_ctrl *ctrl;
	int ret;

	ctrl = wpa_ctrl_open(NULL);
	if (ctrl == NULL)
		return NULL;

	len = sizeof(buf) - 1;
	ret = wpa_ctrl_request(ctrl, "INTERFACES", 10, buf, &len, NULL);
	if (ret >= 0) {
		buf[len] = '\0';
		pos = os_strchr(buf, '\n');
		if (pos)
			*pos = '\0';
		ifname = os_strdup(buf);
	}
	wpa_ctrl_close(ctrl);
#endif /* CONFIG_CTRL_IFACE_NAMED_PIPE */

	return ifname;
}

static int wpa_cli_open_connection(const char *ifname, int attach)
{
#if defined(CONFIG_CTRL_IFACE_UDP) || defined(CONFIG_CTRL_IFACE_NAMED_PIPE)
	ctrl_conn = wpa_ctrl_open(ifname);
	if (ctrl_conn == NULL)
		return -1;

	if (attach && interactive)
		mon_conn = wpa_ctrl_open(ifname);
	else
		mon_conn = NULL;
#else /* CONFIG_CTRL_IFACE_UDP || CONFIG_CTRL_IFACE_NAMED_PIPE */
	char *cfile = NULL;
	int flen, res;

	if (ifname == NULL)
		return -1;

#ifdef ANDROID
	if (access(ctrl_iface_dir, F_OK) < 0) {
		cfile = os_strdup(ifname);
		if (cfile == NULL)
			return -1;
	}
#endif /* ANDROID */

	if (cfile == NULL) {
		flen = os_strlen(ctrl_iface_dir) + os_strlen(ifname) + 2;
		cfile = os_malloc(flen);
		if (cfile == NULL)
			return -1;
		res = os_snprintf(cfile, flen, "%s/%s", ctrl_iface_dir,
				  ifname);
		if (res < 0 || res >= flen) {
			os_free(cfile);
			return -1;
		}
	}

	ctrl_conn = wpa_ctrl_open(cfile);
	if (ctrl_conn == NULL) {
		os_free(cfile);
		return -1;
	}

	if (attach && interactive)
		mon_conn = wpa_ctrl_open(cfile);
	else
		mon_conn = NULL;
	os_free(cfile);
#endif /* CONFIG_CTRL_IFACE_UDP || CONFIG_CTRL_IFACE_NAMED_PIPE */

	if (mon_conn) {
		if (wpa_ctrl_attach(mon_conn) == 0) {
			wpa_cli_attached = 1;
#if 0
			if (interactive)
				eloop_register_read_sock(
					wpa_ctrl_get_fd(mon_conn),
					wpa_cli_mon_receive, NULL, NULL);
#endif
		} else {
			printf("Warning: Failed to attach to "
			       "wpa_supplicant.\n");
			return -1;
		}
	}

	return 0;
}


static int wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd, char *reply, size_t size)
{
	int ret;
	char buf[4096] = { 0 };
	size_t len = sizeof(buf) - 1;

	if (ctrl_conn == NULL) {
		printf("Not connected to wpa_supplicant - command dropped:%s.\n", cmd);
		return -1;
	}

	memset(reply, 0, size);
	pthread_mutex_lock(&at_wpa_mutex);
	ret = wpa_ctrl_request(ctrl, cmd, os_strlen(cmd), buf, &len, NULL);
	memcpy(reply, buf, sizeof(buf) > size ? size : sizeof(buf));
	pthread_mutex_unlock(&at_wpa_mutex);
	if (ret == -2) {
		printf("'%s' command timed out.\n", cmd);
		return -2;
	} else if (ret < 0) {
		printf("'%s' command failed.\n", cmd);
		return -1;
	}
	return 0;
}

void wifi_close_connection(void)
{
	if (ctrl_conn == NULL) {
		return;
	}

	if (wpa_cli_attached) {
		wpa_ctrl_detach(interactive ? mon_conn : ctrl_conn);
		wpa_cli_attached = 0;
	}
	wpa_ctrl_close(ctrl_conn);
	ctrl_conn = NULL;
	if (mon_conn) {
		eloop_unregister_read_sock(wpa_ctrl_get_fd(mon_conn));
		wpa_ctrl_close(mon_conn);
		mon_conn = NULL;
	}

	if(ctrl_ifname) {
		os_free(ctrl_ifname);
		ctrl_ifname = NULL;
	}
}


#if 0
static void wifi_reconnect(void)
{
	wifi_close_connection();
	wpa_cli_open_connection(ctrl_ifname, 0);
}
#endif

int wifi_connect(void)
{
	if(ctrl_conn) {
		return 0;
	}
	if (ctrl_ifname == NULL) {
		ctrl_ifname = wpa_cli_get_default_ifname();
	}

	if (wpa_cli_open_connection(ctrl_ifname, 0) < 0) {
		printf("wpa Connection failed.\n");
		return -1;
	}

	return 0;
}

static char *get_line(char *line, char **next_line)
{
	char *ptr;

	if(!line) {
		return NULL;
	}

	ptr = strchr(line, '\n');
	if(ptr) {
		*next_line = ptr + 1;
		*ptr = '\0';
		return line;
	}else {
		*next_line = NULL;
		return line;
	}
}

static int at_wpa_scan(at_wpa_list_t *scan, size_t array_size, size_t *mem_size, int flag)
{
	char buf[4096] = {0};
	int ret;
	int i = 0;
	char *line = buf, *next_line;

	if(flag) {
		struct timespec sec;
		sec.tv_sec = 5;
		sec.tv_nsec = 0;

		ret = wpa_ctrl_command(ctrl_conn, "SCAN", buf, sizeof(buf));
		while (nanosleep(&sec, &sec) < 0);
	}
	ret = wpa_ctrl_command(ctrl_conn, "SCAN_RESULTS", buf, sizeof(buf));
	if(ret < 0) {
		return ret;
	}

	while((line = get_line(line, &next_line)) && i < array_size) {
		char *ptr = line, *ptr_t;
		line = next_line;

		ptr_t = strchr(ptr, '\t');
		if (!ptr_t) {
			continue;
		}
		strncpy(scan[i].bssid, ptr, (ptr_t - ptr) > (sizeof(scan[i].bssid) - 1) ? (sizeof(scan[i].bssid) - 1) : (ptr_t - ptr));
		//////////
		ptr = ptr_t + 1;
		ptr_t = strchr(ptr, '\t');
		if (!ptr_t) {
			continue;
		}
		/////////////
		ptr = ptr_t + 1;
		ptr_t = strchr(ptr, '\t');
		if (!ptr_t) {
			continue;
		}
		{
			char tmp[16] = { 0 };
			strncpy(tmp, ptr, (ptr_t - ptr) > (sizeof(tmp) - 1) ? (sizeof(tmp) - 1) : (ptr_t - ptr));
			scan[i].signal_lev = atoi(tmp) + 256;
		}
		///////////
		ptr = ptr_t + 1;
		ptr_t = strchr(ptr, '\t');
		if (!ptr_t) {
			continue;
		}
		{
			char tmp[64] = { 0 };
			strncpy(tmp, ptr, (ptr_t - ptr) > (sizeof(tmp) - 1) ? (sizeof(tmp) - 1) : (ptr_t - ptr));
			if(strstr(tmp, "WEP")) {
				scan[i].secur_type = WIFI_MODE_WEP;
			}else if(strstr(tmp, "WPA-PSK") && strstr(tmp, "WPA2-PSK")) {
				scan[i].secur_type = WIFI_MODE_WPA_WPA2;
			}else if(strstr(tmp, "WPA-PSK")) {
				scan[i].secur_type = WIFI_MODE_WPA;
			}else if(strstr(tmp, "WPA2-PSK")) {
				scan[i].secur_type = WIFI_MODE_WPA2;
			}else {
				scan[i].secur_type = WIFI_MODE_OPEN;
			}
		}
		///////////
		ptr = ptr_t + 1;
		if(strlen(ptr) <= 0) {
			continue;
		}
		strncpy(scan[i].ssid, ptr, strlen(ptr) > (sizeof(scan[i].ssid) - 1) ? (sizeof(scan[i].ssid) - 1) : strlen(ptr));
		///////////
		scan[i].network_id = -1;
		scan[i].is_current = 0;

		i++;
	}

	*mem_size = i;
	return ret;
}

static int at_wpa_list(at_wpa_list_t *scan, size_t array_size, size_t *mem_size)
{
	char buf[4096] = {0};
	int ret;
	int i = 0;
	char *line = buf, *next_line;

	ret = wpa_ctrl_command(ctrl_conn, "LIST_NETWORKS", buf, sizeof(buf));

	while ((line = get_line(line, &next_line)) && i < array_size) {
		char *ptr = line, *ptr_t;
		line = next_line;

		ptr_t = strchr(ptr, '\t');
		if (!ptr_t) {
			continue;
		}
		{
			char tmp[16] = { 0 };
			strncpy(tmp, ptr, (ptr_t - ptr) > (sizeof(tmp) - 1) ? (sizeof(tmp) - 1) : (ptr_t - ptr));
			scan[i].network_id = atoi(tmp);
		}
		///////////
		ptr = ptr_t + 1;
		ptr_t = strchr(ptr, '\t');
		if (!ptr_t) {
			continue;
		}
		strncpy(scan[i].ssid, ptr, (ptr_t - ptr) > (sizeof(scan[i].ssid) - 1) ? (sizeof(scan[i].ssid) - 1) : (ptr_t - ptr));
		//////////
		ptr = ptr_t + 1;
		ptr_t = strchr(ptr, '\t');
		if (!ptr_t) {
			continue;
		}
		strncpy(scan[i].bssid, ptr, (ptr_t - ptr) > (sizeof(scan[i].bssid) - 1) ? (sizeof(scan[i].bssid) - 1) : (ptr_t - ptr));
		///////////
		ptr = ptr_t + 1;
		if(strcmp(ptr, "[CURRENT]") == 0) {
			scan[i].is_current = 1;
		}else {
			scan[i].is_current = 0;
		}

		i++;
	}

	*mem_size = i;
	return ret;
}

static int merger_list_to_scan(at_wpa_list_t *scan, int size_scan, at_wpa_list_t *ptr)
{
	int i;
	for (i=0; i<size_scan; i++) {
		if(strcmp(scan[i].ssid, ptr->ssid) == 0) {
			if(strcmp(ptr->bssid, "any") == 0) {
				scan[i].network_id = ptr->network_id;
				scan[i].is_current = ptr->is_current;
				return 0;
			}else {
				if(strcmp(scan[i].bssid, ptr->bssid) == 0) {
					scan[i].network_id = ptr->network_id;
					scan[i].is_current = ptr->is_current;
					return 0;
				}
			}
		}
	}
	return -1;
}

static void add_note_to_list(at_wpa_list_t **list, at_wpa_list_t *ptr)
{
	at_wpa_list_t *tmp = *list;
	at_wpa_list_t *next;

	if(*list == NULL) {
		*list = ptr;
		return;
	}
	next = tmp->next;

	if(ptr->is_current) {
		ptr->next = *list;
		*list = ptr;
		return;
	}

	if(!((*list)->is_current) && ptr->signal_lev > (*list)->signal_lev) {
		ptr->next = *list;
		*list = ptr;
		return;
	}

	while(next) {
		if(ptr->signal_lev > next->signal_lev) {
			break;
		}
		tmp = next;
		next = tmp->next;
	}
	ptr->next = next;
	tmp->next = ptr;
}

static at_wpa_list_t *wpa_cli_search(int flag)
{
	int max_scan = 20;
	int max_list = 20;
	int size_scan, size_list;
	int i;
	at_wpa_list_t *head_scan = NULL, *head_list = NULL;

	at_wpa_list_t *ptr;
	ptr = calloc(sizeof(at_wpa_list_t), max_scan + max_list);
	if(!ptr) {
		return NULL;
	}

	if(0 > at_wpa_scan(ptr, max_scan, &size_scan, flag)) {
		goto AERR;
	}
	if(0 > at_wpa_list(ptr + max_scan, max_list, &size_list)) {
		goto AERR;
	}

	for(i=0; i<size_list; i++) {
		if(0 != merger_list_to_scan(ptr, size_scan, ptr + max_scan + i)) {
			add_note_to_list(&head_list, ptr + max_scan + i);
		}
	}

	for(i=0; i<size_scan; i++) {
		add_note_to_list(&head_scan, ptr + i);
	}

	{
		at_wpa_list_t *tmp = head_scan;
		while(tmp && tmp->next) {
			tmp = tmp->next;
		}
		if(tmp) {
			tmp->next = head_list;
		}else {
			head_scan = head_list;
		}
	}

	if(!head_scan) {
		goto AERR;
	}
	head_scan->free_token = ptr;

	return head_scan;

AERR:
	free(ptr);
	return NULL;
}

void wifi_list_free(at_wpa_list_t *list)
{
	if(list) {
		free(list->free_token);
	}
}

at_wpa_list_t *wifi_search(void)
{
	return wpa_cli_search(1);
}
at_wpa_list_t *wifi_update_list(at_wpa_list_t *old_list)
{
	wifi_list_free(old_list);
	return wpa_cli_search(0);
}


int wifi_add(char *bssid, char *ssid, char *passwd)
{
	char buf[4096] = { 0 };
	int ret;
	char cmd[256];
	at_wpa_list_t *net_list;
	int edit_flag = -1;

	if (!ssid || strlen(ssid) == 0) {
		return -1;
	}

	net_list = wifi_update_list(NULL);
	if(net_list) {
		at_wpa_list_t *node = net_list;
		while (node) {
			if(node->network_id >= 0
					&& strcmp(ssid, node->ssid) == 0) {
				edit_flag = node->network_id;
				break;
			}
			node = node->next;
		}
	}
	wifi_list_free(net_list);
	if(edit_flag >= 0) {
		return wifi_edit(edit_flag, bssid, ssid, passwd);
	}

	ret = wpa_ctrl_command(ctrl_conn, "ADD_NETWORK", buf, sizeof(buf));
	if(ret < 0 || strlen(buf) <= 0) {
		return -1;
	}
	ret = atoi(buf);

	if(bssid && strlen(bssid) > 0) {
		os_snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s", ret, "bssid", bssid);
		wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));
	}

	os_snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s \"%s\"", ret, "ssid", ssid);
	wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));

	if(passwd && strlen(passwd) > 0){
		char passphrase[68] = {0};

		os_snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s", ret, "key_mgmt", "WPA-PSK");
		wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));

		at_wpa_passphrase(ssid, passwd, passphrase, sizeof(passphrase));
		os_snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s", ret, "psk", passphrase);
		wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));
	}else {
		os_snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s", ret, "key_mgmt", "NONE");
		wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));
	}

	wpa_ctrl_command(ctrl_conn, "SAVE_CONFIG", buf, sizeof(buf));
	return ret;
}

int wifi_remove(int id)
{
	char buf[4096] = { 0 };
	char cmd[256];
	if(id < 0) {
		return -1;
	}
	os_snprintf(cmd, sizeof(cmd), "REMOVE_NETWORK %d", id);
	wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));
	wpa_ctrl_command(ctrl_conn, "SAVE_CONFIG", buf, sizeof(buf));
	return 0;
}

int wifi_edit(int id, char *bssid, char *ssid, char *passwd)
{
	char buf[4096] = { 0 };
	char cmd[256];

	if(id < 0) {
		return -1;
	}
	if (!ssid || strlen(ssid) == 0) {
		return -1;
	}

	if(bssid && strlen(bssid) > 0) {
		os_snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s", id, "bssid", bssid);
		wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));
	}

	os_snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s \"%s\"", id, "ssid", ssid);
	wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));

	if(passwd && strlen(passwd) > 0) {
		char passphrase[68] = {0};

		os_snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s", id, "key_mgmt", "WPA-PSK");
		wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));

		at_wpa_passphrase(ssid, passwd, passphrase, sizeof(passphrase));
		os_snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s", id, "psk", passphrase);
		wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));
	}else {
		os_snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s", id, "key_mgmt", "NONE");
		wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));
	}

	wpa_ctrl_command(ctrl_conn, "SAVE_CONFIG", buf, sizeof(buf));
	return 0;
}

int wifi_select(int id)
{
	char buf[4096] = { 0 };
	char cmd[256];
	if(id < 0) {
		return -1;
	}
	os_snprintf(cmd, sizeof(cmd), "SELECT_NETWORK %d", id);
	wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));
	return 0;
}

static int wifi_daemon_terminate(void)
{
	char buf[4096] = { 0 };
	return wpa_ctrl_command(ctrl_conn, "TERMINATE", buf, sizeof(buf));
}

int wifi_disable_ap(void)
{
	char buf[4096] = { 0 };
	return wpa_ctrl_command(ctrl_conn, "DISCONNECT", buf, sizeof(buf));
}

int wifi_reconnect_ap(void)
{
	char buf[4096] = { 0 };
	return wpa_ctrl_command(ctrl_conn, "RECONNECT", buf, sizeof(buf));
}

int wifi_get_rssi(void)
{
	char buf[4096] = { 0 };
	char *ptr;

	wpa_ctrl_command(ctrl_conn, "SIGNAL_POLL", buf, sizeof(buf));
	ptr = strchr(buf, '\n');
	if(ptr) {
		*ptr = '\0';
	}
	ptr = strstr(buf, "RSSI=");
	if(!ptr) {
		return -1;
	}
	ptr += 5;
	return atoi(ptr);
}

int wifi_disable(int id)
{
	char buf[4096] = {0};
	char cmd[256];
	if(id < 0) {
		return -1;
	}
	os_snprintf(cmd, sizeof(cmd), "DISABLE_NETWORK %d", id);
	wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));
	return 0;
}

int wifi_get_auth_state(void)
{
	int state = 0;
	static int last_state = 0;
	char buf[4096] = { 0 };

	if(0 == wpa_ctrl_command(ctrl_conn, "GET_AUTH_STATE", buf, sizeof(buf))) {
		state = atoi(buf);
	}else {
		return last_state;
	}

	last_state = state;
	return state;
}

int wifi_up(int is_silent, char *config_file)
{
	char cmd[1024] = {0};

	if(wifi_connect() == 0) {
		return 0;
	}

	snprintf(cmd, sizeof(cmd), "wpa_supplicant -iwlan0 -Dwext -c %s &", config_file);
	system(cmd);

	{
		struct timespec sec;
		sec.tv_sec = 1;
		sec.tv_nsec = 0;

		while (nanosleep(&sec, &sec) < 0);
	}

	if(0 != wifi_connect()) {
		printf("WIFI ERROR:connect wifi daemon error\n");
		return -1;
	}

#if 0
	if(is_silent) {
		wifi_disable_ap();
	}else
#endif
	{
		at_wpa_list_t *list;
		at_wpa_list_t *node;
		char buf[4096] = {0};

		list = wifi_update_list(NULL);

		node = list;
		while (node) {
			if(node->network_id >= 0) {
				os_snprintf(cmd, sizeof(cmd), "ENABLE_NETWORK %d", node->network_id);
				wpa_ctrl_command(ctrl_conn, cmd, buf, sizeof(buf));
			}
			node = node->next;
		}
		wifi_list_free(list);
	}
	return 0;
}

void wifi_down(void)
{
	if(ctrl_conn) {
		wifi_daemon_terminate();
		wifi_close_connection();
	}
}

int wifi_is_up(void)
{
	if(ctrl_conn) {
		return 1;
	}else {
		return 0;
	}
}
