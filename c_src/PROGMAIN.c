/*
	PROGMAIN.c

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
	PROGram MAIN

	Contains the platform independent main routine, "ProgramMain".
	Called from platform dependent code in the OSGLUxxx.c files.

	This code is a distant descendent of code in vMac by Philip Cummins.
*/

#include "SYSDEPNS.h"

#include "PROGMAIN.h"

#include "MINEM68K.h"
#include "OSGLUSTB.h"
#include "OSCOMVAR.h"

extern void ZapProgramVars(void);
extern blnr InitProgram(void);
extern void UnInitProgram(void);
extern void SixtiethSecondNotify(void);
extern void OneSecondNotify(void);

static void vSonyEjectAllDisks(void)
{
	WORD i;

	for (i = 0; i < NumDrives; ++i) {
		if (vSonyInserted(i)) {
			(void) vSonyEject(i);
		}
	}
}

static int TicksLeftInSecond = 60;
static blnr ProgramDone = falseblnr;

static void DoEach60th(void)
{
	if (RequestMacOff) {
		RequestMacOff = falseblnr;
		if ((! AnyDiskInserted()) ||
			OkCancelAlert("Are you sure you want to Quit?",
				"You should shut down the emulated machine before quiting Mini vMac to prevent data corruption."))
		{
			ProgramDone = trueblnr;
		}
	}
	if (RequestMacInterrupt) {
		RequestMacInterrupt = falseblnr;
		if (OkCancelAlert("Are you sure you want to Interrupt?",
				"This will invoke any installed debugger. But as yet, most debuggers do not work with Mini vMac."))
		{
			MacInterrupt();
		}
	}
	if (RequestMacReset) {
		RequestMacReset = falseblnr;
		if ((! AnyDiskInserted()) ||
			OkCancelAlert("Are you sure you want to Reset?",
				"Unsaved changes will be lost, and there is a risk of data corruption."))
		{
			vSonyEjectAllDisks();
			m68k_reset();
		}
	}
	SixtiethSecondNotify();
	if (--TicksLeftInSecond <= 0) {
		OneSecondNotify();
		TicksLeftInSecond = 60;
	}
}

static void MainEventLoop(void)
{
	long KiloInstructionsCounter = 0;

	do {
		m68k_go_nInstructions(1024);
		++KiloInstructionsCounter;

		if (CheckIntSixtieth(KiloInstructionsCounter >= 12)) {
			if (KiloInstructionsCounter < 6) {
				/*
					if haven't executed enough instructions
					yet, skip this interupt, even though
					this will mess up emulated clock.
				*/
			} else {
				KiloInstructionsCounter = 0;
				DoEach60th();
			}
		}
	} while (! ProgramDone);
}

void ProgramMain(void)
{
	ZapProgramVars();
	if (InitProgram()) {
		m68k_reset();
		MainEventLoop();
	}
	UnInitProgram();
}
