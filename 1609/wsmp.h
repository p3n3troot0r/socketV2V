#ifndef _WSMP_H
#define _WSMP_H

/* WAVE Short Message Protocol - IEEE 1609
 * Copyright (C) 2017 Nicholas Haltmeyer <ginsback@protonmail.com>,
 *                    Duncan Woodbury    <p3n3troot0r@protonmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include <asm/errno.h>
#include <asm/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define WSMP_EID_TX_POWER_USED_80211   4
#define WSMP_EID_2DLOCATION            5
#define WSMP_EID_3DLOCATION            6
#define WSMP_EID_ADVERTISER_ID         7
#define WSMP_EID_PROVIDER_SERV_CONTEXT 8
#define WSMP_EID_IPV6_ADDRESS          9
#define WSMP_EID_SERVICE_PORT         10
#define WSMP_EID_PROVIDER_MAC_ADDRESS 11
#define WSMP_EID_EDCA_PARAMETER_SET   12
#define WSMP_EID_SECONDARY_DNS        13
#define WSMP_EID_GATEWAY_MAC_ADDRESS  14
#define WSMP_EID_CHANNEL_NUMBER_80211 15
#define WSMP_EID_DATA_RATE_80211      16
#define WSMP_EID_REPEAT_RATE          17
#define WSMP_EID_RCPI_THRESHOLD       19
#define WSMP_EID_WSA_COUNT_THRESHOLD  20
#define WSMP_EID_CHANNEL_ACCESS       21
#define WSMP_EID_WSA_COUNT_THRES_INT  22
#define WSMP_EID_CHANNEL_LOAD         23

#define WSMP_MAXSIZE 2302

#define WSMP_WSM_MIN 3
#define WSMP_WSA_MIN 2
#define WSMP_SII_MIN 2
#define WSMP_CII_MIN 5
#define WSMP_WRA_MIN 52 /* 52 with IEX padding */
#define WSMP_IEX_MIN 1

#define WSMP_MAXCOUNT 16383

#define WSMP_P1_PMIN 0x00000000
#define WSMP_P2_PMIN 0x00008000
#define WSMP_P3_PMIN 0x00C00000
#define WSMP_P4_PMIN 0xE0000000

#define WSMP_P1_PMAX 0x0000007F
#define WSMP_P2_PMAX 0x0000BFFF
#define WSMP_P3_PMAX 0x00DFFFFF
#define WSMP_P4_PMAX 0xEFFFFFFF

#define WSMP_P2_PXOR 0x00008000
#define WSMP_P3_PXOR 0x00C00000
#define WSMP_P4_PXOR 0xE0000000

#define WSMP_P1_MAX 0x0000007F
#define WSMP_P2_MAX 0x0000407F
#define WSMP_P3_MAX 0x0020407F
#define WSMP_P4_MAX 0x1020407F

#define WSMP_P2_OFF 0x00007F80
#define WSMP_P3_OFF 0x00BFBF80
#define WSMP_P4_OFF 0xDFDFBF80

#define WSMP_VERSION 3

#define WSMP_WSMP_PREFIX 0x86DD
#define WSMP_IPV6_PREFIX 0x88DD

#define WSMP_STRICT 0
#define WSMP_LAX    1
#define WSMP_LOOSE  2

#define WSMP_ENOSUPPORT EOPNOTSUPP
#define WSMP_EBADMSG    EBADMSG
#define WSMP_EDOMAIN    EINVAL
#define WSMP_EFAULT     EFAULT
#define WSMP_EMODE      EPROTOTYPE
#define WSMP_EDUPLICATE EMLINK

#define WSMP_WSM 0
#define WSMP_WSA 1

/* 1609.3-2016 DATA TYPES */

struct wsmp_ie_raw {
     uint8_t id;
     uint16_t len; /* 0 to 16383 */
     uint8_t *data;
};

struct wsmp_ie_psc {
     uint16_t len; /* 0 to 31 */
     uint8_t psc[31];
};

struct wsmp_ie_edca {
     uint8_t ac_be[4];
     uint8_t ac_bk[4];
     uint8_t ac_vi[4];
     uint8_t ac_vo[4];
};

struct wsmp_ie_2d {
     uint8_t latitude[4];
     uint8_t longitude[4];
};

struct wsmp_ie_3d {
     uint8_t latitude[4];
     uint8_t longitude[4];
     uint8_t elevation[2];
};

struct wsmp_ie_advert_id {
     uint16_t len; /* 0 to 32 */
     uint8_t id[32];
};

struct wsmp_iex {
     uint16_t count; /* 1 to 255 */

     /* WSM Elements */
     uint8_t chan;      /* 0 to 200 */
     uint8_t data_rate; /* 2 to 127 */
     int8_t tx_pow;     /* -128 to 127 */

