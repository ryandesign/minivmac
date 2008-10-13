/*
	GLOBGLUE.c

	Copyright (C) 2003 Bernd Schmidt, Philip Cummins, Paul C. Pratt

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
	GLOBal GLUE (or GLOB of GLUE)

	Holds the program together.

	Some code here adapted from "custom.c" in vMac by Philip Cummins,
	in turn descended from code in the Un*x Amiga Emulator by
	Bernd Schmidt.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"

#include "MYOSGLUE.h"
#include "EMCONFIG.h"
#endif

#include "GLOBGLUE.h"

#if EmRTC
IMPORTFUNC blnr RTC_Init(void);
#endif
IMPORTFUNC blnr ROM_Init(void);
#if (CurEmMd == kEmMd_II)
IMPORTFUNC blnr Vid_Init(void);
#endif
IMPORTFUNC blnr AddrSpac_Init(void);

IMPORTPROC VIA1_Zap(void);
#if EmVIA2
IMPORTPROC VIA2_Zap(void);
#endif

IMPORTPROC m68k_reset(void);
IMPORTPROC IWM_Reset(void);
IMPORTPROC SCC_Reset(void);
IMPORTPROC SCSI_Reset(void);
IMPORTPROC VIA1_Reset(void);
#if EmVIA2
IMPORTPROC VIA2_Reset(void);
#endif
IMPORTPROC Memory_Reset(void);
IMPORTPROC Sony_Reset(void);

IMPORTPROC Mouse_Update(void);
#if EmClassicKbrd
IMPORTPROC KeyBoard_Update(void);
#endif
#if EmADB
IMPORTPROC ADB_Update(void);
#endif
IMPORTPROC InterruptReset_Update(void);
IMPORTPROC VIA1_iCA1_PulseNtfy(void);
IMPORTPROC Sony_Update(void);
#if (CurEmMd == kEmMd_II)
IMPORTPROC Vid_Update(void);
#endif

#if EmRTC
IMPORTPROC RTC_Interrupt(void);
#endif

IMPORTPROC MacSound_SubTick(int SubTick);

IMPORTPROC VIA1_ExtraTimeBegin(void);
IMPORTPROC VIA1_ExtraTimeEnd(void);

#if EmVIA2
IMPORTPROC VIA2_ExtraTimeBegin(void);
IMPORTPROC VIA2_ExtraTimeEnd(void);
#endif

GLOBALFUNC blnr InitEmulation(void)
{
#if EmRTC
	if (RTC_Init())
#endif
	if (ROM_Init())
#if (CurEmMd == kEmMd_II)
	if (Vid_Init())
#endif
	if (AddrSpac_Init())
	{
		return trueblnr;
	}
	return falseblnr;
}

GLOBALPROC EmulatedHardwareZap(void)
{
	Memory_Reset();
	IWM_Reset();
	SCC_Reset();
	SCSI_Reset();
	VIA1_Zap();
#if EmVIA2
	VIA2_Zap();
#endif
	Sony_Reset();
	m68k_reset();
}

GLOBALPROC customreset(void)
{
	IWM_Reset();
	SCC_Reset();
	SCSI_Reset();
	VIA1_Reset();
#if EmVIA2
	VIA2_Reset();
#endif
	Sony_Reset();
#if CurEmMd <= kEmMd_128K
	WantMacReset = trueblnr;
	/*
		kludge, code in Finder appears
		to do RESET and not expect
		to come back. Maybe asserting
		the RESET somehow causes
		other hardware compenents to
		later reset the 68000.
	*/
#endif
}

GLOBALPROC SixtiethSecondNotify(void)
{
	Mouse_Update();
	InterruptReset_Update();
#if EmClassicKbrd
	KeyBoard_Update();
#endif
#if EmADB
	ADB_Update();
#endif

	VIA1_iCA1_PulseNtfy(); /* Vertical Blanking Interrupt */
	Sony_Update();

#if EmRTC
	RTC_Interrupt();
#endif
#if (CurEmMd == kEmMd_II)
	Vid_Update();
#endif
}

GLOBALPROC SubTickNotify(int SubTick)
{
#if MySoundEnabled && (CurEmMd != kEmMd_PB100)
	MacSound_SubTick(SubTick);
#else
	UnusedParam(SubTick);
#endif
}

GLOBALPROC ExtraTimeBeginNotify(void)
{
	VIA1_ExtraTimeBegin();
#if EmVIA2
	VIA2_ExtraTimeBegin();
#endif
}

GLOBALPROC ExtraTimeEndNotify(void)
{
	VIA1_ExtraTimeEnd();
#if EmVIA2
	VIA2_ExtraTimeEnd();
#endif
}
