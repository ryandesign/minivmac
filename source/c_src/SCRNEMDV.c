/*
	SCRNEMDV.c

	Copyright (C) 2006 Philip Cummins, Richard F. Bannister, Paul C. Pratt

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
	SCReeN EMulated DeVice

	Emulation of the screen in the Mac Plus.

	This code descended from "Screen-MacOS.c" in Richard F. Bannister's
	Macintosh port of vMac, by Philip Cummins.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"
#include "MYOSGLUE.h"
#include "ENDIANAC.h"
#include "ADDRSPAC.h"
#endif

#include "SCRNEMDV.h"

LOCALFUNC blnr FindFirstChangeInLVecs(long *ptr1, long *ptr2,
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
			return trueblnr;
		}
	}
	return falseblnr;
}

LOCALFUNC blnr FindLastChangeInLVecs(long *ptr1, long *ptr2,
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
			return trueblnr;
		}
	}
	return falseblnr;
}

#define kMain_Offset      0x5900
#define kAlternate_Offset 0xD900
#define kMain_Buffer      (kRAM_Size - kMain_Offset)
#define kAlternate_Buffer (kRAM_Size - kAlternate_Offset)

LOCALVAR unsigned long NextDrawRow = 0;

GLOBALFUNC blnr ScreenFindChanges(si3b TimeAdjust,
	si4b *top, si4b *left, si4b *bottom, si4b *right)
{
	char *screencurrentbuff;
	long j0;
	long j1;
	long copysize;
	long copyoffset;
	long copyrows;
	unsigned long LimitDrawRow;
	unsigned long MaxRowsDrawnPerTick;

	if (TimeAdjust < 4) {
		MaxRowsDrawnPerTick = vMacScreenHeight;
	} else if (TimeAdjust < 6) {
		MaxRowsDrawnPerTick = vMacScreenHeight / 2;
	} else {
		MaxRowsDrawnPerTick = vMacScreenHeight / 4;
	}

	if (SCRNvPage2 == 1) {
		screencurrentbuff = (char *) get_ram_address(kMain_Buffer);
	} else {
		screencurrentbuff = (char *) get_ram_address(kAlternate_Buffer);
	}

	if (! FindFirstChangeInLVecs((long *)screencurrentbuff + NextDrawRow * (vMacScreenWidth / 32),
			(long *)screencomparebuff + NextDrawRow * (vMacScreenWidth / 32),
			((long)(vMacScreenHeight - NextDrawRow) * (long)vMacScreenWidth) / 32, &j0))
	{
		NextDrawRow = 0;
		return falseblnr;
	}
	j0 /= (vMacScreenWidth / 32);
	j0 += NextDrawRow;
	LimitDrawRow = j0 + MaxRowsDrawnPerTick;
	if (LimitDrawRow >= vMacScreenHeight) {
		LimitDrawRow = vMacScreenHeight;
		NextDrawRow = 0;
	} else {
		NextDrawRow = LimitDrawRow;
	}
	if (! FindLastChangeInLVecs((long *)screencurrentbuff,
		(long *)screencomparebuff,
		((long)LimitDrawRow * (long)vMacScreenWidth) / 32, &j1))
	{
		return falseblnr;
	} else {
		j1 /= (vMacScreenWidth / 32); j1++;

		copyrows = j1 - j0;
		copyoffset = j0 * vMacScreenByteWidth;
		copysize = copyrows * vMacScreenByteWidth;
		MyMoveBytes((anyp)screencurrentbuff + copyoffset, (anyp)screencomparebuff + copyoffset, copysize);

		*top = j0;
		*left = 0;
		*bottom = j0 + copyrows;
		*right = vMacScreenWidth;
		return trueblnr;
	}
}
