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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include "lib.h"

#include <linux/wsmp.h>

/*
static const char *const usages[] = {
    "v2vsend <device> <J2735_MF> [options] [[--] args] <j2735_msg>",
    "v2vsend <device> <J2735_MF> [options] <j2735_msg>",
    NULL,
};
*/

int main(int argc, char **argv) {

	/* check command line options */
/*	TODO: arg parsing in C :( 
    if (argc < 4) {
		fprintf(stderr, "Usage: %s <interface> <J2735_MF> <args> \n", argv[0]);
		print_usg(argv);

		return 1;
	}

	int v2v_msg, opt;
    uint8_t *v2vmsg;
	v2v_msg = parse_v2v_msg(argv, &v2vmsg, argc`);
//  enum DSRCmsgID msgID;*/

    /* create test J2735 message */
    

	/* open UNIX socket to kernel module */

    /* configure socket */

    /* bind to socket */

    /* Get network interface info from socket */

    /* write to socket */

    /* close socket */

    return 0; 
}







