/*
	OSCOMVAR.h

	Copyright (C) 2001 Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#ifdef OSCOMVAR_H
#error "header already included"
#else
#define OSCOMVAR_H
#endif

#define vMacScreenHeight 342
#define vMacScreenWidth 512
#define vMacScreenNumBits ((long)vMacScreenHeight * (long)vMacScreenWidth)
#define vMacScreenNumBytes (vMacScreenNumBits / 8)
#define vMacScreenByteWidth (vMacScreenWidth / 8)

extern char *screencomparebuff;

extern ULONG kRAM_Size;
	/* chosen by OSGLU. Must be a power of two */

#define RAMSafetyMarginFudge 4
extern UWORD *RAM;
	/*
		allocated by OSGLU to be at least kRAM_Size + RAMSafetyMarginFudge
		bytes. Because of shortcuts taken in ADDRSPAC.c, it is in theory
		possible for the emulator to write up to 3 bytes past kRAM_Size.
	*/

#define kROM_Size 0x020000 // ROM size is 128 KB

extern UWORD *ROM;

#define PARAMRAMSize 22
extern UBYTE PARAMRAM[PARAMRAMSize];
extern blnr PARAMRAMloaded;

extern UWORD CurMouseV;
extern UWORD CurMouseH;
extern UBYTE CurMouseButton;

extern ULONG theKeys[4];

extern blnr RequestMacOff;
extern blnr RequestMacInterrupt;
extern blnr RequestMacReset;

#define NumDrives 3
extern ULONG MountPending;
