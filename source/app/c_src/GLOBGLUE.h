/*
	GLOBGLUE.h

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

#ifdef GLOBGLUE_H
#error "header already included"
#else
#define GLOBGLUE_H
#endif

EXPORTPROC EmulationReserveAlloc(void);
EXPORTFUNC blnr InitEmulation(void);
EXPORTPROC customreset(void);
EXPORTPROC EmulatedHardwareZap(void);
EXPORTPROC SixtiethSecondNotify(void);
EXPORTPROC SubTickNotify(int SubTick);
EXPORTPROC ExtraTimeBeginNotify(void);
EXPORTPROC ExtraTimeEndNotify(void);

#define RAMSafetyMarginFudge 4

#define kRAM_Size (kRAMa_Size + kRAMb_Size)
EXPORTVAR(ui3p, RAM)
	/*
		allocated by MYOSGLUE to be at least kRAM_Size + RAMSafetyMarginFudge
		bytes. Because of shortcuts taken in ADDRSPAC.c, it is in theory
		possible for the emulator to write up to 3 bytes past kRAM_Size.
	*/

#if EmVidCard
EXPORTVAR(ui3p, VidROM)
#endif

#if IncludeVidMem
EXPORTVAR(ui3p, VidMem)
#endif
