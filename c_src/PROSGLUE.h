/*
	PROSGLUE.c

	Copyright (C) 2002 Paul Pratt

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
	PRivate implementation (stub) of Operating System GLUE.

	If the target operating system hasn't been specified,
	this file is used. It can be used at the starting
	point for a port to a different operating system.

	The main entry point 'main' is at the end of this file.
*/

/*--- some simple utilities ---*/

GLOBALPROC MyMoveBytes(anyp srcPtr, anyp destPtr, LONG byteCount)
{
	LONG i;
	UBYTE *s = (UBYTE *)srcPtr;
	UBYTE *d = (UBYTE *)destPtr;

	for (i = byteCount; --i >=0; ) {
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

GLOBALFUNC blnr OkCancelAlert(char *briefMsg, char *longMsg)
{
	UnusedParam(briefMsg);
	UnusedParam(longMsg);
	return falseblnr;
}

GLOBALPROC HaveChangedScreenBuff(WORD top, WORD left, WORD bottom, WORD right)
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
	WORD i;

	for (i = 0; i < NumDrives; ++i) {
		Drives[i] = NotAfileRef;
	}
}

GLOBALFUNC WORD vSonyRead(void *Buffer, UWORD Drive_No, ULONG Sony_Start, ULONG *Sony_Count)
{
	WORD result;

	UnusedParam(Buffer);
	UnusedParam(Sony_Start);
	UnusedParam(Sony_Count);
	if (Drive_No < NumDrives) {
		if (Drives[Drive_No] != NotAfileRef) {
			result = -1; /*& figure out what really to return &*/
		} else {
			result = 0xFFBF; // Say it's offline (-65)
		}
	} else {
		result = 0xFFC8; // No Such Drive (-56)
	}
	return result;
}

GLOBALFUNC WORD vSonyWrite(void *Buffer, UWORD Drive_No, ULONG Sony_Start, ULONG *Sony_Count)
{
	WORD result;

	UnusedParam(Buffer);
	UnusedParam(Sony_Start);
	UnusedParam(Sony_Count);
	if (Drive_No < NumDrives) {
		if (Drives[Drive_No] != NotAfileRef) {
			result = -1; /*& figure out what really to return &*/
		} else {
			result = 0xFFBF; // Say it's offline (-65)
		}
	} else {
		result = 0xFFC8; // No Such Drive (-56)
	}
	return result;
}

GLOBALFUNC blnr vSonyDiskLocked(UWORD Drive_No)
{
	UnusedParam(Drive_No);
	return falseblnr;
}

GLOBALFUNC WORD vSonyGetSize(UWORD Drive_No, ULONG *Sony_Count)
{
	WORD result;

	UnusedParam(Sony_Count);
	if (Drive_No < NumDrives) {
		if (Drives[Drive_No] != NotAfileRef) {
			result = -1; /*& figure out what really to return &*/
		} else {
			result = 0xFFBF; // Say it's offline (-65)
		}
	} else {
		result = 0xFFC8; // No Such Drive (-56)
	}
	return result;
}

GLOBALFUNC WORD vSonyEject(UWORD Drive_No)
{
	WORD result;

	if (Drive_No < NumDrives) {
		if (Drives[Drive_No] != NotAfileRef) {
			result = -1; /*& figure out what really to return &*/
			Drives[Drive_No] = NotAfileRef;
		}
		result = 0x0000;
	} else {
		result = 0xFFC8; // No Such Drive (-56)
	}
	return result;
}

GLOBALFUNC WORD vSonyVerify(UWORD Drive_No)
{
	WORD result;

	if (Drive_No < NumDrives) {
		if (Drives[Drive_No] != NotAfileRef) {
			result = 0x0000; // No Error (0)
		} else {
			result = 0xFFBF; // Say it's offline (-65)
		}
	} else {
		result = 0xFFC8; // No Such Drive (-56)
	}
	return result;
}

GLOBALFUNC WORD vSonyFormat(UWORD Drive_No)
{
	WORD result;

	if (Drive_No < NumDrives) {
		if (Drives[Drive_No] != NotAfileRef) {
			result = 0xFFD4; // Write Protected (-44)
		} else {
			result = 0xFFBF; // Say it's offline (-65)
		}
	} else {
		result = 0xFFC8; // No Such Drive (-56)
	}
	return result;
}

GLOBALFUNC blnr vSonyInserted (UWORD Drive_No)
{
	if (Drive_No >= NumDrives) {
		return falseblnr;
	} else {
		return (Drives[Drive_No] != NotAfileRef);
	}
}

LOCALFUNC blnr FirstFreeDisk(UWORD *Drive_No)
{
	WORD i;

	for (i = 0; i < NumDrives; ++i) {
		if (Drives[i] == NotAfileRef) {
			*Drive_No = i;
			return trueblnr;
		}
	}
	return falseblnr;
}

GLOBALFUNC blnr AnyDiskInserted(void)
{
	WORD i;

	for (i = 0; i < NumDrives; ++i) {
		if (Drives[i] != NotAfileRef) {
			return trueblnr;
		}
	}
	return falseblnr;
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
	kRAM_Size = 0x00400000;
	return falseblnr;
}

GLOBALFUNC ULONG GetMacDateInSecond(void)
{
	return 0;
}

GLOBALFUNC blnr CheckIntSixtieth(blnr overdue)
{
	UnusedParam(overdue);
	RequestMacOff = trueblnr;
	return trueblnr;
}

LOCALPROC ZapOSGLUVars(void)
{
}

LOCALFUNC blnr InitOSGLU(void)
{
	if (AllocateMacROM())
	if (LoadMacRom())
	if (AllocateMacRAM())
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
		ProgramMain();
	}
	UnInitOSGLU();
	return 0;
}
