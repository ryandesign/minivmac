/*
	GLOBGLUE.c

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

/*
	GLOBal GLUE (or GLOB of GLUE)
	
	Holds the program together.

	Some code here adapted from "custom.c" in vMac by Philip Cummins,
	in turn descended from code in the Un*x Amiga Emulator by
	Bernd Schmidt.
*/

#include "SYSDEPNS.h"

#include "GLOBGLUE.h"

#include "OSGLUSTB.h"
#include "ADDRSPAC.h"
#include "IWMEMDEV.h"
#include "ROMEMDEV.h"
#include "RTCEMDEV.h"
#include "SCCEMDEV.h"
#include "SCSIEMDV.h"
#include "SCRNEMDV.h"
#include "SONYEMDV.h"
#include "VIAEMDEV.h"
#include "PROGMAIN.h"
#include "MOUSEMDV.h"
#include "KBRDEMDV.h"

void ZapProgramVars(void)
{
	ZapNMemoryVars();
}

blnr InitProgram(void)
{
	if (RTC_Init())
	if (ROM_Init())
	{
		return trueblnr;
	}
	return falseblnr;
}

void UnInitProgram(void)
{
}

void customreset(void)
{
	IWM_Reset();
	SCC_Reset();
	SCSI_Reset();
	VIA_Reset();
	Memory_Reset();
	Sony_Reset();
}

void SixtiethSecondNotify(void)
{
	VIA_Timer();
    Screen_Draw();
    Mouse_Update2();
    KeyBoard_Update();

    VIA_Int_Vertical_Blanking();
	Sony_Update();
}

void OneSecondNotify(void)
{
	RTC_Interrupt();
	VIA_Int_One_Second();
}
