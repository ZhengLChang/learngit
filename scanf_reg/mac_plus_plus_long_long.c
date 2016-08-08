#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	char mac[] = "00:0C:29:54:E2:F4";
	long long mac_space = 0;
	int i = 0, count = strlen(mac);
	if(sizeof(long long) < 6)
	{
		printf("long long is too short\n");
		return -1;
	}
	while(1)
	{
	for(i = 0, mac_space = 0; i < strlen(mac); i++)
	{
		if(mac[i] >= '0' && mac[i] <= '9')
		{
			mac_space = mac_space << 4;
			mac_space += mac[i] - '0';
		}
		else if(mac[i] >= 'a' && mac[i] <= 'f')
		{
			mac_space = mac_space << 4;
			mac_space += mac[i] - 'a' + 10;
		}
		else if(mac[i] >= 'A' && mac[i] <= 'F')
		{
			mac_space = mac_space << 4;
			mac_space += mac[i] - 'A' + 10;
		}
		else if(mac[i] == ':' && (i + 1) % 3 == 0)
		{
			continue;
		}
		else
		{
			printf("format error\n");
			return -1;
		}
	}
	mac_space++;
	sprintf(mac, "%012llx", mac_space);
	for(i = 0; i < 12 - 2; i++)
	{
		mac[count - 1 - i - (i)/ 2] = mac[11 - i];
		if(i != 0 && (i) % 2 == 0)
		{
			mac[count - i - (i)/ 2] = ':';
		}
	}
	mac[count - i - (i)/ 2] = ':';
	printf("now mac = %s\n", mac);
	sleep(1);
	}
	return 0;
}










