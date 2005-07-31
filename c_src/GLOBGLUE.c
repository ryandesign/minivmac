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
#endif

#include "GLOBGLUE.h"

IMPORTFUNC blnr RTC_Init(void);
IMPORTFUNC blnr ROM_Init(void);
IMPORTFUNC blnr AddrSpac_Init(void);

IMPORTPROC VIA_Zap(void);

IMPORTPROC m68k_reset(void);
IMPORTPROC IWM_Reset(void);
IMPORTPROC SCC_Reset(void);
IMPORTPROC SCSI_Reset(void);
IMPORTPROC VIA_Reset(void);
IMPORTPROC Memory_Reset(void);
IMPORTPROC Sony_Reset(void);

IMPORTPROC Mouse_Update(void);
IMPORTPROC KeyBoard_Update(void);
IMPORTPROC VIA_Int_Vertical_Blanking(void);
IMPORTPROC Sony_Update(void);

IMPORTPROC RTC_Interrupt(void);

IMPORTPROC MacSound_SubTick(int SubTick);

IMPORTPROC VIA_ExtraTimeBegin(void);
IMPORTPROC VIA_ExtraTimeEnd(void);

GLOBALFUNC blnr InitEmulation(void)
{
	if (RTC_Init())
	if (ROM_Init())
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
	VIA_Zap();
	Sony_Reset();
	m68k_reset();
}

GLOBALPROC customreset(void)
{
	IWM_Reset();
	SCC_Reset();
	SCSI_Reset();
	VIA_Reset();
	Sony_Reset();
}

GLOBALPROC SixtiethSecondNotify(void)
{
	Mouse_Update();
	KeyBoard_Update();

	VIA_Int_Vertical_Blanking();
	Sony_Update();

	RTC_Interrupt();
}

GLOBALPROC SubTickNotify(int SubTick)
{
#if MySoundEnabled
	MacSound_SubTick(SubTick);
#endif
}

GLOBALPROC ExtraTimeBeginNotify(void)
{
	VIA_ExtraTimeBegin();
}

GLOBALPROC ExtraTimeEndNotify(void)
{
	VIA_ExtraTimeEnd();
}
