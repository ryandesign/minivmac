/*
	OSGIMPRT.h
	Copyright (C) 2001 Paul Pratt

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
	Operating System Glue IMPoRTs

	brings in everything that the Operating
	System Glue (for any platform) can
	use from the rest of the program.
*/

#ifdef OSGIMPRT_H
#error "header already included"
#else
#define OSGIMPRT_H
#endif

#include "OSCOMVAR.h"

extern void MacInterrupt (void);
extern void m68k_reset(void);

extern void vSonyInsertNotify (UWORD Drive_No);
extern void Keyboard_Down (ULONG Key);
extern void Keyboard_Up (ULONG Key);
extern void Keyboard_Auto (ULONG Key);

extern void ProgramMain(void);
extern void SetProgramDone(void);
