/*
	MINEM68K.h

	Copyright (C) 2004 Bernd Schmidt, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

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

#define ln2TotAddrBytes 24

#define ln2BytesPerMemBank 17
#define ln2NumMemBanks (ln2TotAddrBytes - ln2BytesPerMemBank)

#define NumMemBanks (1UL << ln2NumMemBanks)
#define BytesPerMemBank  (1UL << ln2BytesPerMemBank)
#define MemBanksMask (NumMemBanks - 1)
#define MemBankAddrMask (BytesPerMemBank - 1)

#define bankindex(addr) ((((CPTR)(addr)) >> ln2BytesPerMemBank) & MemBanksMask)

TYPEDEFFUNC ui5b (* tMM_Access)(ui5b Data, blnr WriteMem, blnr ByteSize, CPTR addr);
TYPEDEFPROC (* tCustomReset)(void);

EXPORTPROC MINEM68K_Init(ui3b **BankReadAddr, ui3b **BankWritAddr,
	ui3b *fIPL);

EXPORTPROC m68k_IPLchangeNtfy(void);
EXPORTPROC DiskInsertedPsuedoException(CPTR newpc, ui5b data);
EXPORTPROC m68k_reset(void);

EXPORTPROC m68k_go_nInstructions(ui5b n);
