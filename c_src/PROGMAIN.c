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
	
	The main event loop for Mini vMac.
	
	This code is a distant descendent of code in vMac by Philip Cummins.
*/

#include "SYSDEPNS.h"

#include "PROGMAIN.h"

#include "MINEM68K.h"
#include "OSGLUSTB.h"

extern void ZapGlobalVars(void);
extern Boolean InitProgram(void);
extern void SixtiethSecondNotify(void);
extern void OneSecondNotify(void);

static Boolean ProgramDone;

void SetProgramDone(void)
{
	ProgramDone = true;
}

static void MainEventLoop(void)
{
	int TicksLeftInSecond = 60;
	long KiloInstructionsCounter = 0;

	ProgramDone = false;
	do {
		// Clock = 7833600 Hz (7.8336 mHz)
		// At best, we can get 3916800 instructions per second
		// Normally, it's probably a lot lower than that, say
		// 783360 instructions/sec if there's 10 cycles per instr.

		m68k_go_nInstructions(1024);
		++KiloInstructionsCounter;

		if (SpeedLimit && (KiloInstructionsCounter >= 12)) {
			WaitForIntSixtieth();
		}
		
		if (CheckIntSixtieth()) {
			if (KiloInstructionsCounter < 6) {
				/*
					if haven't executed enough instructions
					yet, skip this interupt, even though
					this will mess up timing.
				*/
			} else {
				KiloInstructionsCounter = 0;
				SixtiethSecondNotify();
				if (--TicksLeftInSecond <= 0) {
					OneSecondNotify();
					TicksLeftInSecond = 60;
				}
			}
		}
	} while (! ProgramDone);
}

void ProgramMain(void)
{
	ZapGlobalVars();
	if (InitProgram()) {
		m68k_reset();
		MainEventLoop();
	}
	UnInitProgram();
}
