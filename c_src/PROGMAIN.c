/*
	PROGMAIN.c

	Copyright (C) 2003 Philip Cummins, Paul Pratt

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
	PROGram MAIN

	Contains the platform independent main routine, "ProgramMain".
	Called from platform dependent code in the OSGLUxxx.c files.

	This code is a distant descendent of code in vMac by Philip Cummins.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"
#include "MYOSGLUE.h"
#include "GLOBGLUE.h"
#include "MINEM68K.h"
#endif

#include "PROGMAIN.h"

IMPORTPROC Memory_Reset(void);

LOCALPROC vSonyEjectAllDisks(void)
{
	si4b i;

	for (i = 0; i < NumDrives; ++i) {
		if (vSonyIsInserted(i)) {
			(void) vSonyEject(i);
		}
	}
	vSonyWritableMask = 0;
	vSonyInsertedMask = 0;
	vSonyMountedMask = 0;
}

GLOBALPROC DoMacReset(void)
{
	vSonyEjectAllDisks();
	Memory_Reset();
	m68k_reset();
}

#define InstructionsPerTick 12250
	/*
		This a bit too fast on average, but
		if this was much lower, Concertware wouldn't
		work properly with speed limit on. If this was
		much higher, the initial sounds in Dark Castle
		would have static.
		This can only be an approximation, since on
		a real machine the number of instructions
		executed per time can vary by almost a factor
		of two, because different instructions take
		different times.
	*/

#define InstructionsPerSubTick (InstructionsPerTick / kNumSubTicks)

LOCALPROC MainEventLoop(void)
{
	long KiloInstructionsCounter = 0;
	blnr OverDue;

	do {
		if (KiloInstructionsCounter < kNumSubTicks) {
			m68k_go_nInstructions(InstructionsPerSubTick);
			SubTickNotify(KiloInstructionsCounter);
			++KiloInstructionsCounter;
		} else if (! SpeedLimit) {
			m68k_go_nInstructions(2048);
		}

		OverDue = (KiloInstructionsCounter >= kNumSubTicks);
		CheckIntSixtieth(SpeedLimit && OverDue && (0 == TimeAdjust));

		if (TimeAdjust > 0) {
			blnr WantNextSixtieth;

			if (TimeAdjust <= 6) {
				WantNextSixtieth = OverDue;
			} else {
				/* emulation lagging, try to catch up */
				WantNextSixtieth = (KiloInstructionsCounter
					>= (kNumSubTicks / 2));
				if (TimeAdjust > 8) {
					/* emulation not fast enough */
					TimeAdjust = 8;
				}
			}

			if (WantNextSixtieth) {
				while (KiloInstructionsCounter < kNumSubTicks) {
					SubTickNotify(KiloInstructionsCounter);
					++KiloInstructionsCounter;
				}
				KiloInstructionsCounter = 0;
				SixtiethSecondNotify();
				--TimeAdjust;
			}
		}
	} while (! RequestMacOff);
}

GLOBALPROC ProgramMain(void)
{
	ZapProgramVars();
	if (InitProgram()) {
		MainEventLoop();
	}
	UnInitProgram();
}
