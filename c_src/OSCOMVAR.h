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

extern Boolean HogCPU;

extern unsigned char SpeedLimit;

#define vMacScreenHeight 342
#define vMacScreenWidth 512
#define vMacScreenNumBits ((long)vMacScreenHeight * (long)vMacScreenWidth)
#define vMacScreenNumBytes (vMacScreenNumBits / 8)
#define vMacScreenByteWidth (vMacScreenWidth / 8)

extern char *screencomparebuff;

extern ULONG kRAM_Size; // To work with other headers

extern UWORD *RAM;

#define kROM_Size 0x020000 // ROM size is 128 KB

extern UWORD *ROM;

#define PARAMRAMSize 22

extern UBYTE PARAMRAM[PARAMRAMSize];

extern UWORD CurMouseV;
extern UWORD CurMouseH;
extern UBYTE CurMouseButton;

extern ULONG theKeys[4];

#define NumDrives 3
