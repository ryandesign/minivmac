/*
	SCCEMDEV.c

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

/*
	Serial Communications Controller EMulated DEVice

	Emulates the Z8530 SCC found in the Mac Plus.

	This code adapted from "SCC.c" in vMac by Philip Cummins.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"

#include "ADDRSPAC.h"
#endif

#include "SCCEMDEV.h"

typedef struct {
	ui3b RR[16];
	ui3b WR[16];
} Channel_Ty;

typedef struct {
	Channel_Ty A;
	Channel_Ty B;
} SCC_Ty;

LOCALVAR SCC_Ty SCC;
LOCALVAR ui3b SCC_Mode;
LOCALVAR ui3b SCC_Reg;

EXPORTFUNC blnr Mouse_Enabled(void)
{
	return ((SCC.A.WR[9] & 0x08) != 0x00);
}

GLOBALPROC SCC_Reset(void)
{
	int Counter;

	SCC_Mode = SCC_Reg = 0;

	for (Counter = 0; Counter < 16; Counter++) {
		SCC.A.RR[Counter] = 0;
		SCC.B.RR[Counter] = 0;
		SCC.A.WR[Counter] = 0;
		SCC.B.WR[Counter] = 0;
	}
	SCC.A.RR[0] = 0x04;
	SCC.B.RR[0] = 0x04;
}

LOCALFUNC ui3b SCC_GetAReg(void)
{
	return SCC.A.RR[SCC_Reg];
}

LOCALPROC SCC_PutAReg(ui3b in)
{
	SCC.A.WR[SCC_Reg] = in;
}

LOCALFUNC ui3b SCC_GetBReg(void)
{
	return SCC.B.RR[SCC_Reg];
}

LOCALPROC SCC_PutBReg(ui3b in)
{
	SCC.B.WR[SCC_Reg] = in;
}

GLOBALPROC SCC_Access(CPTR addr)
{
	if (ByteSizeAccess) {
		if ((addr & 1) == 0) {
			switch (addr >> 1) {
				case 0 : // Channel B Control
					if (SCC_Mode == 0) {
						if (WriteMemAccess) {
							SCC_Mode = 1;
							SCC_Reg = DataBus & 0x0F;
							SCC_PutBReg(DataBus & 0xF0);
						} else {
							SCC_Reg = 0;
							DataBus = SCC_GetBReg();
						}
					} else {
						SCC_Mode = 0;
						if (WriteMemAccess) {
							SCC_PutBReg(DataBus);
						} else {
							DataBus = SCC_GetBReg();
						}
					}
					break;
				case 1 : // Channel A Control
					if (SCC_Mode == 0) {
						if (WriteMemAccess) {
							SCC_Mode = 1;
							SCC_Reg = DataBus & 0x0F;
							SCC_PutAReg(DataBus & 0xF0);
						} else {
							SCC_Reg = 0;
							DataBus = SCC_GetAReg();
						}
					} else {
						SCC_Mode = 0;
						if (WriteMemAccess) {
							SCC_PutAReg(DataBus);
						} else {
							DataBus = SCC_GetAReg();
						}
					}
					break;
				case 2 : // Channel B Data
					break;
				case 3 : // Channel A Data
					break;
			}
		} else {
			if (! WriteMemAccess) {
				SCC_Reset();
			}
		}
	} else {
#ifdef _SCC_Debug
		printf("Attempted Phase Adjust\n");
#endif
	}
}

// VIA Interface Functions

EXPORTFUNC ui3b VIA_GORA7(void)
{
	return 1; // No Wait/Requests
}

GLOBALPROC VIA_PORA7(ui3b Data)
{
	UnusedParam(Data);
#ifdef _VIA_Interface_Debug
	printf("VIA ORA7 attempts to be an output\n");
#endif
}
