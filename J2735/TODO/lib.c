/* lib.c - library for command line tools
 * 
 * WAVE Short Message Protocol - IEEE 1609
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <sys/socket.h> /* for sa_family_t */
#include "lib.h"

//#define VDELIM ':'
#define VSEPARATOR '.' 
#define WSM

const char hex_asc_upper[] = "0123456789ABCDEF";

#define hex_asc_upper_lo(x)	hex_asc_upper[((x) & 0x0F)]
#define hex_asc_upper_hi(x)	hex_asc_upper[((x) & 0xF0) >> 4]

void print_usage(char *usg)
{
	fprintf(stderr, "\nUsage: %s <interface> {BSM|CSR|RSA|EVA} [options] \n", usg);
/*	fprintf(stderr, "Options: BSM <msg_ct> <id> <ms> <lat> <long> <elev> 
        <acc> <tx_state> <speed> <heading> <angle> <accel> <brakes> <vsize> 
        [-p2 [bsmpart2content]] [-r regional] \n");
  fprintf(stderr, "Options: CSR <requests> [-t timestamp] [-m msg_ct] [-i tid] [-r regional]\n");
  fprintf(stderr, "Options: RSA <msg_ct> <itis_event_type> [-t min] [-d desc] [-p priority] 
        [-h heading_16b] [-e extent] [-fp [fullposvector]] [-f finfo_id] [-r regional]\n");
  fprintf(stderr, "Options: EVA <RSA> [-t timestamp] [-i tid] [-rt resp_type] [-ed emergency_details] 
      [-sz mass] [-ac axle_ct] [-vt vehicle_type] [-re resp_equip] [-rt resp_type] [-r regional]\n");
	 example structure of {WSA, WSM, WRA} message */
  fprintf(stderr, "Options: BSM <msg_ct> <id> <ms> <lat> <long> <elev> 
        <acc> <tx_state> <speed> <heading> <angle> <accel> <brakes> <vsize> \n");
 /* fprintf(stderr, "Options: CSR <requests> [-t timestamp] [-m msg_ct] [-i tid] \n"); */
  /*fprintf(stderr, "Options: RSA <msg_ct> <itis_event_type> [-t timestamp] [-d desc] [-p priority] 
        [-h heading_16b] \n");*/
  fprintf(stderr, "Options: RSA <msg_ct> <itis_event_type> [-d desc] [-p priority] 
        [-h heading] [-P position: <lat> <long>] \n");
  fprintf(stderr, "Options: EVA <RSA> [-t timestamp] [-i tid] [-r resp_type] [-s mass] [-a axle_ct] \n");

} 

/* Standard options for all messages:
 * t : timestamp CSR EVA - NOT in RSA because RSA nested in EVA
 * m : msg count CSR
 * i : id or tid EVA CSR
 * d : description RSA
 * p : priority RSA
 * h : heading RSA
 * r : response type EVA
 * s : mass EVA
 * a : axle count EVA
 * P : position RSA
 */
int parse_v2v_msg(char *args, __u32 *v2vmsg, int argc) {
  enum DSRCmsgID msgID;

 // opts = malloc((argc - 2) * sizeof(uint8_t));
  int i;
  struct j2735MSG *msg;
  int msgsize;

  switch(argv[2]) {
    case 'BSM':
      msgID = basicSafetyMessage; 
      /* use getopt here */
      break;
    case 'CSR':
      msgID = commonSafetyRequest; 
      break;
    case 'EVA':
      msgID = emergencyVehicleAlert; 
      break;
    case 'RSA':
      msgID = roadSideAlert; 
      break;
    default:
      printf("Error: message type %s not yet supported \n",argv[2]);
      return 0;
  }


  
  
  return 1;
}

static inline void put_hex_byte(char *buf, __u8 byte)
{
	buf[0] = hex_asc_upper_hi(byte);
	buf[1] = hex_asc_upper_lo(byte);
}


static const char *error_classes[] = {
	"tx-timeout",
	"lost-arbitration",
	"controller-problem",
	"protocol-violation",
	"transceiver-status",
	"no-acknowledgement-on-tx",
	"bus-off",
	"bus-error",
	"restarted-after-bus-off",
};

static const char *controller_problems[] = {
	"rx-overflow",
	"tx-overflow",
	"rx-error-warning",
	"tx-error-warning",
	"rx-error-passive",
	"tx-error-passive",
	"back-to-error-active",
};

static const char *protocol_violation_types[] = {
	"single-bit-error",
	"frame-format-error",
	"bit-stuffing-error",
	"tx-dominant-bit-error",
	"tx-recessive-bit-error",
	"bus-overload",
	"active-error",
	"error-on-tx",
};

/* idea: pass in argv[] as *cs 
 */
int parse_v2vargs(char *cs, __u8 *msg) {

    

	return 0;
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


