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

extern void MacMsg(Str255 briefMsg, Str255 longMsg, Boolean fatal);

extern void zapOSGLUMACVars(void);
extern void ResetOSGLUMAC(void);
extern Boolean InitOSGLUSTB(void);
extern void UnInitProgram(void);

extern WORD vSonyRead(void *Buffer, UWORD Drive_No, ULONG Sony_Start, ULONG *Sony_Count);
extern WORD vSonyWrite(void *Buffer, UWORD Drive_No, ULONG Sony_Start, ULONG *Sony_Count);
extern WORD vSonyEject(UWORD Drive_No);
extern WORD vSonyVerify(UWORD Drive_No);
extern WORD vSonyFormat(UWORD Drive_No);
extern WORD vSonyGetSize(UWORD Drive_No, ULONG *Sony_Count);
extern Boolean vSonyInserted (UWORD Drive_No);
extern Boolean vSonyDiskLocked(UWORD Drive_No);
extern Boolean AnyDiskInserted(void);

ULONG GetMacDateInSecond(void);

void HaveChangedScreenBuff(WORD top, WORD left, WORD bottom, WORD right);

Boolean CheckIntSixtieth(void);
void WaitForIntSixtieth(void);
Boolean InitSixtiethInt(void);

void MyMoveBytes(Ptr srcPtr, Ptr destPtr, LONG byteCount);

extern Boolean RAM_Init (void);
extern Boolean Load_ROM(void);
extern UBYTE RTC_Load (void);
extern void RTC_Save0 (void);
