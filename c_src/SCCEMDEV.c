/*
	SCCEMDEV.c
	
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

/*
	Serial Communications Controller EMulated DEVice
	
	Emulates the Z8530 SCC found in the Mac Plus.

	This code adapted from "SCC.c" in vMac by Philip Cummins.
*/

#include "SYSDEPNS.h"
#include "SCCEMDEV.h"

typedef struct
{
  UBYTE RR[16];
  UBYTE WR[16];
} Channel_Ty;

typedef struct
{
  Channel_Ty A;
  Channel_Ty B;
} SCC_Ty;

static SCC_Ty SCC;
static UBYTE SCC_Mode;
static UBYTE SCC_Reg;

blnr Mouse_Enabled(void)
{
	return ((SCC.A.WR[9] & 0x08) != 0x00);
}

void  SCC_Reset (void)
{
  int Counter;
  
  SCC_Mode = SCC_Reg = 0;
  
  for (Counter = 0; Counter < 16; Counter++)
  {
    SCC.A.RR[Counter] = 0;
    SCC.B.RR[Counter] = 0;
    SCC.A.WR[Counter] = 0;
    SCC.B.WR[Counter] = 0;
  }
  SCC.A.RR[0] = 0x04;
  SCC.B.RR[0] = 0x04;
}

static UBYTE SCC_GetAReg (void)
{
  return SCC.A.RR[SCC_Reg];
}

static void  SCC_PutAReg (UBYTE in)
{
  SCC.A.WR[SCC_Reg] = in;
}

static UBYTE SCC_GetBReg (void)
{
  return SCC.B.RR[SCC_Reg];
}

static void  SCC_PutBReg (UBYTE in)
{
  SCC.B.WR[SCC_Reg] = in;
}

extern ULONG DataBus;
extern blnr ByteSizeAccess;
extern blnr WriteMemAccess;

void SCC_Access(CPTR addr)
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

UBYTE VIA_GORA7 (void)
{
  return 1; // No Wait/Requests
}

void  VIA_PORA7 (UBYTE Data)
{
	UnusedParam(Data);
  #ifdef _VIA_Interface_Debug
  printf("VIA ORA7 attempts to be an output\n");
  #endif
}
