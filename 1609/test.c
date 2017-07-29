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

#include "wsmp.h"
#include "encode.h"
#include "decode.h"
#include <time.h>
#include <stdlib.h>
#include <assert.h>

int randr(int a, int b) {
     /* X ~ U[a, b] */
     assert(a < b);

     int n = rand() % (b+1-a);

     return n + a;
}

int urandr(unsigned int a, unsigned int b) {
     /* X ~ U[a, b] */
     assert(a < b);

     unsigned int n = rand() % (b+1-a);

     return n + a;
}

void print_iex(struct wsmp_iex *iex) {
     printf("\nBEGIN IEX\n");
     printf("count:            %d\n", iex->count);
     printf("chan:             %d\n", iex->chan);
     printf("data rate:        %d\n", iex->data_rate);
     printf("tx_pow:           %d\n", iex->tx_pow);
     printf("psc.len:          %d\n", iex->psc.len);
     printf("psc.data:\n");

     int i;
     for (i = 0; i < iex->psc.len; i++)
          printf("%02x", iex->psc.psc[i]);

     printf("\n");
     printf("ip:               ");

     for (i = 0; i < 16; i++)
          printf("%02x", iex->ip[i]);

     printf("\n");
     printf("port:             %02x%02x\n",
            iex->port[0], iex->port[1]);
     printf("mac               %02x%02x%02x%02x%02x%02x\n",
            iex->mac[0], iex->mac[1], iex->mac[2],
            iex->mac[3], iex->mac[4], iex->mac[5]);
     printf("rcpi_thres:       %d\n", iex->rcpi_thres);
     printf("count_thres:      %d\n", iex->count_thres);
     printf("count_thres_int:  %d\n", iex->count_thres_int);
     printf("edca.ac_be:       %02x%02x%02x%02x\n",
            iex->edca.ac_be[0], iex->edca.ac_be[1],
            iex->edca.ac_be[2], iex->edca.ac_be[3]);
     printf("edca.ac_bk:       %02x%02x%02x%02x\n",
            iex->edca.ac_bk[0], iex->edca.ac_bk[1],
            iex->edca.ac_bk[2], iex->edca.ac_bk[3]);
     printf("edca.ac_vi:       %02x%02x%02x%02x\n",
            iex->edca.ac_vi[0], iex->edca.ac_vi[1],
            iex->edca.ac_vi[2], iex->edca.ac_vi[3]);
     printf("edca.ac_vo:       %02x%02x%02x%02x\n",
            iex->edca.ac_vo[0], iex->edca.ac_vo[1],
            iex->edca.ac_vo[2], iex->edca.ac_vo[3]);
     printf("chan_access:      %d\n", iex->chan_access);
     printf("repeat_rate:      %d\n", iex->repeat_rate);
     printf("loc_2d.latitude:  %02x%02x%02x%02x\n",
            iex->loc_2d.latitude[0], iex->loc_2d.latitude[1],
            iex->loc_2d.latitude[2], iex->loc_2d.latitude[3]);
     printf("loc_2d.longitude: %02x%02x%02x%02x\n",
            iex->loc_2d.longitude[0], iex->loc_2d.longitude[1],
            iex->loc_2d.longitude[2], iex->loc_2d.longitude[3]);
     printf("loc_3d.latitude:  %02x%02x%02x%02x\n",
            iex->loc_3d.latitude[0], iex->loc_3d.latitude[1],
            iex->loc_3d.latitude[2], iex->loc_3d.latitude[3]);
     printf("loc_3d.longitude: %02x%02x%02x%02x\n",
            iex->loc_3d.longitude[0], iex->loc_3d.longitude[1],
            iex->loc_3d.longitude[2], iex->loc_3d.longitude[3]);
     printf("loc_3d.elevation: %02x%02x\n", iex->loc_3d.elevation[0],
            iex->loc_3d.elevation[1]);
     printf("advert_id.len:    %d\n", iex->advert_id.len);

     for (i = 0; i < iex->advert_id.len; i++)
          printf("%02x", iex->advert_id.id[i]);

     printf("\n");
     printf("sec_dns:          ");

     for (i = 0; i < 16; i++)
          printf("%02x", iex->sec_dns[i]);

     printf("\n");
     printf("gateway_mac:      %02x%02x%02x%02x%02x%02x\n",
            iex->gateway_mac[0], iex->gateway_mac[1],
            iex->gateway_mac[2], iex->gateway_mac[3],
            iex->gateway_mac[4], iex->gateway_mac[5]);
     printf("\n");
     printf("raw_count:        %d\n", iex->raw_count);
     printf("raw:\n");

     int j;
     for (i = 0; i < iex->raw_count; i++) {
          printf("raw[%d].id:        %d\n", i, iex->raw[i].id);
          printf("raw[%d].len:       %d\n", i, iex->raw[i].len);
          printf("raw[%d].data:\n", i);

          for (j = 0; j < iex->raw[i].len; j++)
               printf("%02x ", iex->raw[i].data[j]);

          printf("\n");
     }

     printf("in_use:\n");
     for (i = 0; i < WSMP_EID_CHANNEL_LOAD+1; i++)
          printf("        (%d, %d)\n", i, iex->use[i]);

     printf("\nEND IEX\n");
}

