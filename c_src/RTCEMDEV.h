/*
	RTCEMDEV.h
	
	Copyright (C) 2001 Philip Cummins, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#ifdef RTCEMDEV_H
#error "header already included"
#else
#define RTCEMDEV_H
#endif

extern blnr RTC_Init(void);
extern void RTC_Interrupt(void);

// VIA Interface Functions

extern UBYTE VIA_GORB2 (void); // RTC Enable
extern void  VIA_PORB2 (UBYTE Data);

extern UBYTE VIA_GORB1 (void); // RTC Data Clock
extern void  VIA_PORB1 (UBYTE Data);

extern UBYTE VIA_GORB0 (void); // RTC Data
extern void  VIA_PORB0 (UBYTE Data);