     /* SII Elements */
     struct wsmp_ie_psc psc;
     uint8_t ip[16];
     uint8_t port[2];
     uint8_t mac[6];
     int8_t rcpi_thres;       /* -110 to 0 */
     uint8_t count_thres;
     uint8_t count_thres_int; /* 1 to 255 */

     /* CII Elements */
     struct wsmp_ie_edca edca;
     uint8_t chan_access; /* 0 to 2 */

     /* WSA Elements */
     uint8_t repeat_rate;
     struct wsmp_ie_2d loc_2d;
     struct wsmp_ie_3d loc_3d;
     struct wsmp_ie_advert_id advert_id;

     /* WRA Elements */
     uint8_t sec_dns[16];
     uint8_t gateway_mac[6];

     bool use[WSMP_EID_CHANNEL_LOAD+1];

     /* Raw elements not defined in 1609.3-2016 */
     uint16_t raw_count;
     struct wsmp_ie_raw *raw;
};

struct wsmp_wsm {
     /* N-Header */
     uint8_t subtype; /* 0 to 15 */
     uint8_t version; /* WSMP_VERSION */
     uint8_t tpid;    /* 0 to 5 */

     bool use_n_iex;
     struct wsmp_iex *n_iex;

     /* T-Header */
     union {
	  uint32_t psid; /* 0x0 to 0xEFFFFFFF */

	  struct {
	       uint8_t src[2];
	       uint8_t dst[2];
	  } ports;
     };

     bool use_t_iex;
     struct wsmp_iex *t_iex;

     uint16_t len; /* 0 to 16383 */
     uint8_t *data;
};

struct wsmp_sii {
     uint32_t psid;
     uint8_t chan_index; /* 0 to 31 */

     bool use_iex;
     struct wsmp_iex *iex;
};

struct wsmp_cii {
     uint8_t op_class;  /* TODO: 802.11 */
     uint8_t chan;      /* TODO: 802.11 */
     int8_t tx_pow;     /* -128 to 127 */
     uint8_t adapt;     /* 0 to 1 */
     uint8_t data_rate; /* 0x02 to 0x7F */

     bool use_iex;
     struct wsmp_iex *iex;
};

struct wsmp_wra {
     uint8_t lifetime[2];   /* TODO: IETF RFC 4861 */
     uint8_t ip_prefix[16]; /* TODO: IETF RFC 4861 */
     uint8_t prefix_len;    /* TODO: IETF RFC 4861 */
     uint8_t gateway[16];
     uint8_t dns[16];

     bool use_iex;
     struct wsmp_iex *iex;
};

struct wsmp_wsa {
     uint8_t version;       /* 0 to 15 */
     uint8_t id;            /* 0 to 15 */
     uint8_t content_count; /* 0 to 15 */

     bool use_iex;
     struct wsmp_iex *iex;

     uint16_t sii_count; /* 0 to 31 */
     struct wsmp_sii **sis;

     uint16_t cii_count; /* 0 to 31 */
     struct wsmp_cii **cis;

     bool use_wra;
     struct wsmp_wra *wra;
};

void free_iex(struct wsmp_iex *curs);
void free_sii(struct wsmp_sii *curs);
void free_cii(struct wsmp_cii *curs);
void free_wra(struct wsmp_wra *curs);
void free_wsa(struct wsmp_wsa *curs);
void free_wsm(struct wsmp_wsm *curs);

uint8_t c_count(uint8_t c);

uint8_t p_count(uint8_t p_c);
uint32_t p_to_hex(uint32_t psid, uint8_t *len);
uint32_t hex_to_p(uint32_t psid, uint8_t *len);

void _g(const uint8_t *buf, size_t *i, uint8_t *v, size_t len, int *err);
void _gs(const uint8_t *buf, size_t *i, int8_t *v, size_t len, int *err);
void _g_n(const uint8_t *buf, size_t *i, size_t n, uint8_t *v, size_t len, int *err);
void _g_p(const uint8_t *buf, size_t *i, uint32_t *v, size_t len, int *err);
void _g_c(const uint8_t *buf, size_t *i, uint16_t *v, size_t len, int *err);

void _s(uint8_t *buf, size_t *i, const uint8_t v, size_t len, int *err);
void _s_n(uint8_t *buf, size_t *i, size_t n, const uint8_t *v, size_t len, int *err);
void _s_p(uint8_t *buf, size_t *i, const uint32_t v, size_t len, int *err);
void _s_c(uint8_t *buf, size_t *i, const uint16_t v, size_t len, int *err);

/* int wsmp_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg); */
/* int wsmp_send(struct sk_buff *skb, int loop); */
/* int wsmp_receive(struct sk_buff *skb, struct net_device *dev); */

#endif /* _WSMP_COMMON_H */