void print_wsm(struct wsmp_wsm *wsm) {
     printf("\nBEGIN WSM\n");
     printf("subtype:   %d\n", wsm->subtype);
     printf("version:   %d\n", wsm->version);
     printf("tpid:      %d\n", wsm->tpid);
     printf("use_n_iex: %d\n", wsm->use_n_iex);

     if (wsm->use_n_iex) {
          printf("n_iex:\n");
          print_iex(wsm->n_iex);
          printf("\n");
     }

     printf("psid:      %08x\n", wsm->psid);
     printf("ports.src: %02x%02x\n", wsm->ports.src[0], wsm->ports.src[1]);
     printf("ports.dst: %02x%02x\n", wsm->ports.dst[0], wsm->ports.dst[1]);
     printf("use_t_iex: %d\n", wsm->use_t_iex);

     if (wsm->use_t_iex) {
          printf("t_iex:\n");
          print_iex(wsm->t_iex);
          printf("\n");
     }

     printf("length:    %d\n", wsm->len);
     printf("data:\n");

     int i;
     for (i = 0; i < wsm->len; i++)
          printf("%02x ", wsm->data[i]);

     printf("\nEND WSM\n");
}

void print_sii(struct wsmp_sii *sii) {
     printf("\nBEGIN SII\n");
     printf("psid:       %08x\n", sii->psid);
     printf("chan_index: %d\n", sii->chan_index);
     printf("use_iex:    %d\n", sii->use_iex);

     if (sii->use_iex) {
          printf("iex:\n");
          print_iex(sii->iex);
          printf("\n");
     }

     printf("\nEND SII\n");
}

void print_cii(struct wsmp_cii *cii) {
     printf("\nBEGIN CII\n");
     printf("op_class:  %d\n", cii->op_class);
     printf("chan:      %d\n", cii->chan);
     printf("tx_pow:    %d\n", cii->tx_pow);
     printf("adapt:     %d\n", cii->adapt);
     printf("data_rate: %d\n", cii->data_rate);
     printf("use_iex:   %d\n", cii->use_iex);

     if (cii->use_iex) {
          printf("iex:\n");
          print_iex(cii->iex);
          printf("\n");
     }

     printf("\nEND CII\n");
}

