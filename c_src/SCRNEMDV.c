/*
	SCRNEMDV.c
	
	Copyright (C) 2001 Philip Cummins, Richard F. Bannister, Paul Pratt

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
	SCReeN EMulated DeVice
	
	Emulation of the screen in the Mac Plus.

	This code descended from "Screen-MacOS.c" in Richard F. Bannister's
	macintosh port of vMac, by Philip Cummins.
*/

#include "SYSDEPNS.h"

#include "SCRNEMDV.h"

#include "OSGLUSTB.h"
#include "OSCOMVAR.h"

UBYTE vPage2 = 1;

static Boolean FindFirstChangeInLVecs(long *ptr1, long *ptr2,
					long L, long *j)
{
/*
	find index of first difference
*/
	long *p1 = ptr1;
	long *p2 = ptr2;
	long i;

	for (i = L; --i >= 0; ) {
		if (*p1++ != *p2++) {
			--p1;
			*j = p1 - ptr1;
			return true;
		}
	}
	return false;
}

static Boolean FindLastChangeInLVecs(long *ptr1, long *ptr2,
					long L, long *j)
{
/*
	find index of last difference
*/
	long *p1 = ptr1 + L;
	long *p2 = ptr2 + L;
	long i;

	for (i = L; --i >= 0; ) {
		if (*--p1 != *--p2) {
			*j = p1 - ptr1;
			return true;
		}
	}
	return false;
}

void HaveChangedScreenBuff(WORD top, WORD left, WORD bottom, WORD right);

// Draw the screen
void Screen_Draw (void)
{
	char *		screencurrentbuff;
	long		j0;
	long		j1;
	long		copysize;
	long		copyoffset;
	long		copyrows;

	if (vPage2 == 1) {
		screencurrentbuff = (char*) /* get_real_address */ ( ((UBYTE *) RAM) + kMain_Buffer);
	} else {
		screencurrentbuff = (char*) /* get_real_address */ ( ((UBYTE *) RAM) + kAlternate_Buffer);
	}

	if (FindFirstChangeInLVecs((long *)screencurrentbuff,
			(long *)screencomparebuff,
			vMacScreenNumBits / 32, &j0))
	if (FindLastChangeInLVecs((long *)screencurrentbuff,
		(long *)screencomparebuff,
		vMacScreenNumBits / 32, &j1))
	{
		j0 /= (vMacScreenWidth / 32);
		j1 /= (vMacScreenWidth / 32); j1++;

		copyrows = j1 - j0;
		copyoffset = j0 * vMacScreenByteWidth;
		copysize = copyrows * vMacScreenByteWidth;
		MyMoveBytes(screencurrentbuff + copyoffset, screencomparebuff + copyoffset, copysize);

		HaveChangedScreenBuff(j0, 0, j0 + copyrows, vMacScreenWidth);
	}
}

// VIA Interface Functions

UBYTE VIA_GORA6 (void) // Main/Alternate Screen Buffer
{
#ifdef _VIA_Interface_Debug
  printf("VIA ORA6 attempts to be an input\n");
#endif
  return 0;
}

void  VIA_PORA6 (UBYTE Data)
{
	vPage2 = Data;
}

UBYTE VIA_GORB6 (void) // Video Beam in Display
{
	return 0; // Assume it is
}

void  VIA_PORB6 (UBYTE Data)
{
	UnusedParam(Data);
#ifdef _VIA_Interface_Debug
  printf("VIA ORB6 attempts to be an output\n");
#endif
}

