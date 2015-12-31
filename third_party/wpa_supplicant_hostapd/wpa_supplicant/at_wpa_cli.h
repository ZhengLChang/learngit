#ifndef __AT_WPA_CLI_H__
#define __AT_WPA_CLI_H__

typedef enum {
	WIFI_MODE_OPEN = 1,
	WIFI_MODE_WEP,
	WIFI_MODE_WPA,
	WIFI_MODE_WPA2,
	WIFI_MODE_WPA_WPA2,
}at_wifi_secure_mode;


typedef struct _at_wpa_list{
	struct _at_wpa_list *next;
	int network_id;
	char bssid[20];
	char ssid[128];
	at_wifi_secure_mode secur_type;
	int signal_lev;
	int is_current;
	void *free_token;
}at_wpa_list_t;


int wifi_up(int is_silent, char *config_file);
void wifi_down(void);
int wifi_is_up(void);

void wifi_close_connection(void);
int wifi_connect(void);

at_wpa_list_t *wifi_search(void);
at_wpa_list_t *wifi_update_list(at_wpa_list_t *old_list);
void wifi_list_free(at_wpa_list_t *list);

int wifi_add(char *bssid, char *ssid, char *passwd);
int wifi_remove(int id);
int wifi_edit(int id, char *bssid, char *ssid, char *passwd);
int wifi_select(int id);
int wifi_disable(int id);
int wifi_disable_ap(void);
int wifi_reconnect_ap(void);

int wifi_get_rssi(void);

int wifi_get_auth_state(void);

#endif