void print_wra(struct wsmp_wra *wra) {
     printf("\nBEGIN WRA\n");
     printf("lifetime:    %02x%02x\n", wra->lifetime[0], wra->lifetime[1]);
     printf("ip_prefix:   ");

     int i;
     for (i = 0; i < 16; i++)
          printf("%02x", wra->ip_prefix[i]);

     printf("\n");
     printf("prefix_len: %d\n", wra->prefix_len);
     printf("gateway:    ");

     for (i = 0; i < 16; i++)
          printf("%02x", wra->gateway[i]);

     printf("\n");
     printf("dns:    ");

     for (i = 0; i < 16; i++)
          printf("%02x", wra->dns[i]);

     printf("\n");
     printf("use_iex:   %d\n", wra->use_iex);

     if (wra->use_iex) {
          printf("iex:\n");
          print_iex(wra->iex);
          printf("\n");
     }

     printf("\nEND WRA\n");
}

void print_wsa(struct wsmp_wsa *wsa) {
     printf("\nBEGIN WSA\n");
     printf("version:       %d\n", wsa->version);
     printf("id:            %d\n", wsa->id);
     printf("content_count: %d\n", wsa->content_count);
     printf("use_iex:       %d\n", wsa->use_iex);

     if (wsa->use_iex) {
          printf("iex:\n");
          print_iex(wsa->iex);
          printf("\n");
     }

     printf("sii_count:    %d\n", wsa->sii_count);

     int i;

     if (wsa->sii_count > 0) {
          printf("sis:\n");
          for (i = 0; i < wsa->sii_count; i++) {
               printf("sis[%d]:\n", i);
               print_sii(wsa->sis[i]);
               printf("\n");
          }
     }

     printf("cii_count:    %d\n", wsa->cii_count);
     if (wsa->cii_count > 0) {
          printf("cis:\n");
          for (i = 0; i < wsa->cii_count; i++) {
               printf("cis[%d]:\n", i);
               print_cii(wsa->cis[i]);
               printf("\n");
          }
     }

     printf("use_wra      %d\n", wsa->use_wra);
     if (wsa->use_wra) {
          printf("wra:\n");
          print_wra(wsa->wra);
          printf("\n");
     }

     printf("\nEND WSA\n");
}

struct wsmp_iex *gen_iex(int mode) {
     struct wsmp_iex *iex = calloc(1, sizeof(struct wsmp_iex));
     int i, j;

     if (urandr(0, 1)) {
          iex->chan = mode != WSMP_LOOSE ? urandr(0, 200) : urandr(0, 255);
          iex->count++;
          iex->use[WSMP_EID_CHANNEL_NUMBER_80211] = 1;
     }

     if (urandr(0, 1)) {
          iex->data_rate = mode != WSMP_LOOSE ? urandr(2, 127) : urandr(0, 255);
          iex->count++;
          iex->use[WSMP_EID_DATA_RATE_80211] = 1;
     }

     if (urandr(0, 1)) {
          iex->tx_pow = randr(-128, 127);
          iex->count++;
          iex->use[WSMP_EID_TX_POWER_USED_80211] = 1;
     }

     if (urandr(0, 1)) {
          iex->psc.len = mode != WSMP_LOOSE ? urandr(1, 31) : urandr(0, 31);
          iex->count++;
          iex->use[WSMP_EID_PROVIDER_SERV_CONTEXT] = 1;

          for (i = 0; i < iex->psc.len; i++)
               iex->psc.psc[i] = urandr(0, 255);
     }

     if (urandr(0, 1)) {
          for (i = 0; i < 16; i++)
               iex->ip[i] = urandr(0, 255);

          iex->count++;
          iex->use[WSMP_EID_IPV6_ADDRESS] = 1;
     }

     if (urandr(0, 1)) {
          iex->port[0] = urandr(0, 255);
          iex->port[1] = urandr(0, 255);
          iex->count++;
          iex->use[WSMP_EID_SERVICE_PORT] = 1;
     }

