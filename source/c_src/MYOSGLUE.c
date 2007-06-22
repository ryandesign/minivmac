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

IMPORTFUNC blnr InitEmulation(void);
IMPORTPROC DoEmulateOneTick(void);
IMPORTFUNC blnr ScreenFindChanges(si3b TimeAdjust,
	si4b *top, si4b *left, si4b *bottom, si4b *right);
IMPORTPROC DoEmulateExtraTime(void);

GLOBALVAR char *screencomparebuff = nullpr;

GLOBALVAR ui4b *RAM = nullpr;

GLOBALVAR ui4b *ROM = nullpr;

GLOBALVAR ui4b CurMouseV = 0;
GLOBALVAR ui4b CurMouseH = 0;
GLOBALVAR ui3b CurMouseButton = falseblnr;

GLOBALVAR ui5b theKeys[4];

#ifndef WantInitSpeedValue
#define WantInitSpeedValue 3
#endif

GLOBALVAR ui3b SpeedValue = WantInitSpeedValue;

GLOBALVAR blnr SpeedLimit = (WantInitSpeedValue != -1);

#if EnableMouseMotion
GLOBALVAR blnr HaveMouseMotion = falseblnr;
GLOBALVAR ui4b MouseMotionV = 0;
GLOBALVAR ui4b MouseMotionH = 0;
#endif

#if MySoundEnabled
#ifndef MySoundFullScreenOnly
#define MySoundFullScreenOnly 0
#endif
#endif

#ifndef EnableDragDrop
#define EnableDragDrop 1
#endif

LOCALVAR blnr RequestMacOff = falseblnr;

GLOBALVAR blnr ForceMacOff = falseblnr;

GLOBALVAR blnr WantMacInterrupt = falseblnr;

GLOBALVAR blnr WantMacReset = falseblnr;

GLOBALVAR ui5b vSonyWritableMask = 0;
GLOBALVAR ui5b vSonyInsertedMask = 0;
GLOBALVAR ui5b vSonyMountedMask = 0;

#if IncludeSonyRawMode
GLOBALVAR blnr vSonyRawMode = falseblnr;
#endif

#if IncludePbufs
GLOBALVAR ui5b PbufAllocatedMask;
GLOBALVAR ui5b PbufSize[NumPbufs];
#endif

#if IncludeSonyNew
GLOBALVAR blnr vSonyNewDiskWanted = falseblnr;
GLOBALVAR ui5b vSonyNewDiskSize;
#endif

#if IncludeSonyNameNew
GLOBALVAR ui4b vSonyNewDiskName = NotAPbuf;
#endif

GLOBALVAR ui5b CurMacDateInSeconds = 0;
GLOBALVAR ui5b CurMacLatitude = 0;
GLOBALVAR ui5b CurMacLongitude = 0;
GLOBALVAR ui5b CurMacDelta = 0;

#if IncludePbufs
LOCALFUNC blnr FirstFreePbuf(ui4b *r)
{
	si4b i;

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
LOCALPROC PbufNewNotify(ui4b Pbuf_No, ui5b count)
{
	PbufSize[Pbuf_No] = count;
	PbufAllocatedMask |= ((ui5b)1 << Pbuf_No);
}
#endif

#if IncludePbufs
LOCALPROC PbufDisposeNotify(ui4b Pbuf_No)
{
	PbufAllocatedMask &= ~ ((ui5b)1 << Pbuf_No);
}
#endif

LOCALFUNC blnr FirstFreeDisk(ui4b *Drive_No)
{
	si4b i;

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
	si4b i;

	for (i = 0; i < NumDrives; ++i) {
		if (vSonyIsInserted(i)) {
			return trueblnr;
		}
	}
	return falseblnr;
}

LOCALPROC DiskInsertNotify(ui4b Drive_No, blnr locked)
{
	vSonyInsertedMask |= ((ui5b)1 << Drive_No);
	if (! locked) {
		vSonyWritableMask |= ((ui5b)1 << Drive_No);
	}
}

LOCALPROC DiskEjectedNotify(ui4b Drive_No)
{
	vSonyWritableMask &= ~ ((ui5b)1 << Drive_No);
	vSonyInsertedMask &= ~ ((ui5b)1 << Drive_No);
	vSonyMountedMask &= ~ ((ui5b)1 << Drive_No);
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

#include "STRCONST.h"

#include "PLATGLUE.h"
