/*
	SCCEMDEV.h
	
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

#ifdef SCCEMDEV_H
#error "header already included"
#else
#define SCCEMDEV_H
#endif

void  SCC_Reset(void);

extern void SCC_Access(CPTR addr);

// VIA Interface Headers

extern UBYTE VIA_GORA7(void);
extern void  VIA_PORA7(UBYTE Data);

extern blnr Mouse_Enabled(void);