     if (urandr(0, 1)) {
          iex->mac[0] = urandr(0, 255);
          iex->mac[1] = urandr(0, 255);
          iex->mac[2] = urandr(0, 255);
          iex->mac[3] = urandr(0, 255);
          iex->mac[4] = urandr(0, 255);
          iex->mac[5] = urandr(0, 255);
          iex->count++;
          iex->use[WSMP_EID_PROVIDER_MAC_ADDRESS] = 1;
     }

     if (urandr(0, 1)) {
          iex->rcpi_thres = mode != WSMP_LOOSE ? randr(-110, 0) : randr(-128, 127);
          iex->count++;
          iex->use[WSMP_EID_RCPI_THRESHOLD] = 1;
     }

     if (urandr(0, 1)) {
          iex->count_thres = urandr(0, 255);
          iex->count++;
          iex->use[WSMP_EID_WSA_COUNT_THRESHOLD] = 1;
     }

     if (urandr(0, 1)) {
          iex->count_thres_int = mode != WSMP_LOOSE ? urandr(1, 255) : urandr(0, 255);
          iex->count++;
          iex->use[WSMP_EID_WSA_COUNT_THRES_INT] = 1;
     }

     if (urandr(0, 1)) {
          iex->edca.ac_be[0] = urandr(0, 255);
          iex->edca.ac_be[1] = urandr(0, 255);
          iex->edca.ac_be[2] = urandr(0, 255);
          iex->edca.ac_be[3] = urandr(0, 255);
          iex->edca.ac_bk[0] = urandr(0, 255);
          iex->edca.ac_bk[1] = urandr(0, 255);
          iex->edca.ac_bk[2] = urandr(0, 255);
          iex->edca.ac_bk[3] = urandr(0, 255);
          iex->edca.ac_vi[0] = urandr(0, 255);
          iex->edca.ac_vi[1] = urandr(0, 255);
          iex->edca.ac_vi[2] = urandr(0, 255);
          iex->edca.ac_vi[3] = urandr(0, 255);
          iex->edca.ac_vo[0] = urandr(0, 255);
          iex->edca.ac_vo[1] = urandr(0, 255);
          iex->edca.ac_vo[2] = urandr(0, 255);
          iex->edca.ac_vo[3] = urandr(0, 255);
          iex->count++;
          iex->use[WSMP_EID_EDCA_PARAMETER_SET] = 1;
     }

     if (urandr(0, 1)) {
          iex->chan_access = mode != WSMP_LOOSE ? urandr(0, 2) : urandr(0, 255);
          iex->count++;
          iex->use[WSMP_EID_CHANNEL_ACCESS] = 1;
     }

     if (urandr(0, 1)) {
          iex->repeat_rate = urandr(0, 255);
          iex->count++;
          iex->use[WSMP_EID_REPEAT_RATE] = 1;
     }

     if (urandr(0, 1)) {
          iex->loc_2d.latitude[0] = urandr(0, 255);
          iex->loc_2d.latitude[1] = urandr(0, 255);
          iex->loc_2d.latitude[2] = urandr(0, 255);
          iex->loc_2d.latitude[3] = urandr(0, 255);
          iex->loc_2d.longitude[0] = urandr(0, 255);
          iex->loc_2d.longitude[1] = urandr(0, 255);
          iex->loc_2d.longitude[2] = urandr(0, 255);
          iex->loc_2d.longitude[3] = urandr(0, 255);
          iex->count++;
          iex->use[WSMP_EID_2DLOCATION] = 1;
     }

     if (urandr(0, 1)) {
          iex->loc_3d.latitude[0] = urandr(0, 255);
          iex->loc_3d.latitude[1] = urandr(0, 255);
          iex->loc_3d.latitude[2] = urandr(0, 255);
          iex->loc_3d.latitude[3] = urandr(0, 255);
          iex->loc_3d.longitude[0] = urandr(0, 255);
          iex->loc_3d.longitude[1] = urandr(0, 255);
          iex->loc_3d.longitude[2] = urandr(0, 255);
          iex->loc_3d.longitude[3] = urandr(0, 255);
          iex->loc_3d.elevation[0] = urandr(0, 255);
          iex->loc_3d.elevation[1] = urandr(0, 255);
          iex->count++;
          iex->use[WSMP_EID_3DLOCATION] = 1;
     }

