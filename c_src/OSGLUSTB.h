/*
	OSGLUSTB.c

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
	Operating System GLUe Stub.

	header file for operating system dependent code.
	the same header is used for all platforms.

	This code is descended from Richard F. Bannister's Macintosh
	port of vMac, by Philip Cummins.
*/

#ifdef OSGLUSTB_H
#error "header already included"
#else
#define OSGLUSTB_H
#endif

extern void MacMsg(char *briefMsg, char *longMsg, blnr fatal);

extern blnr OkCancelAlert(char *briefMsg, char *longMsg);

extern WORD vSonyRead(void *Buffer, UWORD Drive_No, ULONG Sony_Start, ULONG *Sony_Count);
extern WORD vSonyWrite(void *Buffer, UWORD Drive_No, ULONG Sony_Start, ULONG *Sony_Count);
extern WORD vSonyEject(UWORD Drive_No);
extern WORD vSonyVerify(UWORD Drive_No);
extern WORD vSonyFormat(UWORD Drive_No);
extern WORD vSonyGetSize(UWORD Drive_No, ULONG *Sony_Count);
extern blnr vSonyInserted (UWORD Drive_No);
extern blnr vSonyDiskLocked(UWORD Drive_No);
extern blnr AnyDiskInserted(void);

extern ULONG GetMacDateInSecond(void);

extern void HaveChangedScreenBuff(WORD top, WORD left, WORD bottom, WORD right);

extern blnr CheckIntSixtieth(blnr overdue);

extern void MyMoveBytes(anyp srcPtr, anyp destPtr, LONG byteCount);
