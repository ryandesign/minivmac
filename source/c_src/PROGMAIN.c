/*
	PROGMAIN.c

	Copyright (C) 2006 Philip Cummins, Paul C. Pratt

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

	Contains the platform independent main routines
		"DoEmulateOneTick" and "DoEmulateExtraTime".
	Called from platform dependent code in the OSGLUxxx.c files.

	This code is a distant descendent of code in vMac by Philip Cummins.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"
#include "MYOSGLUE.h"
#include "GLOBGLUE.h"
#include "ADDRSPAC.h"
#endif

#include "PROGMAIN.h"

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
	EmulatedHardwareZap();
}

#define InstructionsPerSubTick (InstructionsPerTick / kNumSubTicks)

LOCALVAR ui5b ExtraSubTicksToDo = 0;

GLOBALPROC DoEmulateOneTick(void)
{
	long KiloInstructionsCounter = 0;

	SixtiethSecondNotify();

	do {
		m68k_go_nInstructions_1(InstructionsPerSubTick);
		SubTickNotify(KiloInstructionsCounter);
		++KiloInstructionsCounter;
	} while (KiloInstructionsCounter < kNumSubTicks);

	if (! SpeedLimit) {
		ExtraSubTicksToDo = (ui5b) -1;
	} else {
		ui5b ExtraAdd = (kNumSubTicks << SpeedValue) - kNumSubTicks;
		ui5b ExtraLimit = ExtraAdd << 3;

		ExtraSubTicksToDo += ExtraAdd;
		if (ExtraSubTicksToDo > ExtraLimit) {
			ExtraSubTicksToDo = ExtraLimit;
		}
	}
}

GLOBALPROC DoEmulateExtraTime(void)
{
	if (ExtraTimeNotOver() && (ExtraSubTicksToDo > 0)) {
		ExtraTimeBeginNotify();
		do {
			m68k_go_nInstructions_1(InstructionsPerSubTick);
			--ExtraSubTicksToDo;
		} while (ExtraTimeNotOver() && (ExtraSubTicksToDo > 0));
		ExtraTimeEndNotify();
	}
}
