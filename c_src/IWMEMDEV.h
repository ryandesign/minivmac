/*
	IWMEVDEV.h

	Copyright (C) 2002 Philip Cummins, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#ifdef IWMEVDEV_H
#error "header already included"
#else
#define IWMEVDEV_H
#endif

EXPORTPROC IWM_Reset(void);

EXPORTPROC IWM_Access(CPTR addr);

// VIA Interface Headers

EXPORTFUNC UBYTE VIA_GORA5(void); // Floppy Disk Line SEL
EXPORTPROC VIA_PORA5(UBYTE Data);
