#ifndef _WSMP_DECODE_H
#define _WSMP_DECODE_H

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

struct wsmp_iex *wsmp_iex_decode(const uint8_t *msg, size_t *cnt, size_t len, int *err, int mode);
struct wsmp_sii *wsmp_sii_decode(const uint8_t *msg, size_t *cnt, size_t len, int *err, int mode);
struct wsmp_cii *wsmp_cii_decode(const uint8_t *msg, size_t *cnt, size_t len, int *err, int mode);
struct wsmp_wra *wsmp_wra_decode(const uint8_t *msg, size_t *cnt, size_t len, int *err, int mode);
struct wsmp_wsa *wsmp_wsa_decode(const uint8_t *msg, size_t *cnt, size_t len, int *err, int mode);
struct wsmp_wsm *wsmp_wsm_decode(const uint8_t *msg, size_t *cnt, size_t len, int *err, int mode);

#endif /* _WSMP_DECODE_H */
