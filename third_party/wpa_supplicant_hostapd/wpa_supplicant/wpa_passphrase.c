/*
 * WPA Supplicant - ASCII passphrase to WPA PSK tool
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
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

#include "common.h"
#include "crypto/sha1.h"

#if 1
int at_wpa_passphrase(char *ssid, char *password, unsigned char *buf, int buf_size)
{
	unsigned char psk[32];
	int i;
	if(!password || !ssid
			|| (password && os_strlen(password)<=0)) {
		memset(buf, 0, buf_size);
		return 0;
	}

	if (os_strlen(password) > 63) {
		printf("Passphrase must be 1..63 characters\n");
		return -1;
	}

	pbkdf2_sha1(password, ssid, os_strlen(ssid), 4096, psk, 32);
	for (i = 0; i < 32; i++) {
		char tmp[4] = {0};
		snprintf(tmp, sizeof(tmp),"%02x", psk[i]);
		buf[i * 2] = tmp[0];
		buf[i * 2 + 1] = tmp[1];
	}
	return 0;
}

#else
int main(int argc, char *argv[])
{
	unsigned char psk[32];
	int i;
	char *ssid, *passphrase, buf[64], *pos;

	if (argc < 2) {
		printf("usage: wpa_passphrase <ssid> [passphrase]\n"
			"\nIf passphrase is left out, it will be read from "
			"stdin\n");
		return 1;
	}

	ssid = argv[1];

	if (argc > 2) {
		passphrase = argv[2];
	} else {
		printf("# reading passphrase from stdin\n");
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			printf("Failed to read passphrase\n");
			return 1;
		}
		buf[sizeof(buf) - 1] = '\0';
		pos = buf;
		while (*pos != '\0') {
			if (*pos == '\r' || *pos == '\n') {
				*pos = '\0';
				break;
			}
			pos++;
		}
		passphrase = buf;
	}

	if (os_strlen(passphrase) < 8 || os_strlen(passphrase) > 63) {
		printf("Passphrase must be 8..63 characters\n");
		return 1;
	}

	pbkdf2_sha1(passphrase, ssid, os_strlen(ssid), 4096, psk, 32);

	printf("network={\n");
	printf("\tssid=\"%s\"\n", ssid);
	printf("\t#psk=\"%s\"\n", passphrase);
	printf("\tpsk=");
	for (i = 0; i < 32; i++)
		printf("%02x", psk[i]);
	printf("\n");
	printf("}\n");

	return 0;
}
#endif
