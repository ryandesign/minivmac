/*
	RTCEMDEV.h

	Copyright (C) 2002 Philip Cummins, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

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

EXPORTFUNC blnr RTC_Init(void);
EXPORTPROC RTC_Interrupt(void);

// VIA Interface Functions

EXPORTFUNC ui3b VIA_GORB2 (void); // RTC Enable
EXPORTPROC VIA_PORB2(ui3b Data);

EXPORTFUNC ui3b VIA_GORB1 (void); // RTC Data Clock
EXPORTPROC VIA_PORB1(ui3b Data);

EXPORTFUNC ui3b VIA_GORB0 (void); // RTC Data
EXPORTPROC VIA_PORB0(ui3b Data);