     if (urandr(0, 1)) {
          iex->advert_id.len = mode != WSMP_LOOSE ? urandr(1, 32) : urandr(0, 32);
          iex->count++;
          iex->use[WSMP_EID_ADVERTISER_ID] = 1;

          for (i = 0; i < iex->advert_id.len; i++)
               iex->advert_id.id[i] = urandr(0, 255);
     }

     if (urandr(0, 1)) {
          for (i = 0; i < 16; i++)
               iex->sec_dns[i] = urandr(0, 255);

          iex->count++;
          iex->use[WSMP_EID_SECONDARY_DNS] = 1;
     }

     if (urandr(0, 1)) {
          iex->gateway_mac[0] = urandr(0, 255);
          iex->gateway_mac[1] = urandr(0, 255);
          iex->gateway_mac[2] = urandr(0, 255);
          iex->gateway_mac[3] = urandr(0, 255);
          iex->gateway_mac[4] = urandr(0, 255);
          iex->gateway_mac[5] = urandr(0, 255);
          iex->count++;
          iex->use[WSMP_EID_GATEWAY_MAC_ADDRESS] = 1;
     }

     iex->raw_count = urandr(0, 2);
     iex->count += iex->raw_count;
     iex->raw = calloc(iex->raw_count, sizeof(struct wsmp_ie_raw));

     for (i = 0; i < iex->raw_count; i++) {
          iex->raw[i].id = urandr(WSMP_EID_CHANNEL_LOAD+1, 255);
          iex->raw[i].len = mode == WSMP_STRICT ? urandr(1, 50) : urandr(0, 50);
          iex->raw[i].data = calloc(iex->raw[i].len, 1);

          for (j = 0; j < iex->raw[i].len; j++)
               iex->raw[i].data[j] = urandr(0, 255);
     }

     if (iex->count == 0) {
          free_iex(iex);
          return gen_iex(mode);
     }

     return iex;
}

struct wsmp_sii *gen_sii(int mode) {
     struct wsmp_sii *sii = calloc(1, sizeof(struct wsmp_sii));

     sii->psid = urandr(0, WSMP_P4_MAX);
     sii->chan_index = mode != WSMP_LOOSE ? urandr(0, 31) : urandr(0, 255);

     if (urandr(0, 1)) {
          sii->use_iex = 1;

          struct wsmp_iex *iex = gen_iex(mode);
          sii->iex = iex;
     }

     return sii;
}

struct wsmp_cii *gen_cii(int mode) {
     struct wsmp_cii *cii = calloc(1, sizeof(struct wsmp_cii));

     cii->op_class = urandr(0, 255);
     cii->chan = urandr(0, 255);
     cii->tx_pow = randr(-128, 127);
     cii->adapt = urandr(0, 1);
     cii->data_rate = mode != WSMP_LOOSE ? urandr(0x02, 0x7F) : urandr(0, 255);

     if (urandr(0, 1)) {
          cii->use_iex = 1;

          struct wsmp_iex *iex = gen_iex(mode);
          cii->iex = iex;
     }

     return cii;
}

struct wsmp_wra *gen_wra(int mode) {
     struct wsmp_wra *wra = calloc(1, sizeof(struct wsmp_wra));

     wra->lifetime[0] = urandr(0, 255);
     wra->lifetime[1] = urandr(0, 255);

     int i;
     for (i = 0; i < 16; i++)
          wra->ip_prefix[i] = urandr(0, 255);

     wra->prefix_len = urandr(0, 255);

     for (i = 0; i < 16; i++) {
          wra->gateway[i] = urandr(0, 255);
          wra->dns[i] = urandr(0, 255);
     }

