/*
	MOUSEMDV.c

	Copyright (C) 2002 Philip Cummins, Paul Pratt

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
	MOUSe EMulated DeVice

	Emulation of the mouse in the Mac Plus.

	This code descended from "Mouse-MacOS.c" in Richard F. Bannister's
	macintosh port of vMac, by Philip Cummins.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"
#include "MYOSGLUE.h"
#include "ADDRSPAC.h"
#include "SCCEMDEV.h"
#endif

#include "MOUSEMDV.h"

LOCALVAR UBYTE X2 = 0;
LOCALVAR UBYTE Y2 = 0;

#if 0
LOCALFUNC unsigned int mapmouse0(unsigned int i)
{
	if (i < 8) {
		return 0;
	} else if (i < 24) {
		return (i - 8) / 4 + 2;
	} else if (i < 40) {
		return (i - 24) / 2 + 4 + 2;
	} else {
		return (i - 40) + 12 + 2;
	}
}

LOCALFUNC int mapmouse(int i)
{
	if (i >= 0) {
		return mapmouse0(i);
	} else {
		return -mapmouse0(-i);
	}
}

LOCALVAR int mousedeltav = 0;
LOCALVAR int mousedeltah = 0;
#endif

GLOBALPROC Mouse_Update2(void)
{
	ULONG NewMouse;

	if (Mouse_Enabled()) { // SCC activated yet
#if 1
		NewMouse = (CurMouseV << 16) | CurMouseH;

		if (get_long(0x0828) != NewMouse) {
			put_long(0x0828, NewMouse); // Set Mouse Position
			put_long(0x082C, NewMouse);
			put_long(0x0830, NewMouse);
			put_byte(0x08CE, 0xFF); // Tell MacOS to redraw the Mouse
		}
#else
		UWORD CurMacMouseV = get_word(0x82C);
		UWORD CurMacMouseH = get_word(0x82E);
		WORD dv = CurMouseV - CurMacMouseV;
		WORD dh = CurMouseH - CurMacMouseH;

		if ((dv != 0) || (dh != 0)) {
			UWORD CrsrThresh = get_word(0x8Ec);
			blnr adv = dv;
			blnr adh = dh;
#if 0
			if (dv < 0) {
				adv = -adv;
			}
			if (dh < 0) {
				adh = -adh;
			}
			if (adv + adh >= CrsrThresh) {
				adv /= 2;
				adh /= 2;
			}
			if (dv < 0) {
				adv = -adv;
			}
			if (dh < 0) {
				adh = -adh;
			}
#endif
			mousedeltav += mapmouse(CurMouseV - (vMacScreenHeight / 2));
			mousedeltah += mapmouse(CurMouseH - (vMacScreenWidth / 2));
			{
				int nextv = mousedeltav / 32;
				int nexth = mousedeltah / 32;
				mousedeltav -= nextv * 32;
				mousedeltah -= nexth * 32;
				put_word(0x0828, get_word(0x0828) + /* adv */nextv);
				put_word(0x082A, get_word(0x082A) + /* adh */nexth);
				put_byte(0x08CE, 0xFF); // Tell MacOS to redraw the Mouse
			}
		}
#endif
	}
}

// VIA Interface Functions

GLOBALFUNC UBYTE VIA_GORB5(void) // Mouse Y2
{
	return Y2;
}

GLOBALFUNC UBYTE VIA_GORB4(void) // Mouse X2
{
	return X2;
}

GLOBALFUNC UBYTE VIA_GORB3(void) // Mouse Button
{
	return ! CurMouseButton;
}

GLOBALPROC VIA_PORB5(UBYTE Data)
{
	UnusedParam(Data);
#ifdef _VIA_Interface_Debug
	printf("VIA ORB5 attempts to be an output\n");
#endif
}

GLOBALPROC VIA_PORB4(UBYTE Data)
{
	UnusedParam(Data);
#ifdef _VIA_Interface_Debug
	printf("VIA ORB4 attempts to be an output\n");
#endif
}

GLOBALPROC VIA_PORB3(UBYTE Data)
{
	UnusedParam(Data);
#ifdef _VIA_Interface_Debug
	printf("VIA ORB3 attempts to be an output\n");
#endif
}
