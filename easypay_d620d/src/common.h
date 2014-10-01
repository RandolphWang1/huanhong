/*  common.h
 *  Copyright (C) 2001 Yan "Warrior" Gurtovoy (ymg@dakotacom.net)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _COMMON_H
#include <termios.h>
#include <string.h>
#include <stdbool.h>
#include "esyslog.h"
#ifndef FALSE
#define FALSE       0
#endif
#ifndef TRUE
#define TRUE        1
#endif

typedef struct _tty_struct {
    int             ptyfd;
    int             portfd;
    int             posfd;
    int             logfd;
    int             ptyraw;
    int             portraw;
    int             ptypipefd[2];
    int             portpipefd[2];
    int             dspbytes;
    int             tstamp;
    int             nolock;
    char            *portName;
    char            *ptyName;
    char            *posName;
    struct termios  ptystate_orig;
    struct termios  portstate_orig;
    speed_t         baudrate;
    char            clr[8];
    char            bclr[8];
    char            tclr[8];
	bool            using_lp;
} tty_struct;

/*colors*/
typedef struct _clr_struct {
    char *name;
    char *color;
} clr_struct;

typedef struct _pos_gs_struct {
    unsigned char*data;
    int length;
} pos_gs_struct;
#define _COMMON_H
#endif