     if (urandr(0, 1)) {
          wra->use_iex = 1;

          struct wsmp_iex *iex = gen_iex(mode);
          wra->iex = iex;
     }

     return wra;
}

struct wsmp_wsa *gen_wsa(int mode) {
     struct wsmp_wsa *wsa = calloc(1, sizeof(struct wsmp_wsa));

     wsa->version = WSMP_VERSION;
     wsa->id = urandr(0, 15);
     wsa->content_count = urandr(0, 15);

     if (urandr(0, 1)) {
          wsa->use_iex = 1;

          struct wsmp_iex *iex = gen_iex(mode);
          wsa->iex = iex;
     }

     int i;

     wsa->sii_count = urandr(0, 2);
     if (wsa->sii_count > 0) {
          wsa->sis = calloc(wsa->sii_count, sizeof(struct wsmp_sii));

          for (i = 0; i < wsa->sii_count; i++) {
               struct wsmp_sii *sii = gen_sii(mode);
               wsa->sis[i] = sii;
          }
     }

     wsa->cii_count = urandr(0, 2);
     if (wsa->cii_count > 0) {
          wsa->cis = calloc(wsa->cii_count, sizeof(struct wsmp_cii));

          for (i = 0; i < wsa->cii_count; i++) {
               struct wsmp_cii *cii = gen_cii(mode);
               wsa->cis[i] = cii;
          }
     }

     if (urandr(0, 1)) {
          wsa->use_wra = 1;

          struct wsmp_wra *wra = gen_wra(mode);
          wsa->wra = wra;
     }

     return wsa;
}

struct wsmp_wsm *gen_wsm(int mode) {
     struct wsmp_wsm *wsm = calloc(1, sizeof(struct wsmp_wsm));

     wsm->subtype = 0;
     wsm->version = WSMP_VERSION;
     wsm->tpid = urandr(0, 5);

     if (urandr(0, 1)) {
          wsm->use_n_iex = 1;

          struct wsmp_iex *n_iex = gen_iex(mode);
          wsm->n_iex = n_iex;
     }

     if (wsm->tpid < 2) {
          wsm->psid = urandr(0, WSMP_P4_MAX);
     } else {
          wsm->ports.src[0] = urandr(0, 255);
          wsm->ports.src[1] = urandr(0, 255);
          wsm->ports.dst[0] = urandr(0, 255);
          wsm->ports.dst[1] = urandr(0, 255);
     }

     if (wsm->tpid % 2 == 1) {
          wsm->use_t_iex = 1;

          struct wsmp_iex *t_iex = gen_iex(mode);
          wsm->t_iex = t_iex;
     }

     wsm->len = mode == WSMP_STRICT ? urandr(1, 255) : urandr(0, 255);
     wsm->data = calloc(wsm->len, 1);

     int i;
     for (i = 0; i < wsm->len; i++)
          wsm->data[i] = urandr(0, 255);

     return wsm;
}

uint8_t *gen_bytes(size_t *len) {
     *len = urandr(1, 128);
     uint8_t *ret = calloc(*len, 1);

     int i;
     for (i = 0; i < *len; i++)
          ret[i] = urandr(0, 255);

     return ret;
}

void test_bytes(int mode) {
     size_t len;
     uint8_t *bs = gen_bytes(&len);

     size_t cnt = 0;
     int err = 0;
     struct wsmp_iex *iex = wsmp_iex_decode(bs, &cnt, len, &err, mode);

     if (err)
          ; //printf("ERR: %d CNT: %lu LEN: %lu MODE: %d\n", err, cnt, len, mode);
     else
          print_iex(iex);

     free_iex(iex);
     free(bs);
}

int cmp_bytes(uint8_t *b1, uint8_t *b2, size_t i) {
     int j;
     for (j = 0; j < i; j++) {
          if (b1[j] != b2[j])
               return 0;
     }

     return 1;
}

