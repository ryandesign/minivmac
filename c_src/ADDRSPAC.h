/*
	ADDRSPAC.h

	Copyright (C) 2001 Bernd Schmidt, Philip Cummins, Paul Pratt

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

extern void  Memory_Reset (void);
extern void ZapNMemoryVars(void);

extern ULONG get_long(CPTR addr);
extern ULONG get_word(CPTR addr);
extern ULONG get_byte(CPTR addr);
extern void put_long(CPTR addr, ULONG l);
extern void put_word(CPTR addr, ULONG w);
extern void put_byte(CPTR addr, ULONG b);

extern UBYTE *get_real_address(CPTR addr);

extern CPTR AddressBus;
extern ULONG DataBus;
extern blnr ByteSizeAccess;
extern blnr WriteMemAccess;

// VIA Interface Functions

extern UBYTE VIA_GORA4 (void); // Overlay/Normal Memory Mapping
extern void  VIA_PORA4 (UBYTE Data);
