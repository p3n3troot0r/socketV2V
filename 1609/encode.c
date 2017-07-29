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

uint8_t *wsmp_iex_encode(const struct wsmp_iex *curs, size_t *cnt, int *err, int mode) {
     uint8_t buf[WSMP_MAXSIZE];
     uint8_t *ret;
     size_t i[1];

     *i = 0;
     *cnt = 0;
     *err = 0;

     if (mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE) {
	  *err = -WSMP_EMODE;
	  goto out;
     }

     /* IEX Count */
     if (mode == WSMP_STRICT && curs->count == 0) {
	  fprintf(stderr, "ERROR: curs->count == 0\n");
	  *err = -WSMP_EDOMAIN;
	  goto out;
     } else if (curs->count > 256) {
	  fprintf(stderr, "ERROR: ret->count > 256\n");
	  *err = -WSMP_EBADMSG;
	  goto out;
     }

     _s_c(buf, i, curs->count, WSMP_MAXSIZE, err);

     int absent = 0;
     int j = 0;

     for (j = 0; j < WSMP_EID_CHANNEL_LOAD+1; j++) {
	  if (curs->use[j]) {
	       switch (j) {
	       case WSMP_EID_TX_POWER_USED_80211:
		    _s(buf, i, WSMP_EID_TX_POWER_USED_80211, WSMP_MAXSIZE, err);
		    _s(buf, i, 1,                            WSMP_MAXSIZE, err);
		    _s(buf, i, curs->tx_pow,                 WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_2DLOCATION:
		    _s  (buf, i, WSMP_EID_2DLOCATION,       WSMP_MAXSIZE, err);
		    _s  (buf, i, 8,                         WSMP_MAXSIZE, err);
		    _s_n(buf, i, 4, curs->loc_2d.latitude,  WSMP_MAXSIZE, err);
		    _s_n(buf, i, 4, curs->loc_2d.longitude, WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_3DLOCATION:
		    _s  (buf, i, WSMP_EID_3DLOCATION,       WSMP_MAXSIZE, err);
		    _s  (buf, i, 10,                        WSMP_MAXSIZE, err);
		    _s_n(buf, i, 4, curs->loc_3d.latitude,  WSMP_MAXSIZE, err);
		    _s_n(buf, i, 4, curs->loc_3d.longitude, WSMP_MAXSIZE, err);
		    _s_n(buf, i, 2, curs->loc_3d.elevation, WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_ADVERTISER_ID:
		    /* Following the standard (1609.3-2016-8.2.2.6.4, pg. 65), this IE wastes a byte */
		    if (mode == WSMP_STRICT && curs->advert_id.len == 0) {
			 fprintf(stderr, "ERROR: advert_id.len == 0\n");
			 *err = -WSMP_EDOMAIN;
			 goto out;
		    } else if (curs->advert_id.len > 32) {
			 fprintf(stderr, "ERROR: advert_id.len > 32\n");
			 *err = -WSMP_EBADMSG;
			 goto out;
		    }

		    _s  (buf, i, WSMP_EID_ADVERTISER_ID,                  WSMP_MAXSIZE, err);
		    _s_c(buf, i, curs->advert_id.len+1,                   WSMP_MAXSIZE, err);
		    _s_c(buf, i, curs->advert_id.len,                     WSMP_MAXSIZE, err);
		    _s_n(buf, i, curs->advert_id.len, curs->advert_id.id, WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_PROVIDER_SERV_CONTEXT:
		    /* Again, wasting a byte (1609.3-2016-8.2.3.5.1, pg. 66) */
		    if (mode == WSMP_STRICT && curs->psc.len == 0) {
			 fprintf(stderr, "ERROR: curs->psc.len == 0\n");
			 *err = -WSMP_EDOMAIN;
			 goto out;
		    } else if (curs->psc.len > 31) {
			 fprintf(stderr, "ERROR: curs->psc.len > 31\n");
			 *err = -WSMP_EBADMSG;
			 goto out;
		    }

		    _s  (buf, i, WSMP_EID_PROVIDER_SERV_CONTEXT, WSMP_MAXSIZE, err);
		    _s_c(buf, i, curs->psc.len+1,                WSMP_MAXSIZE, err);
		    _s_c(buf, i, curs->psc.len,                  WSMP_MAXSIZE, err);
		    _s_n(buf, i, curs->psc.len, curs->psc.psc,   WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_IPV6_ADDRESS:
		    _s  (buf, i, WSMP_EID_IPV6_ADDRESS, WSMP_MAXSIZE, err);
		    _s  (buf, i, 16,                    WSMP_MAXSIZE, err);
		    _s_n(buf, i, 16, curs->ip,          WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_SERVICE_PORT:
		    _s  (buf, i, WSMP_EID_SERVICE_PORT, WSMP_MAXSIZE, err);
		    _s  (buf, i, 2,                     WSMP_MAXSIZE, err);
		    _s_n(buf, i, 2, curs->port,         WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_PROVIDER_MAC_ADDRESS:
		    _s  (buf, i, WSMP_EID_PROVIDER_MAC_ADDRESS, WSMP_MAXSIZE, err);
		    _s  (buf, i, 6,                             WSMP_MAXSIZE, err);
		    _s_n(buf, i, 6, curs->mac,                  WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_EDCA_PARAMETER_SET:
		    _s  (buf, i, WSMP_EID_EDCA_PARAMETER_SET, WSMP_MAXSIZE, err);
		    _s  (buf, i, 16,                          WSMP_MAXSIZE, err);
		    _s_n(buf, i, 4, curs->edca.ac_be,         WSMP_MAXSIZE, err);
		    _s_n(buf, i, 4, curs->edca.ac_bk,         WSMP_MAXSIZE, err);
		    _s_n(buf, i, 4, curs->edca.ac_vi,         WSMP_MAXSIZE, err);
		    _s_n(buf, i, 4, curs->edca.ac_vo,         WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_SECONDARY_DNS:
		    _s  (buf, i, WSMP_EID_SECONDARY_DNS, WSMP_MAXSIZE, err);
		    _s  (buf, i, 16,                     WSMP_MAXSIZE, err);
		    _s_n(buf, i, 16, curs->sec_dns,      WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_GATEWAY_MAC_ADDRESS:
		    _s  (buf, i, WSMP_EID_GATEWAY_MAC_ADDRESS, WSMP_MAXSIZE, err);
		    _s  (buf, i, 6,                            WSMP_MAXSIZE, err);
		    _s_n(buf, i, 6, curs->gateway_mac,         WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_CHANNEL_NUMBER_80211:
		    if (mode != WSMP_LOOSE && curs->chan > 200) {
			 fprintf(stderr, "ERROR: curs->chan > 200\n");
			 *err = -WSMP_EDOMAIN;
			 goto out;
		    }

		    _s(buf, i, WSMP_EID_CHANNEL_NUMBER_80211, WSMP_MAXSIZE, err);
		    _s(buf, i, 1,                             WSMP_MAXSIZE, err);
		    _s(buf, i, curs->chan,                    WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_DATA_RATE_80211:
		    if (mode != WSMP_LOOSE && (curs->data_rate < 2 || curs->data_rate > 127)) {
			 fprintf(stderr, "ERROR: curs->data_rate < 2 || curs->data_rate > 127\n");
			 *err = -WSMP_EDOMAIN;
			 goto out;
		    }

		    _s(buf, i, WSMP_EID_DATA_RATE_80211, WSMP_MAXSIZE, err);
		    _s(buf, i, 1,                        WSMP_MAXSIZE, err);
		    _s(buf, i, curs->data_rate,          WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_REPEAT_RATE:
		    _s(buf, i, WSMP_EID_REPEAT_RATE, WSMP_MAXSIZE, err);
		    _s(buf, i, 1,                    WSMP_MAXSIZE, err);
		    _s(buf, i, curs->repeat_rate,    WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_RCPI_THRESHOLD:
		    if (mode != WSMP_LOOSE && (curs->rcpi_thres > 0 || curs->rcpi_thres < -110)) {
			 fprintf(stderr, "ERROR: curs->rcpi_thres > 0 || curs->rcpi_thres < -110\n");
			 *err = -WSMP_EDOMAIN;
			 goto out;
		    }

		    _s(buf, i, WSMP_EID_RCPI_THRESHOLD, WSMP_MAXSIZE, err);
		    _s(buf, i, 1,                       WSMP_MAXSIZE, err);
		    _s(buf, i, curs->rcpi_thres,        WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_WSA_COUNT_THRESHOLD:
		    _s(buf, i, WSMP_EID_WSA_COUNT_THRESHOLD, WSMP_MAXSIZE, err);
		    _s(buf, i, 1,                            WSMP_MAXSIZE, err);
		    _s(buf, i, curs->rcpi_thres,             WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_CHANNEL_ACCESS:
		    if (mode != WSMP_LOOSE && curs->chan_access > 2) {
			 fprintf(stderr, "ERROR: curs->chan_access > 2\n");
			 *err = -WSMP_EDOMAIN;
			 goto out;
		    }

		    _s(buf, i, WSMP_EID_CHANNEL_ACCESS, WSMP_MAXSIZE, err);
		    _s(buf, i, 1,                       WSMP_MAXSIZE, err);
		    _s(buf, i, curs->chan_access,       WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_WSA_COUNT_THRES_INT:
		    if (mode != WSMP_LOOSE && curs->count_thres_int == 0) {
			 fprintf(stderr, "ERROR: curs->count_thres_int == 0\n");
			 *err = -WSMP_EDOMAIN;
			 goto out;
		    }

		    _s(buf, i, WSMP_EID_WSA_COUNT_THRES_INT, WSMP_MAXSIZE, err);
		    _s(buf, i, 1,                            WSMP_MAXSIZE, err);
		    _s(buf, i, curs->count_thres_int,        WSMP_MAXSIZE, err);

		    break;

	       case WSMP_EID_CHANNEL_LOAD:
		    /* FALL THROUGH (UNSPECIFIED) */
		    if (mode == WSMP_STRICT) {
			 fprintf(stderr, "case WSMP_EID_CHANNEL_LOAD:\n");
			 *err = -WSMP_EBADMSG;
			 goto out;
		    }

	       default:
		    absent++;
		    break;
	       }
	  } else {
	       absent++;
	  }
     }

     int present = WSMP_EID_CHANNEL_LOAD+1 - absent;

     if (curs->raw_count != curs->count - present) {
	  /* Number of remaining IEs is invalid */
	  fprintf(stderr, "ERROR: curs->raw_count != curs->count - present\n");
	  *err = -WSMP_EBADMSG;
	  goto out;
     }

     for (j = 0; j < curs->raw_count; j++) {
	  /* if (mode == WSMP_STRICT && */
	  /*     (curs->raw[j].id <= WSMP_EID_CHANNEL_LOAD && curs->use[curs->raw[j].id])) { */
	  /*      fprintf(stderr, "ERROR: mode == WSMP_STRICT && (curs->raw[j].id <= WSMP_EID_CHANNEL_LOAD && curs->use[curs->raw[j].id])\n"); */
	  /*      *err = -WSMP_EBADMSG; */
	  /*      goto out; */
	  /* } */

	  _s(buf, i, curs->raw[j].id, WSMP_MAXSIZE, err);

	  if (mode == WSMP_STRICT && curs->raw[j].len == 0) {
	       fprintf(stderr, "ERROR: curs->raw[j].len == 0\n");
	       *err = -WSMP_EDOMAIN;
	       goto out;
	  }

	  _s_c(buf, i, curs->raw[j].len,                    WSMP_MAXSIZE, err);
	  _s_n(buf, i, curs->raw[j].len, curs->raw[j].data, WSMP_MAXSIZE, err);
     }

out:
     ret = calloc(*i, 1);
     memcpy(ret, buf, *i);
     *cnt = *i;

     return ret;
}

uint8_t *wsmp_sii_encode(const struct wsmp_sii *curs, size_t *cnt, int *err, int mode) {
     uint8_t buf[WSMP_MAXSIZE];
     uint8_t *ret;
     size_t i[1];

     *i = 0;
     *cnt = 0;
     *err = 0;

     if (mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE) {
	  fprintf(stderr, "ERROR: mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE\n");
	  *err = -WSMP_EMODE;
	  goto out;
     }

     _s_p(buf, i, curs->psid, WSMP_MAXSIZE, err);

     if (mode != WSMP_LOOSE && curs->chan_index > 31) {
	  fprintf(stderr, "ERROR: mode != WSMP_LOOSE && curs->chan_index > 31\n");
	  *err = -WSMP_EDOMAIN;
	  goto out;
     }

     if (curs->use_iex) {
	  _s(buf, i, (curs->chan_index << 3) | 0x01, WSMP_MAXSIZE, err);

	  size_t iex_cnt = 0;
	  uint8_t *iex_buf = wsmp_iex_encode(curs->iex, &iex_cnt, err, mode);

	  _s_n(buf, i, iex_cnt, iex_buf, WSMP_MAXSIZE, err);
	  free(iex_buf);
     } else {
	  _s(buf, i, curs->chan_index << 3, WSMP_MAXSIZE, err);
     }

out:
     ret = calloc(*i, 1);
     memcpy(ret, buf, *i);
     *cnt = *i;

     return ret;
}

uint8_t *wsmp_cii_encode(const struct wsmp_cii *curs, size_t *cnt, int *err, int mode) {
     uint8_t buf[WSMP_MAXSIZE];
     uint8_t *ret;
     size_t i[1];

     *i = 0;
     *cnt = 0;
     *err = 0;

     if (mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE) {
	  fprintf(stderr, "ERROR: mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE\n");
	  *err = -WSMP_EMODE;
	  goto out;
     }

     /* TODO: ADD RANGE CHECK FOR OPERATING CLASS WITHIN THE 5GHz BAND */
     _s(buf, i, curs->op_class, WSMP_MAXSIZE, err);
     _s(buf, i, curs->chan,     WSMP_MAXSIZE, err);
     _s(buf, i, curs->tx_pow,   WSMP_MAXSIZE, err);

     if (curs->adapt != 0 && curs->adapt != 1) {
	  fprintf(stderr, "ERROR: curs->adapt != 0 && curs->adapt != 1\n");
	  *err = -WSMP_EBADMSG;
	  goto out;
     }

     if (mode != WSMP_LOOSE && (curs->data_rate < 2 || curs->data_rate > 127)) {
	  fprintf(stderr, "ERROR: mode != WSMP_LOOSE && (curs->data_rate < 2 || curs->data_rate > 127)\n");
	  *err = -WSMP_EDOMAIN;
	  goto out;
     }

     _s(buf, i, (curs->adapt << 7) | curs->data_rate, WSMP_MAXSIZE, err);

     if (curs->use_iex) {
	  _s(buf, i, 1, WSMP_MAXSIZE, err);

	  size_t iex_cnt = 0;
	  uint8_t *iex_buf = wsmp_iex_encode(curs->iex, &iex_cnt, err, mode);

	  _s_n(buf, i, iex_cnt, iex_buf, WSMP_MAXSIZE, err);
	  free(iex_buf);
     } else {
	  _s(buf, i, 0, WSMP_MAXSIZE, err);
     }

out:
     ret = calloc(*i, 1);
     memcpy(ret, buf, *i);
     *cnt = *i;

     return ret;
}

uint8_t *wsmp_wra_encode(const struct wsmp_wra *curs, size_t *cnt, int *err, int mode) {
     uint8_t buf[WSMP_MAXSIZE];
     uint8_t *ret;
     size_t i[1];

     *i = 0;
     *cnt = 0;
     *err = 0;

     if (mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE) {
	  fprintf(stderr, "ERROR: mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE\n");
	  *err = -WSMP_EMODE;
	  goto out;
     }

     _s_n(buf, i, 2,  curs->lifetime,   WSMP_MAXSIZE, err);
     _s_n(buf, i, 16, curs->ip_prefix,  WSMP_MAXSIZE, err);
     _s  (buf, i,     curs->prefix_len, WSMP_MAXSIZE, err);
     _s_n(buf, i, 16, curs->gateway,    WSMP_MAXSIZE, err);
     _s_n(buf, i, 16, curs->dns,        WSMP_MAXSIZE, err);

     if (curs->use_iex) {
	  _s(buf, i, 1, WSMP_MAXSIZE, err);

	  size_t iex_cnt = 0;
	  uint8_t *iex_buf = wsmp_iex_encode(curs->iex, &iex_cnt, err, mode);

	  _s_n(buf, i, iex_cnt, iex_buf, WSMP_MAXSIZE, err);
	  free(iex_buf);
     } else {
	  /* "In this case the number of such extension elements preceeds the extension elements. */
	  /*  This number is set to zero in case no extension elements are needed." */
	  /*  Dr. Hans-Joachim Fischer <HJFischer@e-postdienst.de> */
	  _s(buf, i, 0, WSMP_MAXSIZE, err);
     }

out:
     ret = calloc(*i, 1);
     memcpy(ret, buf, *i);
     *cnt = *i;

     return ret;
}

uint8_t *wsmp_wsa_encode(const struct wsmp_wsa *curs, size_t *cnt, int *err, int mode) {
     uint8_t buf[WSMP_MAXSIZE];
     uint8_t *ret;
     size_t i[1];

     *i = 0;
     *cnt = 0;
     *err = 0;

     if (mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE) {
	  fprintf(stderr, "ERROR: mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE\n");
	  *err = -WSMP_EMODE;
	  goto out;
     }

     if (curs->version != WSMP_VERSION) {
	  fprintf(stderr, "ERROR: curs->version != WSMP_VERSION\n");
	  *err = -WSMP_ENOSUPPORT;
	  goto out;
     }

     uint8_t hoi = 0x00;

     if (curs->use_iex)
	  hoi |= 0b1000;
     if (curs->sii_count > 0)
	  hoi |= 0b0100;
     if (curs->cii_count > 0)
	  hoi |= 0b0010;
     if (curs->use_wra)
	  hoi |= 0b0001;

     _s(buf, i, (curs->version << 4) | hoi, WSMP_MAXSIZE, err);

     if (curs->id > 15 || curs->content_count > 15) {
	  fprintf(stderr, "ERROR: curs->id > 15 || curs->content_count > 15\n");
	  *err = -WSMP_EBADMSG;
	  goto out;
     }

     _s(buf, i, (curs->id << 4) | curs->content_count, WSMP_MAXSIZE, err);

     if (curs->use_iex) {
	  size_t iex_cnt = 0;
	  uint8_t *iex_buf = wsmp_iex_encode(curs->iex, &iex_cnt, err, mode);

	  _s_n(buf, i, iex_cnt, iex_buf, WSMP_MAXSIZE, err);
	  free(iex_buf);
     }

     int j = 0;

     if (curs->sii_count > 0) {
	  if (mode == WSMP_STRICT && curs->sii_count == 0) {
	       fprintf(stderr, "ERROR: mode == WSMP_STRICT && curs->sii_count == 0\n");
	       *err = -WSMP_EDOMAIN;
	       goto out;
	  } else if (curs->sii_count > 31) {
	       fprintf(stderr, "ERROR: curs->sii_count > 31\n");
	       *err = -WSMP_EBADMSG;
	       goto out;
	  }

	  _s_c(buf, i, curs->sii_count, WSMP_MAXSIZE, err);

	  for (j = 0; j < curs->sii_count; j++) {
	       size_t sii_cnt = 0;
	       uint8_t *sii_buf = wsmp_sii_encode(curs->sis[j], &sii_cnt, err, mode);

	       _s_n(buf, i, sii_cnt, sii_buf, WSMP_MAXSIZE, err);
	       free(sii_buf);
	  }
     }

     if (curs->cii_count > 0) {
	  if (mode == WSMP_STRICT && curs->cii_count == 0) {
	       fprintf(stderr, "ERROR: mode == WSMP_STRICT && curs->cii_count == 0\n");
	       *err = -WSMP_EDOMAIN;
	       goto out;
	  } else if (curs->cii_count > 31) {
	       fprintf(stderr, "ERROR: curs->cii_count > 31\n");
	       *err = -WSMP_EBADMSG;
	       goto out;
	  }

	  _s_c(buf, i, curs->cii_count, WSMP_MAXSIZE, err);

	  for (j = 0; j < curs->cii_count; j++) {
	       size_t cii_cnt = 0;
	       uint8_t *cii_buf = wsmp_cii_encode(curs->cis[j], &cii_cnt, err, mode);

	       _s_n(buf, i, cii_cnt, cii_buf, WSMP_MAXSIZE, err);
	       free(cii_buf);
	  }
     }

     if (curs->use_wra) {
	  size_t wra_cnt = 0;
	  uint8_t *wra_buf = wsmp_wra_encode(curs->wra, &wra_cnt, err, mode);

	  _s_n(buf, i, wra_cnt, wra_buf, WSMP_MAXSIZE, err);
	  free(wra_buf);
     }

out:
     ret = calloc(*i, 1);
     memcpy(ret, buf, *i);
     *cnt = *i;

     return ret;
}

uint8_t *wsmp_wsm_encode(const struct wsmp_wsm *curs, size_t *cnt, int *err, int mode) {
     uint8_t buf[WSMP_MAXSIZE];
     uint8_t *ret;
     size_t i[1];

     *i = 0;
     *cnt = 0;
     *err = 0;

     if (mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE) {
	  fprintf(stderr, "ERROR: mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE\n");
	  *err = -WSMP_EMODE;
	  goto out;
     }

     if (mode != WSMP_LOOSE && curs->subtype > 4) {
	  fprintf(stderr, "ERROR: mode != WSMP_LOOSE && curs->subtype > 4\n");
	  *err = -WSMP_EDOMAIN;
	  goto out;
     } else if (curs->version != WSMP_VERSION) {
	  fprintf(stderr, "ERROR: curs->version != WSMP_VERSION\n");
	  *err = -WSMP_ENOSUPPORT;
	  goto out;
     }

     _s(buf, i, (curs->subtype << 4) | (curs->use_n_iex << 3) | curs->version, WSMP_MAXSIZE, err);

     if (curs->use_n_iex) {
	  size_t n_iex_cnt = 0;
	  uint8_t *n_iex_buf = wsmp_iex_encode(curs->n_iex, &n_iex_cnt, err, mode);

	  _s_n(buf, i, n_iex_cnt, n_iex_buf, WSMP_MAXSIZE, err);
	  free(n_iex_buf);
     }

     if (curs->tpid > 5) {
	  fprintf(stderr, "ERROR: curs->tpid > 5\n");
	  *err = -WSMP_ENOSUPPORT;
	  goto out;
     }

     _s(buf, i, curs->tpid, WSMP_MAXSIZE, err);

     if (curs->tpid < 2) {
	  _s_p(buf, i, curs->psid, WSMP_MAXSIZE, err);
     } else {
	  _s_n(buf, i, 2, curs->ports.src, WSMP_MAXSIZE, err);
	  _s_n(buf, i, 2, curs->ports.dst, WSMP_MAXSIZE, err);
     }

     uint8_t tmp = curs->tpid % 2 == 1;

     if ((tmp && !curs->use_t_iex) || (!tmp && curs->use_t_iex)) {
	  fprintf(stderr, "ERROR: (tmp && !curs->use_t_iex) || (!tmp && curs->use_t_iex)\n");
	  *err = -WSMP_EBADMSG;
	  goto out;
     }

     if (curs->use_t_iex) {
	  size_t t_iex_cnt = 0;
	  uint8_t *t_iex_buf = wsmp_iex_encode(curs->t_iex, &t_iex_cnt, err, mode);

	  _s_n(buf, i, t_iex_cnt, t_iex_buf, WSMP_MAXSIZE, err);
	  free(t_iex_buf);
     }

     if (mode == WSMP_STRICT && curs->len == 0) {
	  fprintf(stderr, "ERROR: mode == WSMP_STRICT && curs->len == 0\n");
	  *err = -WSMP_EDOMAIN;
	  goto out;
     }

     _s_c(buf, i, curs->len, WSMP_MAXSIZE, err);
     _s_n(buf, i, curs->len, curs->data, WSMP_MAXSIZE, err);

out:
     ret = calloc(*i, 1);
     memcpy(ret, buf, *i);
     *cnt = *i;

     return ret;
}
