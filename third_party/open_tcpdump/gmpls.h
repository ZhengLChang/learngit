/* @(#) $Header: /tcpdump/master/tcpdump/gmpls.h,v 1.5 2006-04-14 07:11:59 hannes Exp $ (LBL) */
/* 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code
 * distributions retain the above copyright notice and this paragraph
 * in its entirety, and (2) distributions including binary code include
 * the above copyright notice and this paragraph in its entirety in
 * the documentation or other materials provided with the distribution.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND
 * WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT
 * LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE.
 *
 * Original code by Hannes Gredler (hannes@juniper.net)
 */

#define GMPLS_PSC1   1
#define GMPLS_PSC2   2
#define GMPLS_PSC3   3
#define GMPLS_PSC4   4
#define GMPLS_L2SC  51
#define GMPLS_TSC  100
#define GMPLS_LSC  150
#define GMPLS_FSC  200

extern struct tok gmpls_link_prot_values[];
extern struct tok gmpls_switch_cap_values[];
extern struct tok gmpls_switch_cap_tsc_indication_values[];
extern struct tok gmpls_encoding_values[];
extern struct tok gmpls_payload_values[];
extern struct tok diffserv_te_bc_values[];
extern struct tok lmp_sd_service_config_cpsa_link_type_values[];
extern struct tok lmp_sd_service_config_cpsa_signal_type_sdh_values[];
extern struct tok lmp_sd_service_config_cpsa_signal_type_sonet_values[];
