/*
	SCCEMDEV.h

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

#ifdef SCCEMDEV_H
#error "header already included"
#else
#define SCCEMDEV_H
#endif

EXPORTPROC SCC_Reset(void);

EXPORTPROC SCC_Access(CPTR addr);

// VIA Interface Headers

EXPORTFUNC UBYTE VIA_GORA7(void);
EXPORTPROC VIA_PORA7(UBYTE Data);

EXPORTFUNC blnr Mouse_Enabled(void);
