/*
	MINEM68K.h

	Copyright (C) 2001 Bernd Schmidt, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#ifdef MINEM68K_H
#error "header already included"
#else
#define MINEM68K_H
#endif

extern blnr ViaException(void);
extern void DiskInsertedPsuedoException(CPTR newpc, ULONG data);
extern void SetAutoVector(void);
extern void MacInterrupt (void);
extern void m68k_reset(void);

extern void m68k_go_nInstructions(ULONG n);
