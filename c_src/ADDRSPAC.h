/*
	ADDRSPAC.h

	Copyright (C) 2002 Bernd Schmidt, Philip Cummins, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#ifdef ADDRSPAC_H
#error "header already included"
#else
#define ADDRSPAC_H
#endif

EXPORTPROC Memory_Reset(void);
EXPORTPROC ZapNMemoryVars(void);

EXPORTFUNC ULONG get_long(CPTR addr);
EXPORTFUNC ULONG get_word(CPTR addr);
EXPORTFUNC ULONG get_byte(CPTR addr);
EXPORTPROC put_long(CPTR addr, ULONG l);
EXPORTPROC put_word(CPTR addr, ULONG w);
EXPORTPROC put_byte(CPTR addr, ULONG b);

EXPORTFUNC UBYTE *get_real_address(CPTR addr);

EXPORTVAR(CPTR, AddressBus);
EXPORTVAR(ULONG, DataBus);
EXPORTVAR(blnr, ByteSizeAccess);
EXPORTVAR(blnr, WriteMemAccess);

// VIA Interface Functions

EXPORTFUNC UBYTE VIA_GORA4(void); // Overlay/Normal Memory Mapping
EXPORTPROC VIA_PORA4(UBYTE Data);
