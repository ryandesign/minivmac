/*
	VIAEMDEV.h

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

#ifdef VIAEMDEV_H
#error "header already included"
#else
#define VIAEMDEV_H
#endif

// PC - VIA Memory Handlers

EXPORTVAR(blnr, VIAInterruptRequest);

EXPORTPROC VIA_Reset(void);

EXPORTPROC VIA_Access(CPTR addr);

EXPORTPROC VIA_Timer(void);
EXPORTPROC VIA_Int_Vertical_Blanking(void);
EXPORTPROC VIA_Int_One_Second(void);

EXPORTPROC GotKeyBoardData(UBYTE v);
