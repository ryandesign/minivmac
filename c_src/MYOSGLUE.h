/*
	MYOSGLUE.h

	Copyright (C) 2002 Philip Cummins, Richard F. Bannister, Paul Pratt

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
	MY Operating System GLUE.

	header file for operating system dependent code.
	the same header is used for all platforms.

	This code is descended from Richard F. Bannister's Macintosh
	port of vMac, by Philip Cummins.
*/

#ifdef MYOSGLUE_H
#ifndef AllFiles
#error "header already included"
#endif
#else
#define MYOSGLUE_H
#endif

EXPORTPROC MacMsg(char *briefMsg, char *longMsg, blnr fatal);

EXPORTFUNC blnr OkCancelAlert(char *briefMsg, char *longMsg);

EXPORTPROC MyMoveBytes(anyp srcPtr, anyp destPtr, LONG byteCount);

EXPORTVAR(ULONG, kRAM_Size);
	/* chosen by MYOSGLUE. Must be a power of two */

#define RAMSafetyMarginFudge 4
EXPORTVAR(UWORD, *RAM);
	/*
		allocated by MYOSGLUE to be at least kRAM_Size + RAMSafetyMarginFudge
		bytes. Because of shortcuts taken in ADDRSPAC.c, it is in theory
		possible for the emulator to write up to 3 bytes past kRAM_Size.
	*/

#define kROM_Size 0x020000 // ROM size is 128 KB

EXPORTVAR(UWORD, *ROM);

#define NumDrives 3
EXPORTVAR(ULONG, MountPending);

EXPORTFUNC WORD vSonyRead(void *Buffer, UWORD Drive_No, ULONG Sony_Start, ULONG *Sony_Count);
EXPORTFUNC WORD vSonyWrite(void *Buffer, UWORD Drive_No, ULONG Sony_Start, ULONG *Sony_Count);
EXPORTFUNC WORD vSonyEject(UWORD Drive_No);
EXPORTFUNC WORD vSonyVerify(UWORD Drive_No);
EXPORTFUNC WORD vSonyFormat(UWORD Drive_No);
EXPORTFUNC WORD vSonyGetSize(UWORD Drive_No, ULONG *Sony_Count);
EXPORTFUNC blnr vSonyInserted (UWORD Drive_No);
EXPORTFUNC blnr vSonyDiskLocked(UWORD Drive_No);
EXPORTFUNC blnr AnyDiskInserted(void);

EXPORTFUNC ULONG GetMacDateInSecond(void);

#define vMacScreenHeight 342
#define vMacScreenWidth 512
#define vMacScreenNumBits ((long)vMacScreenHeight * (long)vMacScreenWidth)
#define vMacScreenNumBytes (vMacScreenNumBits / 8)
#define vMacScreenByteWidth (vMacScreenWidth / 8)

EXPORTVAR(char, *screencomparebuff);

EXPORTPROC HaveChangedScreenBuff(WORD top, WORD left, WORD bottom, WORD right);

EXPORTVAR(UWORD, CurMouseV);
EXPORTVAR(UWORD, CurMouseH);
EXPORTVAR(UBYTE, CurMouseButton);

EXPORTVAR(ULONG, theKeys[4]);

EXPORTVAR(blnr, RequestMacOff);
EXPORTVAR(blnr, RequestMacInterrupt);
EXPORTVAR(blnr, RequestMacReset);

EXPORTFUNC blnr CheckIntSixtieth(blnr overdue);
