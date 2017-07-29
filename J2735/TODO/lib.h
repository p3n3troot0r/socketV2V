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

#ifndef J2735_LIB_H
#define J2735_LIB_H

#include <stdio.h>
#include <linux/wsmp.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

typedef unsigned char __u8;

void print_usage(char *usg);

/* Parse command line arguements to make sure they make sense 
 * don't parse the frame - that gets done in kernel space
 * msg - output - serialized arguments 
 */
int parse_v2v_msg(char *args, __u32 *v2vmsg, int argc);

/* Print WSM message observed by COM
 * @buf input - serialized msg from kernel mod
 * @len input - length of msg
 * @cs input - flags about msg
 */
//void sprint_wsmframe(char *cs, char *buf, int len);

void print_iex(struct wsmp_iex *iex);
void print_wsm(struct wsmp_wsm *wsm);
void print_sii(struct wsmp_sii *sii);
void print_cii(struct wsmp_cii *cii);
void print_wra(struct wsmp_wra *wra);
void print_wsa(struct wsmp_wsa *wsa);

#endif