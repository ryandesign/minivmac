/*
	ADDRSPAC.h

	Copyright (C) 2002 Bernd Schmidt, Philip Cummins, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

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

EXPORTFUNC ui5b get_long(CPTR addr);
EXPORTFUNC ui5b get_word(CPTR addr);
EXPORTFUNC ui5b get_byte(CPTR addr);
EXPORTPROC put_long(CPTR addr, ui5b l);
EXPORTPROC put_word(CPTR addr, ui5b w);
EXPORTPROC put_byte(CPTR addr, ui5b b);

EXPORTFUNC ui3b *get_real_address(CPTR addr);

EXPORTVAR(CPTR, AddressBus)
EXPORTVAR(ui5b, DataBus)
EXPORTVAR(blnr, ByteSizeAccess)
EXPORTVAR(blnr, WriteMemAccess)

// VIA Interface Functions

EXPORTFUNC ui3b VIA_GORA4(void); // Overlay/Normal Memory Mapping
EXPORTPROC VIA_PORA4(ui3b Data);
