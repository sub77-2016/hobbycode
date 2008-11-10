/* $Id: udelay.h,v 1.1 2007/05/21 02:35:20 elbriga Exp $
 *
 * short delays 
 *
 * Copyright (C) 1999, 2000 Michael Reinelt <reinelt@eunet.at>
 * Copyright (C) 2004 The LCD4Linux Team <lcd4linux-devel@users.sourceforge.net>
 *
 * This file is part of LCD4Linux.
 *
 * LCD4Linux is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * LCD4Linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * $Log: udelay.h,v $
 * Revision 1.1  2007/05/21 02:35:20  elbriga
 * Versão inicial. Modulo para delay de micro-segundos (us). Originalmente do projeto lcd4linux.
 *
 * Revision 1.10  2005/05/08 04:32:45  reinelt
 * CodingStyle added and applied
 *
 * Revision 1.9  2005/01/18 06:30:24  reinelt
 * added (C) to all copyright statements
 *
 * Revision 1.8  2004/06/26 12:05:00  reinelt
 *
 * uh-oh... the last CVS log message messed up things a lot...
 *
 * Revision 1.7  2004/06/26 09:27:21  reinelt
 *
 * added '-W' to CFLAGS
 * changed all C++ comments to C ones
 * cleaned up a lot of signed/unsigned mistakes
 *
 * Revision 1.6  2004/06/20 10:09:56  reinelt
 *
 * 'const'ified the whole source
 *
 * Revision 1.5  2003/10/05 17:58:50  reinelt
 * libtool junk; copyright messages cleaned up
 *
 * Revision 1.4  2002/04/29 11:00:28  reinelt
 *
 * added Toshiba T6963 driver
 * added ndelay() with nanosecond resolution
 *
 * Revision 1.3  2001/03/12 13:44:58  reinelt
 *
 * new udelay() using Time Stamp Counters
 *
 * Revision 1.2  2001/03/12 12:39:36  reinelt
 *
 * reworked autoconf a lot: drivers may be excluded, #define's went to config.h
 *
 * Revision 1.1  2000/04/15 16:56:52  reinelt
 *
 * moved delay loops to udelay.c
 * renamed -d (debugging) switch to -v (verbose)
 * new switch -d to calibrate delay loop
 * 'Delay' entry for HD44780 back again
 * delay loops will not calibrate automatically, because this will fail with high CPU load
 *
 */

#ifndef _UDELAY_H_
#define _UDELAY_H_

/* stolen from linux/asm-i386/processor.h */
/* REP NOP (PAUSE) is a good thing to insert into busy-wait loops. */
static inline void rep_nop(void)
{
    __asm__ __volatile__("rep; nop");
}


#ifdef USE_OLD_UDELAY

extern unsigned long loops_per_usec;
void udelay_calibrate(void);

#else

void udelay_init(void);

#endif

void ndelay(const unsigned long nsec);

#define udelay(usec) ndelay(usec*1000)

#endif
