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
#include "decode.h"

struct wsmp_iex *wsmp_iex_decode(const uint8_t *msg, size_t *cnt, size_t len, int *err, int mode) {
     struct wsmp_iex *ret = calloc(1, sizeof(struct wsmp_iex));
     size_t i[1];

     *i = *cnt;
     *err = 0;

     int raw_j = 0;
     int j = 0;

     if (mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE) {
	  *err = -WSMP_EMODE;
	  goto out;
     }

     if (len < WSMP_IEX_MIN || len > WSMP_MAXSIZE) {
	  fprintf(stderr, "ERROR: len < WSMP_IEX_MIN || len > WSMP_MAXSIZE\n");
	  *err = -WSMP_EFAULT;
	  goto out;
     }

     _g_c(msg, i, &ret->count, len, err);

     if (mode == WSMP_STRICT && ret->count == 0) {
	  fprintf(stderr, "ERROR: ret->count == 0\n");
	  *err = -WSMP_EDOMAIN;
	  goto out;
     } else if (ret->count > 256) {
	  fprintf(stderr, "ERROR: ret->count > 256\n");
	  *err = -WSMP_EBADMSG;
	  goto out;
     }

     for (j = 0; j < ret->count; j++) {
	  uint8_t elem_id = 0;
	  uint16_t elem_len = 0;

	  _g  (msg, i, &elem_id,  len, err);
	  _g_c(msg, i, &elem_len, len, err);

	  if (*i + elem_len > len) {
	       fprintf(stderr, "ERROR: *i + elem_len > len\n");
	       *err = -WSMP_EFAULT;
	       goto out;
	  }

	  if (elem_id <= WSMP_EID_CHANNEL_LOAD) {
	       if (ret->use[elem_id]) {
		    /* if (mode == WSMP_STRICT) { */
		    /*      fprintf(stderr, "ERROR: mode == WSMP_STRICT && ret->use[elem_id]\n"); */
		    /*      *err = -WSMP_EDUPLICATE; */
		    /*      goto out; */
		    /* } */

		    fprintf(stderr, "NOTICE: goto raw\n");
		    goto raw;
	       }

	       ret->use[elem_id] = 1;
	  }

	  switch (elem_id) {
	  case WSMP_EID_TX_POWER_USED_80211:
	       if (elem_len != 1) {
		    fprintf(stderr, "ERROR: elem_len != 1\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _gs(msg, i, &ret->tx_pow, len, err);

	       break;

	  case WSMP_EID_2DLOCATION:
	       if (elem_len != 8) {
		    fprintf(stderr, "ERROR: elem_len != 8\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g_n(msg, i, 4, ret->loc_2d.latitude,  len, err);
	       _g_n(msg, i, 4, ret->loc_2d.longitude, len, err);

	       break;

	  case WSMP_EID_3DLOCATION:
	       if (elem_len != 10) {
		    fprintf(stderr, "ERROR: elem_len != 10\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g_n(msg, i, 4, ret->loc_3d.latitude,  len, err);
	       _g_n(msg, i, 4, ret->loc_3d.longitude, len, err);
	       _g_n(msg, i, 2, ret->loc_3d.elevation, len, err);

	       break;

	  case WSMP_EID_ADVERTISER_ID:
	       if (mode == WSMP_STRICT && elem_len == 1) {
		    fprintf(stderr, "mode == WSMP_STRICT && elem_len == 1\n");
		    *err = -WSMP_EDOMAIN;
		    goto out;
	       } else if (elem_len < 1 || elem_len > 34) {
		    fprintf(stderr, "ERROR: elem_len < 1 || elem_len > 34\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g_c(msg, i, &ret->advert_id.len, len, err);

	       if (mode == WSMP_STRICT && ret->advert_id.len == 0) {
		    fprintf(stderr, "ERROR: mode == WSMP_STRICT && ret->advert_id.len == 0\n");
		    *err = -WSMP_EDOMAIN;
		    goto out;
	       } else if (ret->advert_id.len > 32) {
		    fprintf(stderr, "ERROR: ret->advert_id.len > 32\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       } else if (ret->advert_id.len != elem_len-1) {
		    fprintf(stderr, "ERROR: ret->advert_id.len != elem_len-1\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g_n(msg, i, ret->advert_id.len, ret->advert_id.id, len, err);

	       break;

	  case WSMP_EID_PROVIDER_SERV_CONTEXT:
	       if (mode == WSMP_STRICT && elem_len == 1) {
		    fprintf(stderr, "mode == WSMP_STRICT && elem_len == 1\n");
		    *err = -WSMP_EDOMAIN;
		    goto out;
	       } else if (elem_len < 1 || elem_len > 32) {
		    fprintf(stderr, "ERROR: elem_len < 1 || elem_len > 32\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g_c(msg, i, &ret->psc.len, len, err);

	       if (mode == WSMP_STRICT && ret->psc.len == 0) {
		    fprintf(stderr, "ERROR: mode == WSMP_STRICT && ret->psc.len == 0\n");
		    *err = -WSMP_EDOMAIN;
		    goto out;
	       } else if (ret->psc.len > 31) {
		    fprintf(stderr, "ERROR: ret->psc.len > 31\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       } else if (ret->psc.len != elem_len-1) {
		    fprintf(stderr, "ERROR: ret->psc.len != elem_len-1\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g_n(msg, i, ret->psc.len, ret->psc.psc, len, err);

	       break;

	  case WSMP_EID_IPV6_ADDRESS:
	       if (elem_len != 16) {
		    fprintf(stderr, "ERROR: elem_len != 16\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g_n(msg, i, 16, ret->ip, len, err);

	       break;

	  case WSMP_EID_SERVICE_PORT:
	       if (elem_len != 2) {
		    fprintf(stderr, "ERROR: elem_len != 2\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g_n(msg, i, 2, ret->port, len, err);

	       break;

	  case WSMP_EID_PROVIDER_MAC_ADDRESS:
	       if (elem_len != 6) {
		    fprintf(stderr, "ERROR: elem_len != 6\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g_n(msg, i, 6, ret->mac, len, err);

	       break;

	  case WSMP_EID_EDCA_PARAMETER_SET:
	       if (elem_len != 16) {
		    fprintf(stderr, "ERROR: elem_len != 16\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g_n(msg, i, 4, ret->edca.ac_be, len, err);
	       _g_n(msg, i, 4, ret->edca.ac_bk, len, err);
	       _g_n(msg, i, 4, ret->edca.ac_vi, len, err);
	       _g_n(msg, i, 4, ret->edca.ac_vo, len, err);

	       break;

	  case WSMP_EID_SECONDARY_DNS:
	       if (elem_len != 16) {
		    fprintf(stderr, "ERROR: elem_len != 16\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g_n(msg, i, 16, ret->sec_dns, len, err);

	       break;

	  case WSMP_EID_GATEWAY_MAC_ADDRESS:
	       if (elem_len != 6) {
		    fprintf(stderr, "ERROR: elem_len != 6\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g_n(msg, i, 6, ret->gateway_mac, len, err);

	       break;

	  case WSMP_EID_CHANNEL_NUMBER_80211:
	       if (elem_len != 1) {
		    fprintf(stderr, "ERROR: elem_len != 1\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g(msg, i, &ret->chan, len, err);

	       if (mode != WSMP_LOOSE && ret->chan > 200) {
		    fprintf(stderr, "ERROR: mode != WSMP_LOOSE && ret->chan > 200\n");
		    *err = -WSMP_EDOMAIN;
		    goto out;
	       }

	       break;

	  case WSMP_EID_DATA_RATE_80211:
	       if (elem_len != 1) {
		    fprintf(stderr, "ERROR: elem_len != 1\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g(msg, i, &ret->data_rate, len, err);

	       if (mode != WSMP_LOOSE && (ret->data_rate < 2 || ret->data_rate > 127)) {
		    fprintf(stderr, "ERROR: mode != WSMP_LOOSE && (ret->data_rate < 2 || ret->data_rate > 127)\n");
		    *err = -WSMP_EDOMAIN;
		    goto out;
	       }

	       break;

	  case WSMP_EID_REPEAT_RATE:
	       if (elem_len != 1) {
		    fprintf(stderr, "ERROR: elem_len != 1\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g(msg, i, &ret->repeat_rate, len, err);

	       break;

	  case WSMP_EID_RCPI_THRESHOLD:
	       if (elem_len != 1) {
		    fprintf(stderr, "ERROR: elem_len != 1\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _gs(msg, i, &ret->rcpi_thres, len, err);

	       if (mode != WSMP_LOOSE && (ret->rcpi_thres > 0 || ret->rcpi_thres < -110)) {
		    fprintf(stderr, "ERROR: mode != WSMP_LOOSE && (ret->rcpi_thres > 0 || ret->rcpi_thres < -110)\n");
		    *err = -WSMP_EDOMAIN;
		    goto out;
	       }

	       break;

	  case WSMP_EID_WSA_COUNT_THRESHOLD:
	       if (elem_len != 1) {
		    fprintf(stderr, "ERROR: elem_len != 1\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g(msg, i, &ret->count_thres, len, err);

	       break;

	  case WSMP_EID_CHANNEL_ACCESS:
	       if (elem_len != 1) {
		    fprintf(stderr, "ERROR: elem_len != 1\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g(msg, i, &ret->chan_access, len, err);

	       if (mode != WSMP_LOOSE && ret->chan_access > 2) {
		    fprintf(stderr, "ERROR: mode != WSMP_LOOSE && ret->chan_access > 2\n");
		    *err = -WSMP_EDOMAIN;
		    goto out;
	       }

	       break;

	  case WSMP_EID_WSA_COUNT_THRES_INT:
	       if (elem_len != 1) {
		    fprintf(stderr, "ERROR: elem_len != 1\n");
		    *err = -WSMP_EBADMSG;
		    goto out;
	       }

	       _g(msg, i, &ret->count_thres_int, len, err);

	       if (mode != WSMP_LOOSE && ret->count_thres_int == 0) {
		    fprintf(stderr, "ERROR: mode != WSMP_LOOSE && ret->count_thres_int == 0\n");
		    *err = -WSMP_EDOMAIN;
		    goto out;
	       }

	       break;

	  case WSMP_EID_CHANNEL_LOAD:
	       /* FALL THROUGH (UNSPECIFIED) */

	  default:
	  raw:
	       /* if (raw_j >= ret->raw_count-1) { */
	       /*	    fprintf(stderr, "ERROR: raw_j >= ret->raw_count-1\n"); */
	       /*	    *err = -WSMP_EBADMSG; */
	       /*	    goto out; */
	       /* } */

	       if (ret->raw_count == 0) {
		    ret->raw_count = ret->count - j+1;
		    ret->raw = calloc(ret->raw_count, sizeof(struct wsmp_ie_raw));
	       }

	       ret->raw[raw_j].id = elem_id;
	       ret->raw[raw_j].len = elem_len;
	       ret->raw[raw_j].data = calloc(elem_len, 1);

	       _g_n(msg, i, elem_len, ret->raw[raw_j].data, len, err);
	       raw_j++;

	       break;
	  }
     }

     /* if (*i != len) { */
     /*		  *err = -WSMP_EFAULT; */
     /*		  goto out; */
     /* } */

out:
     ret->raw_count = raw_j;

     if (ret->raw_count > 0)
	  ret->raw = realloc(ret->raw, ret->raw_count * sizeof(struct wsmp_ie_raw));

     *cnt = *i;
     return ret;
}

struct wsmp_sii *wsmp_sii_decode(const uint8_t *msg, size_t *cnt, size_t len, int *err, int mode) {
     struct wsmp_sii *ret = calloc(1, sizeof(struct wsmp_sii));
     size_t i[1];

     *i = *cnt;
     *err = 0;

     if (mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE) {
	  fprintf(stderr, "ERROR: mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE\n");
	  *err = -WSMP_EMODE;
	  goto out;
     }

     if (len < WSMP_SII_MIN || len > WSMP_MAXSIZE) {
	  fprintf(stderr, "ERROR: len < WSMP_SII_MIN || len > WSMP_MAXSIZE\n");
	  *err = -WSMP_EFAULT;
	  goto out;
     }

     uint8_t tmp = 0;

     _g_p(msg, i, &ret->psid, len, err);
     _g  (msg, i, &tmp, len, err);

     ret->chan_index = tmp >> 3;
     ret->use_iex = tmp & 0x01;

     if (mode != WSMP_LOOSE && ret->chan_index > 31) {
	  fprintf(stderr, "ERROR: mode != WSMP_LOOSE && ret->chan_index > 31\n");
	  *err = -WSMP_EDOMAIN;
	  goto out;
     }

     if (ret->use_iex) {
	  size_t iex_cnt = *i;
	  struct wsmp_iex *iex = wsmp_iex_decode(msg, &iex_cnt, len, err, mode);

	  *i = iex_cnt;
	  ret->iex = iex;
     }

     /* if (*i != len) { */
     /*		  fprintf(stderr, "ERROR: *i != len\n"); */
     /*		  *err = -WSMP_EFAULT; */
     /*		  goto out; */
     /* } */

out:
     *cnt = *i;
     return ret;
}

struct wsmp_cii *wsmp_cii_decode(const uint8_t *msg, size_t *cnt, size_t len, int *err, int mode) {
     struct wsmp_cii *ret = calloc(1, sizeof(struct wsmp_cii));
     size_t i[1];

     *i = *cnt;
     *err = 0;

     if (mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE) {
	  fprintf(stderr, "ERROR: mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE\n");
	  *err = -WSMP_EMODE;
	  goto out;
     }

     if (len < WSMP_CII_MIN || len > WSMP_MAXSIZE) {
	  fprintf(stderr, "ERROR: len < WSMP_CII_MIN || len > WSMP_MAXSIZE\n");
	  *err = -WSMP_EFAULT;
	  goto out;
     }

     /* TODO: ADD RANGE CHECK FOR OPERATING CLASS WITHIN THE 5GHz BAND */
     _g (msg, i, &ret->op_class, len, err);
     _g (msg, i, &ret->chan,     len, err);
     _gs(msg, i, &ret->tx_pow,   len, err);

     uint8_t tmp = 0;

     _g(msg, i, &tmp, len, err);

     ret->adapt = tmp >> 7;
     ret->data_rate = tmp & 0x7F;

     if (mode != WSMP_LOOSE && ret->data_rate < 0x02) {
	  fprintf(stderr, "ERROR: mode != WSMP_LOOSE && ret->data_rate < 0x02\n");
	  *err = -WSMP_EDOMAIN;
	  goto out;
     }

     _g(msg, i, &tmp, len, err);

     ret->use_iex = tmp;

     if (ret->use_iex != 0 && ret->use_iex != 1) {
	  fprintf(stderr, "ERROR: ret->use_iex != 0 && ret->use_iex != 1\n");
	  *err = -WSMP_EBADMSG;
	  goto out;
     }

     if (ret->use_iex) {
	  size_t iex_cnt = *i;
	  struct wsmp_iex *iex = wsmp_iex_decode(msg, &iex_cnt, len, err, mode);

	  *i = iex_cnt;
	  ret->iex = iex;
     }

     /* if (*i != len) { */
     /*		  fprintf(stderr, "ERROR: *i != len\n"); */
     /*		  *err = -WSMP_EFAULT; */
     /*		  goto out; */
     /* } */

out:
     *cnt = *i;
     return ret;
}

struct wsmp_wra *wsmp_wra_decode(const uint8_t *msg, size_t *cnt, size_t len, int *err, int mode) {
     struct wsmp_wra *ret = calloc(1, sizeof(struct wsmp_wra));
     size_t i[1];

     *i = *cnt;
     *err = 0;

     if (mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE) {
	  fprintf(stderr, "ERROR: mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE\n");
	  *err = -WSMP_EMODE;
	  goto out;
     }

     if (len < WSMP_WRA_MIN || len > WSMP_MAXSIZE) {
	  fprintf(stderr, "ERROR: len < WSMP_WRA_MIN || len > WSMP_MAXSIZE\n");
	  *err = -WSMP_EFAULT;
	  goto out;
     }

     _g_n(msg, i, 2,  ret->lifetime,    len, err);
     _g_n(msg, i, 16, ret->ip_prefix,   len, err);
     _g  (msg, i,     &ret->prefix_len, len, err);
     _g_n(msg, i, 16, ret->gateway,     len, err);
     _g_n(msg, i, 16, ret->dns,         len, err);

     uint8_t tmp = 0;

     /* "In this case the number of such extension elements preceeds the extension elements. */
     /*  This number is set to zero in case no extension elements are needed." */
     /*  Dr. Hans-Joachim Fischer <HJFischer@e-postdienst.de> */
     _g(msg, i, &tmp, len, err);

     if (tmp > 0) {
	  ret->use_iex = 1;

	  size_t iex_cnt = *i;
	  struct wsmp_iex *iex = wsmp_iex_decode(msg, &iex_cnt, len, err, mode);

	  *i = iex_cnt;
	  ret->iex = iex;
     }

     /* if (*i != len) { */
     /*		  fprintf(stderr, "ERROR: *i != len\n"); */
     /*		  *err = -WSMP_EFAULT; */
     /*		  goto out; */
     /* } */

out:
     *cnt = *i;
     return ret;
}

struct wsmp_wsa *wsmp_wsa_decode(const uint8_t *msg, size_t *cnt, size_t len, int *err, int mode) {
     struct wsmp_wsa *ret = calloc(1, sizeof(struct wsmp_wsa));
     size_t i[1];

     *i = *cnt;
     *err = 0;

     if (mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE) {
	  fprintf(stderr, "ERROR: mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE\n");
	  *err = -WSMP_EMODE;
	  goto out;
     }

     if (len < WSMP_WSA_MIN || len > WSMP_MAXSIZE) {
	  fprintf(stderr, "ERROR: len < WSMP_WSA_MIN || len > WSMP_MAXSIZE\n");
	  *err = -WSMP_EFAULT;
	  goto out;
     }

     uint8_t tmp = 0;

     _g(msg, i, &tmp, len, err);

     ret->version = tmp >> 4;

     if (ret->version != WSMP_VERSION) {
	  fprintf(stderr, "ERROR: ret->version != WSMP_VERSION\n");
	  *err = -WSMP_ENOSUPPORT;
	  goto out;
     }

     uint8_t hoi = tmp & 0x0F;

     ret->use_iex = (hoi & 0x08) >> 3;
     ret->sii_count = (hoi & 0x04) >> 2;
     ret->cii_count = (hoi & 0x02) >> 1;
     ret->use_wra = (hoi &0x01);

     _g(msg, i, &tmp, len, err);

     ret->id = tmp >> 4;
     ret->content_count = tmp & 0x0F;

     if (ret->use_iex) {
	  size_t iex_cnt = *i;
	  struct wsmp_iex *iex = wsmp_iex_decode(msg, &iex_cnt, len, err, mode);

	  *i = iex_cnt;
	  ret->iex = iex;
     }

     int j = 0;

     if (ret->sii_count) {
	  _g_c(msg, i, &ret->sii_count, len, err);
	  ret->sis = calloc(ret->sii_count, sizeof(struct wsmp_sii));

	  for (j = 0; j < ret->sii_count; j++) {
	       size_t sii_cnt = *i;
	       struct wsmp_sii *sii = wsmp_sii_decode(msg, &sii_cnt, len, err, mode);

	       *i = sii_cnt;
	       ret->sis[j] = sii;
	  }
     }

     if (ret->cii_count) {
	  _g_c(msg, i, &ret->cii_count, len, err);
	  ret->cis = calloc(ret->cii_count, sizeof(struct wsmp_cii));

	  for (j = 0; j < ret->cii_count; j++) {
	       size_t cii_cnt = *i;
	       struct wsmp_cii *cii = wsmp_cii_decode(msg, &cii_cnt, len, err, mode);

	       *i = cii_cnt;
	       ret->cis[j] = cii;
	  }
     }

     if (ret->use_wra) {
	  size_t wra_cnt = *i;
	  struct wsmp_wra *wra = wsmp_wra_decode(msg, &wra_cnt, len, err, mode);

	  *i = wra_cnt;
	  ret->wra = wra;
     }

     /* if (*i != len) { */
     /*		  fprintf(stderr, "ERROR: *i != len\n"); */
     /*		  *err = -WSMP_EFAULT; */
     /*		  goto out; */
     /* } */

out:
     *cnt = *i;
     return ret;
}

struct wsmp_wsm *wsmp_wsm_decode(const uint8_t *msg, size_t *cnt, size_t len, int *err, int mode) {
     struct wsmp_wsm *ret = calloc(1, sizeof(struct wsmp_wsm));
     size_t i[1];

     *i = *cnt;
     *err = 0;

     if (mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE) {
	  fprintf(stderr, "ERROR: mode != WSMP_STRICT && mode != WSMP_LAX && mode != WSMP_LOOSE\n");
	  *err = -WSMP_EMODE;
	  goto out;
     }

     if (len < WSMP_WSM_MIN || len > WSMP_MAXSIZE) {
	  fprintf(stderr, "ERROR: len < WSMP_WSM_MIN || len > WSMP_MAXSIZE\n");
	  *err = -WSMP_EFAULT;
	  goto out;
     }

     uint8_t tmp = 0;

     _g(msg, i, &tmp, len, err);

     ret->subtype = tmp >> 4;
     ret->use_n_iex = (tmp & 0x0F) >> 3;
     ret->version = tmp & 0x07;

     if (mode != WSMP_LOOSE && ret->subtype > 4) {
	  fprintf(stderr, "ERROR: mode != WSMP_LOOSE && curs->subtype > 4\n");
	  *err = -WSMP_EDOMAIN;
	  goto out;
     } else if (ret->version != WSMP_VERSION) {
	  fprintf(stderr, "ERROR: ret->version != WSMP_VERSION\n");
	  *err = WSMP_ENOSUPPORT;
	  goto out;
     }

     if (ret->use_n_iex) {
	  size_t n_iex_cnt = *i;
	  struct wsmp_iex *n_iex = wsmp_iex_decode(msg, &n_iex_cnt, len, err, mode);

	  *i = n_iex_cnt;
	  ret->n_iex = n_iex;
     }

     _g(msg, i, &tmp, len, err);

     ret->tpid = tmp;

     if (ret->tpid > 5) {
	  fprintf(stderr, "ERROR: ret->tpid > 5\n");
	  *err = -WSMP_ENOSUPPORT;
	  goto out;
     }

     if (ret->tpid < 2) {
	  _g_p(msg, i, &ret->psid, len, err);
     } else {
	  _g_n(msg, i, 2, ret->ports.src, len, err);
	  _g_n(msg, i, 2, ret->ports.dst, len, err);
     }

     if (ret->tpid % 2 == 1) {
	  ret->use_t_iex = 1;

	  size_t t_iex_cnt = *i;
	  struct wsmp_iex *t_iex = wsmp_iex_decode(msg, &t_iex_cnt, len, err, mode);

	  *i = t_iex_cnt;
	  ret->t_iex = t_iex;
     }

     _g_c(msg, i, &ret->len, len, err);

     if (mode == WSMP_STRICT && ret->len == 0) {
	  fprintf(stderr, "ERROR: mode == WSMP_STRICT && ret->len == 0\n");
	  *err = -WSMP_EDOMAIN;
	  goto out;
     }

     ret->data = calloc(ret->len, 1);

     _g_n(msg, i, ret->len, ret->data, len, err);

     /* if (*i != len) { */
     /*		  fprintf(stderr, "ERROR: *i != len\n"); */
     /*		  *err = -WSMP_EFAULT; */
     /*		  goto out; */
     /* } */

out:
     *cnt = *i;
     return ret;
}
