/*
	PROSGLUE.h

	Copyright (C) 2004 Paul C. Pratt

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
	PRivate implementation (stub) of Operating System GLUE.

	If the target operating system hasn't been specified,
	this file is used. It can be used at the starting
	point for a port to a different operating system.

	The main entry point 'main' is at the end of this file.
*/

/*--- some simple utilities ---*/

GLOBALPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
	si5b i;
	ui3b *s = (ui3b *)srcPtr;
	ui3b *d = (ui3b *)destPtr;

	for (i = byteCount; --i >= 0; ) {
		*d++ = *s++;
	}
}

/*--- basic dialogs ---*/

GLOBALPROC MacMsg(char *briefMsg, char *longMsg, blnr fatal)
{
	UnusedParam(briefMsg);
	UnusedParam(longMsg);
	UnusedParam(fatal);
}

GLOBALPROC HaveChangedScreenBuff(si4b top, si4b left, si4b bottom, si4b right)
{
	UnusedParam(top);
	UnusedParam(left);
	UnusedParam(bottom);
	UnusedParam(right);
}

#define NotAfileRef nullpr

void *Drives[NumDrives]; /* open disk image files */

LOCALPROC InitDrives(void)
{
	si4b i;

	for (i = 0; i < NumDrives; ++i) {
		Drives[i] = NotAfileRef;
	}
}

GLOBALFUNC si4b vSonyRead(void *Buffer, ui4b Drive_No, ui5b Sony_Start, ui5b *Sony_Count)
{
	UnusedParam(Buffer);
	UnusedParam(Drive_No);
	UnusedParam(Sony_Start);
	UnusedParam(Sony_Count);
	return -1; /*& figure out what really to return &*/
}

GLOBALFUNC si4b vSonyWrite(void *Buffer, ui4b Drive_No, ui5b Sony_Start, ui5b *Sony_Count)
{
	UnusedParam(Buffer);
	UnusedParam(Drive_No);
	UnusedParam(Sony_Start);
	UnusedParam(Sony_Count);
	return -1; /*& figure out what really to return &*/
}

GLOBALFUNC si4b vSonyGetSize(ui4b Drive_No, ui5b *Sony_Count)
{
	UnusedParam(Drive_No);
	UnusedParam(Sony_Count);
	return -1; /*& figure out what really to return &*/
}

GLOBALFUNC si4b vSonyEject(ui4b Drive_No)
{
	Drives[Drive_No] = NotAfileRef;
	return 0x0000;
}

LOCALFUNC blnr AllocateMacROM(void)
{
	return falseblnr;
}

LOCALFUNC blnr LoadMacRom(void)
{
	return falseblnr;
}

LOCALFUNC blnr AllocateMacRAM (void)
{
	return falseblnr;
}

GLOBALFUNC si3b GetCurrentTimeAdjust(void)
{
	ForceMacOff = trueblnr;
	return TimeAdjust;
}

LOCALPROC DoEndTheSixtieth(void)
{
	if (TimeAdjust < 0) {
		do {
		} while (GetCurrentTimeAdjust() < 0);
	} else if (TimeAdjust > 7) {
		/* emulation not fast enough */
		TimeAdjust = 7;
	}
}

LOCALPROC MainEventLoop(void)
{
	do {
		DoEmulateOneTick();
		--TimeAdjust;
		DoEmulateExtraTime();
		DoEndTheSixtieth();
	} while (! ForceMacOff);
}

LOCALPROC ZapOSGLUVars(void)
{
}

LOCALFUNC blnr InitOSGLU(void)
{
	if (AllocateMacROM())
	if (LoadMacRom())
	if (AllocateMacRAM())
	if (InitEmulation())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
}

int main(void)
{
	ZapOSGLUVars();
	if (InitOSGLU()) {
		MainEventLoop();
	}
	UnInitOSGLU();
	return 0;
}
