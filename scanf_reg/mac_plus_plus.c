#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DEC_TEST (0x10C)
int main(void)
{
	char mac[] = "00:0C:29:54:E2:F4";
	unsigned mac_calc[6] = {0};
	int count;
	for(; ; )
	{
		count = sscanf(mac, "%x:%x:%x:%x:%x:%x", 
				&mac_calc[5], &mac_calc[4], &mac_calc[3], &mac_calc[2], &mac_calc[1], &mac_calc[0]);
		/*
		printf("%#x\t", mac_calc[5]);
		printf("%#x\t", mac_calc[4]);
		printf("%#x\t", mac_calc[3]);
		printf("%#x\t", mac_calc[2]);
		printf("%#x\t", mac_calc[1]);
		printf("%#x\n", mac_calc[0]);
		*/
		if(count != 6)
		{
			printf("count = %d\n", count);
			printf("read mac error\n");
			return -1;
		}
		mac_calc[0]++;
		mac_calc[1] += ((mac_calc[0] & (0x100))  >> 8);
		mac_calc[0] = mac_calc[0] & (0xff);
		mac_calc[2] += ((mac_calc[1] & (0x100))  >> 8);
		mac_calc[1] = mac_calc[1] & (0xff);
		mac_calc[3] += ((mac_calc[2] & (0x100))  >> 8);
		mac_calc[2] = mac_calc[2] & (0xff);
		mac_calc[4] += ((mac_calc[3] & (0x100))  >> 8);
		mac_calc[3] = mac_calc[3] & (0xff);
		mac_calc[5] += ((mac_calc[4] & (0x100))  >> 8);
		mac_calc[4] = mac_calc[4] & (0xff);
		if(mac_calc[5] > 0xff)
		{
			printf("%#x invalid mac\n", mac_calc[5]);
			break;
		}
		sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", 
				mac_calc[5], mac_calc[4],mac_calc[3],mac_calc[2],mac_calc[1],mac_calc[0]);
		printf("now mac: %s\n", mac);
		//sleep(1);

	}
	return 0;
}