void test_iex(int mode) {
     struct wsmp_iex *iex1 = gen_iex(mode);
     struct wsmp_iex *iex2;
     uint8_t *buf1;
     uint8_t *buf2;
     size_t cnt1 = 0;
     size_t cnt2 = 0;
     int err1 = 0;
     int err2 = 0;


     buf1 = wsmp_iex_encode(iex1, &cnt1, &err1, mode);
     iex2 = wsmp_iex_decode(buf1, &cnt2, cnt1, &err2, mode);
     buf2 = wsmp_iex_encode(iex2, &cnt2, &err2, mode);

     if (cnt1 != cnt2 || err1 || err2 || !cmp_bytes(buf1, buf2, cnt1)) {
          printf("ERR1 %d ERR2 %d\n", err1, err2);
          printf("BUG: PACKETS NOT EQUAL\n\n");
          fflush(stdout);
     }

     free_iex(iex1);
     free_iex(iex2);
     free(buf1);
     free(buf2);
}

void test_sii(int mode) {
     struct wsmp_sii *sii1 = gen_sii(mode);
     struct wsmp_sii *sii2;
     uint8_t *buf1;
     uint8_t *buf2;
     size_t cnt1 = 0;
     size_t cnt2 = 0;
     int err1 = 0;
     int err2 = 0;

     buf1 = wsmp_sii_encode(sii1, &cnt1, &err1, mode);
     sii2 = wsmp_sii_decode(buf1, &cnt2, cnt1, &err2, mode);
     buf2 = wsmp_sii_encode(sii2, &cnt2, &err2, mode);

     if (cnt1 != cnt2 || err1 || err2 || !cmp_bytes(buf1, buf2, cnt1)) {
          printf("ERR1 %d ERR2 %d\n", err1, err2);
          printf("BUG: PACKETS NOT EQUAL\n\n");
          fflush(stdout);
     }

     free_sii(sii1);
     free_sii(sii2);
     free(buf1);
     free(buf2);
}

void test_cii(int mode) {
     struct wsmp_cii *cii1 = gen_cii(mode);
     struct wsmp_cii *cii2;
     uint8_t *buf1;
     uint8_t *buf2;
     size_t cnt1 = 0;
     size_t cnt2 = 0;
     int err1 = 0;
     int err2 = 0;

     buf1 = wsmp_cii_encode(cii1, &cnt1, &err1, mode);
     cii2 = wsmp_cii_decode(buf1, &cnt2, cnt1, &err2, mode);
     buf2 = wsmp_cii_encode(cii2, &cnt2, &err2, mode);

     if (cnt1 != cnt2 || err1 || err2 || !cmp_bytes(buf1, buf2, cnt1)) {
          printf("ERR1 %d ERR2 %d\n", err1, err2);
          printf("BUG: PACKETS NOT EQUAL\n\n");
          fflush(stdout);
     }

     free_cii(cii1);
     free_cii(cii2);
     free(buf1);
     free(buf2);
}

void test_wra(int mode) {
     struct wsmp_wra *wra1 = gen_wra(mode);
     struct wsmp_wra *wra2;
     uint8_t *buf1;
     uint8_t *buf2;
     size_t cnt1 = 0;
     size_t cnt2 = 0;
     int err1 = 0;
     int err2 = 0;

     buf1 = wsmp_wra_encode(wra1, &cnt1, &err1, mode);
     wra2 = wsmp_wra_decode(buf1, &cnt2, cnt1, &err2, mode);
     buf2 = wsmp_wra_encode(wra2, &cnt2, &err2, mode);

     if (cnt1 != cnt2 || err1 || err2 || !cmp_bytes(buf1, buf2, cnt1)) {
          printf("ERR1 %d ERR2 %d\n", err1, err2);
          printf("BUG: PACKETS NOT EQUAL\n\n");
          fflush(stdout);
     }

     free_wra(wra1);
     free_wra(wra2);
     free(buf1);
     free(buf2);
}

