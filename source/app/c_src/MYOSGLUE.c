/*
	MYOSGLUE.c

	Copyright (C) 2006 Paul C. Pratt

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
	MY Operating System GLUE
*/

#include "CNFGRAPI.h"
#include "SYSDEPNS.h"

#include "MYOSGLUE.h"

IMPORTPROC EmulationReserveAlloc(void);
IMPORTFUNC blnr InitEmulation(void);
IMPORTPROC DoEmulateOneTick(void);
IMPORTFUNC blnr ScreenFindChanges(si3b TimeAdjust,
	si4b *top, si4b *left, si4b *bottom, si4b *right);
IMPORTPROC DoEmulateExtraTime(void);

GLOBALVAR ui3p ROM = nullpr;

#if IncludePbufs
GLOBALVAR ui5b PbufAllocatedMask;
GLOBALVAR ui5b PbufSize[NumPbufs];
#endif

GLOBALVAR ui5b vSonyWritableMask = 0;
GLOBALVAR ui5b vSonyInsertedMask = 0;

#if IncludeSonyRawMode
GLOBALVAR blnr vSonyRawMode = falseblnr;
#endif

#if IncludeSonyNew
GLOBALVAR blnr vSonyNewDiskWanted = falseblnr;
GLOBALVAR ui5b vSonyNewDiskSize;
#endif

#if IncludeSonyNameNew
GLOBALVAR tPbuf vSonyNewDiskName = NotAPbuf;
#endif

GLOBALVAR ui5b CurMacDateInSeconds = 0;
GLOBALVAR ui5b CurMacLatitude = 0;
GLOBALVAR ui5b CurMacLongitude = 0;
GLOBALVAR ui5b CurMacDelta = 0;

GLOBALVAR char *screencomparebuff = nullpr;

#if 0 != vMacScreenDepth
GLOBALVAR blnr UseColorMode = falseblnr;
#endif

#if 0 != vMacScreenDepth
GLOBALVAR blnr ColorMappingChanged = falseblnr;
#endif

#if (0 != vMacScreenDepth) && (vMacScreenDepth < 4)
GLOBALVAR ui4r CLUT_reds[CLUT_size];
GLOBALVAR ui4r CLUT_greens[CLUT_size];
GLOBALVAR ui4r CLUT_blues[CLUT_size];
#endif

GLOBALVAR MyEvtQEl MyEvtQA[MyEvtQSz];
GLOBALVAR ui4r MyEvtQIn = 0;
GLOBALVAR ui4r MyEvtQOut = 0;

LOCALVAR blnr RequestMacOff = falseblnr;

GLOBALVAR blnr ForceMacOff = falseblnr;

GLOBALVAR blnr WantMacInterrupt = falseblnr;

GLOBALVAR blnr WantMacReset = falseblnr;

GLOBALVAR blnr SpeedLimit = (WantInitSpeedValue != -1);

GLOBALVAR ui3b SpeedValue = WantInitSpeedValue;

GLOBALVAR ui4b CurMouseV = 0;
GLOBALVAR ui4b CurMouseH = 0;

#if EnableMouseMotion
LOCALVAR blnr HaveMouseMotion = falseblnr;
#endif

#if MySoundEnabled
#ifndef MySoundFullScreenOnly
#define MySoundFullScreenOnly 0
#endif
#endif

#if IncludePbufs
LOCALFUNC blnr FirstFreePbuf(tPbuf *r)
{
	tPbuf i;

	for (i = 0; i < NumPbufs; ++i) {
		if (! PbufIsAllocated(i)) {
			*r = i;
			return trueblnr;
		}
	}
	return falseblnr;
}
#endif

#if IncludePbufs
LOCALPROC PbufNewNotify(tPbuf Pbuf_No, ui5b count)
{
	PbufSize[Pbuf_No] = count;
	PbufAllocatedMask |= ((ui5b)1 << Pbuf_No);
}
#endif

#if IncludePbufs
LOCALPROC PbufDisposeNotify(tPbuf Pbuf_No)
{
	PbufAllocatedMask &= ~ ((ui5b)1 << Pbuf_No);
}
#endif

LOCALFUNC blnr FirstFreeDisk(tDrive *Drive_No)
{
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		if (! vSonyIsInserted(i)) {
			*Drive_No = i;
			return trueblnr;
		}
	}
	return falseblnr;
}

GLOBALFUNC blnr AnyDiskInserted(void)
{
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		if (vSonyIsInserted(i)) {
			return trueblnr;
		}
	}
	return falseblnr;
}

LOCALPROC DiskInsertNotify(tDrive Drive_No, blnr locked)
{
	vSonyInsertedMask |= ((ui5b)1 << Drive_No);
	if (! locked) {
		vSonyWritableMask |= ((ui5b)1 << Drive_No);
	}
}

LOCALPROC DiskEjectedNotify(tDrive Drive_No)
{
	vSonyWritableMask &= ~ ((ui5b)1 << Drive_No);
	vSonyInsertedMask &= ~ ((ui5b)1 << Drive_No);
}

FORWARDPROC HaveChangedScreenBuff(si4b top, si4b left, si4b bottom, si4b right);

/* Draw the screen */
LOCALPROC Screen_Draw(si3b TimeAdjust)
{
	si4b top;
	si4b left;
	si4b bottom;
	si4b right;

	if (ScreenFindChanges(TimeAdjust,
		&top, &left, &bottom, &right))
	{
		HaveChangedScreenBuff(top, left, bottom, right);
	}

}

LOCALPROC SetLongs(ui5b *p, long n)
{
	long i;

	for (i = n; --i >= 0; ) {
		*p++ = (ui5b) -1;
	}
}

LOCALVAR uimr ReserveAllocOffset;
LOCALVAR ui3p ReserveAllocBigBlock = nullpr;

#define PowOf2(p) ((uimr)1 << (p))
#define Pow2Mask(p) (PowOf2(p) - 1)
#define FloorPow2Mult(i, p) ((i) & (~ Pow2Mask(p)))
#define CeilPow2Mult(i, p) FloorPow2Mult((i) + Pow2Mask(p), (p))
	/* warning - CeilPow2Mult evaluates p twice */

GLOBALPROC ReserveAllocOneBlock(ui3p *p, uimr n, ui3r align, blnr FillOnes)
{
	ReserveAllocOffset = CeilPow2Mult(ReserveAllocOffset, align);
	if (nullpr == ReserveAllocBigBlock) {
		*p = nullpr;
	} else {
		*p = ReserveAllocBigBlock + ReserveAllocOffset;
		if (FillOnes) {
			SetLongs((ui5b *)*p, n / 4);
		}
	}
	ReserveAllocOffset += n;
}

#include "STRCONST.h"

#include "PLATGLUE.h"
