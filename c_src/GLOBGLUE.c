/*
	GLOBGLUE.c

	Copyright (C) 2003 Bernd Schmidt, Philip Cummins, Paul Pratt

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
#include "MINEM68K.h"
#endif

#include "GLOBGLUE.h"

IMPORTPROC ZapNMemoryVars(void);

IMPORTFUNC blnr RTC_Init(void);
IMPORTFUNC blnr ROM_Init(void);
IMPORTFUNC blnr AddrSpac_Init(void);

IMPORTPROC IWM_Reset(void);
IMPORTPROC SCC_Reset(void);
IMPORTPROC SCSI_Reset(void);
IMPORTPROC VIA_Reset(void);
IMPORTPROC Memory_Reset(void);
IMPORTPROC Sony_Reset(void);

IMPORTPROC Screen_Draw(void);
IMPORTPROC Mouse_Update(void);
IMPORTPROC KeyBoard_Update(void);
IMPORTPROC VIA_Int_Vertical_Blanking(void);
IMPORTPROC Sony_Update(void);

IMPORTPROC RTC_Interrupt(void);

IMPORTPROC VIA_Timer(int SubTick);
IMPORTPROC MacSound_SubTick(int SubTick);

GLOBALPROC ZapProgramVars(void)
{
	ZapNMemoryVars();
}

GLOBALFUNC blnr InitProgram(void)
{
	if (RTC_Init())
	if (ROM_Init())
	if (AddrSpac_Init())
	{
		return trueblnr;
	}
	return falseblnr;
}

GLOBALPROC UnInitProgram(void)
{
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
	Screen_Draw();

	VIA_Int_Vertical_Blanking();
	Sony_Update();

	RTC_Interrupt();
}

GLOBALPROC SubTickNotify(int SubTick)
{
#if MySoundEnabled
	MacSound_SubTick(SubTick);
#endif
	VIA_Timer(SubTick);
}