void test_wsa(int mode) {
     struct wsmp_wsa *wsa1 = gen_wsa(mode);
     struct wsmp_wsa *wsa2;
     uint8_t *buf1;
     uint8_t *buf2;
     size_t cnt1 = 0;
     size_t cnt2 = 0;
     int err1 = 0;
     int err2 = 0;

     buf1 = wsmp_wsa_encode(wsa1, &cnt1, &err1, mode);
     wsa2 = wsmp_wsa_decode(buf1, &cnt2, cnt1, &err2, mode);
     buf2 = wsmp_wsa_encode(wsa2, &cnt2, &err2, mode);

     if (cnt1 != cnt2 || err1 || err2 || !cmp_bytes(buf1, buf2, cnt1)) {
          printf("ERR1 %d ERR2 %d\n", err1, err2);
          printf("BUG: PACKETS NOT EQUAL\n\n");
          fflush(stdout);
     }

     free_wsa(wsa1);
     free_wsa(wsa2);
     free(buf1);
     free(buf2);
}

void test_wsm(int mode) {
     struct wsmp_wsm *wsm1 = gen_wsm(mode);
     struct wsmp_wsm *wsm2;
     uint8_t *buf1;
     uint8_t *buf2;
     size_t cnt1 = 0;
     size_t cnt2 = 0;
     int err1 = 0;
     int err2 = 0;

     buf1 = wsmp_wsm_encode(wsm1, &cnt1, &err1, mode);
     wsm2 = wsmp_wsm_decode(buf1, &cnt2, cnt1, &err2, mode);
     buf2 = wsmp_wsm_encode(wsm2, &cnt2, &err2, mode);

     if (cnt1 != cnt2 || err1 || err2 || !cmp_bytes(buf1, buf2, cnt1)) {
	print_wsm(wsm1);
	print_wsm(wsm2);
          printf("ERR1 %d ERR2 %d\n", err1, err2);
          printf("BUG: PACKETS NOT EQUAL\n\n");

	  printf("WSM1 FRAME:\n");
	  int i;
	  for (i = 0; i < cnt1; i++) {
		  printf("%02x ", buf1[i]);
	  }

	  printf("\nWSM2 FRAME:\n");
	  for (i = 0; i < cnt1; i++) {
		  printf("%02x ", buf2[i]);
	  }
	  printf("\n\n");
          fflush(stdout);
     }

     free_wsm(wsm1);
     free_wsm(wsm2);
     free(buf1);
     free(buf2);
}

int main(int ac, char **av) {
     if (ac != 3) {
          printf("usage: test seed it\n");
          exit(1);
     }

     srand(strtol(av[1], NULL, 10));

     printf("Beginning WSMP test...\n");
     int start = time(NULL);

     int i;
     for (i = 0; i < strtol(av[2], NULL, 10); i++) {
          // test_bytes(WSMP_STRICT);
	  test_wsm(WSMP_STRICT);
	  test_wsm(WSMP_LAX);
	  test_wsm(WSMP_LOOSE);
	  test_wsa(WSMP_STRICT);
	  test_wsa(WSMP_LAX);
	  test_wsa(WSMP_LOOSE);
	  test_iex(WSMP_STRICT);
	  test_iex(WSMP_LAX);
	  test_iex(WSMP_LOOSE);
	  test_sii(WSMP_STRICT);
	  test_sii(WSMP_LAX);
	  test_sii(WSMP_LOOSE);
	  test_cii(WSMP_STRICT);
	  test_cii(WSMP_LAX);
	  test_cii(WSMP_LOOSE);
	  test_wra(WSMP_STRICT);
	  test_wra(WSMP_LAX);
	  test_wra(WSMP_LOOSE);
     }

     int end = time(NULL);
     printf("Finished test in %d seconds.\n", (end-start));

     return 0;
}
