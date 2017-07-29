#ifndef _WSMP_ENCODE_H
#define _WSMP_ENCODE_H

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

uint8_t *wsmp_iex_encode(const struct wsmp_iex *curs, size_t *cnt, int *err, int mode);
uint8_t *wsmp_sii_encode(const struct wsmp_sii *curs, size_t *cnt, int *err, int mode);
uint8_t *wsmp_cii_encode(const struct wsmp_cii *curs, size_t *cnt, int *err, int mode);
uint8_t *wsmp_wra_encode(const struct wsmp_wra *curs, size_t *cnt, int *err, int mode);
uint8_t *wsmp_wsa_encode(const struct wsmp_wsa *curs, size_t *cnt, int *err, int mode);
uint8_t *wsmp_wsm_encode(const struct wsmp_wsm *curs, size_t *cnt, int *err, int mode);

#endif /* _WSMP_ENCODE_H */
